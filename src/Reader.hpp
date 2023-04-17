#ifndef CPPJSON_READER_HPP
#define CPPJSON_READER_HPP
#include "Value.hpp"
#include "Exception.hpp"
#include "Nocopyable.hpp"
#include <limits>
#include <cmath>
#include <stdexcept>

namespace cppjson {

class Reader : public Nocopyable {
public:
    template <typename ReadStream, typename Handler>
    static ParseError parse(ReadStream& is, Handler& handler) {
        try {
            parseWhiteSpace(is);
            parseValue(is, handler);
            parseWhiteSpace(is);
            if (is.hasNext()) {
                throw Exception(PARSE_ROOT_NOT_SINGULAR);
            }
            return PARSE_OK;
        } catch (Exception& e) {
            return e.getError();
        }
    }

private:
#define CALL(expr) do {if (!(expr)) throw Exception(PARSE_USER_STOPPED); } while(0)

    template <typename ReadStream>
    static void parseWhiteSpace(ReadStream& is) {
        while (is.hasNext()) {
            char ch = is.peek();
            if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n') {
                is.next();
            } else {
                break;
            }
        }
    }

    template <typename ReadStream, typename Handler>
    static void parseValue(ReadStream& is, Handler& handler) {
        if (!is.hasNext())
            throw Exception(PARSE_EXPECT_VALUE);//没有可解析的json

        switch (is.peek()) {
            case 'n': return parseLiteral(is, handler, "null", TYPE_NULL);
            case 't': return parseLiteral(is, handler, "true", TYPE_BOOL);
            case 'f': return parseLiteral(is, handler, "false", TYPE_BOOL);
            case '\"': return parseString(is, handler, false);
            case '[': return parseArray(is, handler);
            case '{': return parseObject(is, handler);
            default:  return parseNumber(is, handler);
        }
    }

    template <typename ReadStream, typename Handler>
    static void parseLiteral(ReadStream& is, Handler& handler, const char* literal, ValueType type) {
        char c = *literal;

        is.assertNext(*literal++);
        while (*literal != '\0' && *literal == is.peek()) {
            literal++;
            is.next();
        }
        if (*literal == '\0') {
            switch (type) {
                case TYPE_NULL:
                    CALL(handler.Null());
                    return;
                case TYPE_BOOL:
                    CALL(handler.Bool(c == 't'));
                    return;
                case TYPE_DOUBLE:
                    CALL(handler.Double(c == 'N' ? NAN : INFINITY));
                    return;
                default:
                    assert(false && "bad type");//never reach
            }
        }
        throw Exception(PARSE_BAD_VALUE);
    }

    template <typename ReadStream, typename Handler>
    static void parseNumber(ReadStream& is, Handler& handler) {
        // parse 'NaN' (Not a Number) && 'Infinity'
        if (is.peek() == 'N') {
            parseLiteral(is, handler, "NaN", TYPE_DOUBLE);
            return;
        }
        else if (is.peek() == 'I') {
            parseLiteral(is, handler, "Infinity", TYPE_DOUBLE);
            return;
        }

        auto start = is.getIter();

        if (is.peek() == '-') {
            is.next();
        }

        if (is.peek() == '0') {
            is.next();
            if (isdigit(is.peek()))
                throw Exception(PARSE_BAD_VALUE);
        } else if (isDigit19(is.peek())) {
            is.next();
            while (isDigit(is.peek()))
                is.next();
        } else {
            throw Exception(PARSE_BAD_VALUE);
        }

        auto expectType = TYPE_NULL;

        if (is.peek() == '.') {
            expectType = TYPE_DOUBLE;//浮点数
            is.next();
            if (!isDigit(is.peek())) {
                throw Exception(PARSE_BAD_VALUE);
            }
            is.next();
            while (isDigit(is.peek())) {
                is.next();
            }
        }

        if (is.peek() == 'e' || is.peek() == 'E') {
            expectType = TYPE_DOUBLE;//浮点数
            is.next();
            if (is.peek() == '+' || is.peek() == '-') {
                is.next();
            }

            if (!isDigit(is.peek())) {
                throw Exception(PARSE_BAD_VALUE);
            }

            is.next();
            while (isDigit(is.peek())) {
                is.next();
            }
        }

        // int64 or int32 ? 这个标志可以不写
        if (is.peek() == 'i') {
            is.next();
            if (expectType == TYPE_DOUBLE) {//整数，非浮点数
                throw Exception(PARSE_BAD_VALUE);
            }
            switch (is.next()) {
                case '3':
                    if (is.next() != '2') {
                        throw Exception(PARSE_BAD_VALUE);
                    }
                    expectType = TYPE_INT32;
                    break;
                case '6':
                    if (is.next() != '4') {
                        throw Exception(PARSE_BAD_VALUE);
                    }
                    expectType = TYPE_INT64;
                    break;
                default:
                    throw Exception(PARSE_BAD_VALUE);
            }
        }

        auto end = is.getIter();
        if (start == end) {
            throw Exception(PARSE_BAD_VALUE);
        }

        try {
            //
            // std::stod() && std::stoi() are bad ideas,
            // because new string buffer is needed
            //
            std::size_t idx;
            if (expectType == TYPE_DOUBLE) {
                double d = __gnu_cxx::__stoa(&std::strtod, "stod", &*start, &idx);
                assert(start + idx == end);
                CALL(handler.Double(d));
            }

            else {
                int64_t i64 = __gnu_cxx::__stoa(&std::strtol, "stol", &*start, &idx, 10);
                if (expectType == TYPE_INT64) {
                    CALL(handler.Int64(i64));
                } else if (expectType == TYPE_INT32) {
                    if (i64 > std::numeric_limits<int32_t>::max() || i64 < std::numeric_limits<int32_t>::min()) {
                        throw std::out_of_range("int32_t overflow");
                    }
                    CALL(handler.Int32(static_cast<int32_t>(i64)));
                } else if (i64 <= std::numeric_limits<int32_t>::max() && i64 >= std::numeric_limits<int32_t>::min()) {
                    CALL(handler.Int32(static_cast<int32_t>(i64)));
                } else {
                    CALL(handler.Int64(i64));
                }
            }
        } catch (std::exception &e) {
            throw Exception(PARSE_NUMBER_TOO_BIG);
        }
    }

    template <typename ReadStream, typename Handler>
    static void parseString(ReadStream& is, Handler& handler, bool isKey) {
        is.assertNext('\"');
        std::string buffer;
        while (is.hasNext()) {
            switch (char ch = is.next()) {
                case '"':
                    if (isKey) {
                        CALL(handler.Key(std::move(buffer)));
                    }
                    else {
                        CALL(handler.String(std::move(buffer)));
                    }
                    return;
                case '\x01'...'\x1f'://小于0x20
                    throw Exception(PARSE_BAD_STRING_CHAR);
                case '\\'://转义符
                    switch (is.next()) {
                        case '"':  buffer.push_back('"');  break;
                        case '\\': buffer.push_back('\\'); break;
                        case '/':  buffer.push_back('/');  break;
                        case 'b':  buffer.push_back('\b'); break;
                        case 'f':  buffer.push_back('\f'); break;
                        case 'n':  buffer.push_back('\n'); break;
                        case 'r':  buffer.push_back('\r'); break;
                        case 't':  buffer.push_back('\t'); break;
                        case 'u': {
                            // unicode stuff from Milo's tutorial
                            unsigned u = parseHex4(is);
                            if (u >= 0xD800 && u <= 0xDBFF) {
                                if (is.next() != '\\')
                                    throw Exception(PARSE_BAD_UNICODE_SURROGATE);
                                if (is.next() != 'u')
                                    throw Exception(PARSE_BAD_UNICODE_SURROGATE);
                                unsigned u2 = parseHex4(is);
                                if (u2 >= 0xDC00 && u2 <= 0xDFFF)
                                    u = 0x10000 + (u - 0xD800) * 0x400 + (u2 - 0xDC00);
                                else
                                    throw Exception(PARSE_BAD_UNICODE_SURROGATE);
                            }
                            encodeUtf8(buffer, u);
                            break;
                        }
                        default: throw Exception(PARSE_BAD_STRING_ESCAPE);
                    }
                    break;
                default: buffer.push_back(ch);//普通字符
            }
        }
        throw Exception(PARSE_MISS_QUOTATION_MARK);
    }

    template <typename ReadStream, typename Handler>
    static void parseArray(ReadStream& is, Handler& handler) {
        CALL(handler.StartArray());
        
        is.assertNext('[');
        parseWhiteSpace(is);

        if (is.peek() == ']') {
            is.next();
            CALL(handler.EndArray());
            return;
        }

        while (true) {
            parseValue(is, handler);
            parseWhiteSpace(is);
            switch (is.next()) {
                case ',':
                    parseWhiteSpace(is);
                    break;
                case ']':
                    CALL(handler.EndArray());
                    return;
                default:
                    throw Exception(PARSE_MISS_COMMA_OR_SQUARE_BRACKET);
            }
        }
    }

    template <typename ReadStream, typename Handler>
    static void parseObject(ReadStream& is, Handler& handler) {
        CALL(handler.StartObject());
        is.assertNext('{');
        parseWhiteSpace(is);
        if (is.peek() == '}') {
            is.next();
            CALL(handler.EndObject());
            return;
        }

        while (true) {
            if (is.peek() != '"') {
                throw Exception(PARSE_MISS_KEY);
            }
            parseString(is, handler, true);//解析key值

            // parse ':'
            parseWhiteSpace(is);
            if (is.next() != ':') {
                throw Exception(PARSE_MISS_COLON);
            }
            parseWhiteSpace(is);

            // go on
            parseValue(is, handler);
            parseWhiteSpace(is);
            switch (is.next()) {
                case ',':
                    parseWhiteSpace(is);
                    break;
                case '}':
                    CALL(handler.EndObject());
                    return;
                default:
                    throw Exception(PARSE_MISS_COMMA_OR_CURLY_BRACKET);
            }
        }
    }
#undef CALL

public:
    static bool isDigit(char ch) { return ch >= '0' && ch <= '9'; }
    static bool isDigit19(char ch) { return ch >= '1' && ch <= '9'; }
private:
    static void encodeUtf8(std::string& buffer, unsigned u);
    template <typename ReadStream>
    static unsigned parseHex4(ReadStream& is) {
        // unicode stuff from Milo's tutorial
        unsigned u = 0;
        for (int i = 0; i < 4; i++) {
            u <<= 4;
            switch (char ch = is.next()) {
                case '0'...'9': u |= ch - '0'; break;
                case 'a'...'f': u |= ch - 'a' + 10; break;
                case 'A'...'F': u |= ch - 'A' + 10; break;
                default: throw Exception(PARSE_BAD_UNICODE_HEX);
            }
        }
        return u;
    }
    
};

}

#endif