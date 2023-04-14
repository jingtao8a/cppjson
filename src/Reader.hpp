#ifndef CPPJSON_READER_HPP
#define CPPJSON_READER_HPP
#include "Value.hpp"
#include "Exception.hpp"
#include "Nocopyable.hpp"
#include "StringReadStream.hpp"
#include "FileReadStream.hpp"
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
            // case '\"': return parseString(is, handler, false);
            // case '[': return parseArray(is, handler);
            // case '{': return parseObject(is, handler);
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
#undef CALL


private:
    static bool isDigit(char ch) { return ch >= '0' && ch <= '9'; }
    static bool isDigit19(char ch) { return ch >= '1' && ch <= '9'; }
    // static void encodeUtf8(std::string& buffer, unsigned u);
};

}

#endif