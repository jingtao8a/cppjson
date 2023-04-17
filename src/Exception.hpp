#ifndef CPPJSON_EXCEPTION_HPP
#define CPPJSON_EXCEPTION_HPP

#include <exception>

namespace cppjson {

#define ERROR_MAP(XX) \
    XX(OK, "ok") \
    XX(ROOT_NOT_SINGULAR, "root not singular") \
    XX(BAD_VALUE, "bad value") \
    XX(EXPECT_VALUE, "expect value") \
    XX(NUMBER_TOO_BIG, "number too big") \
    XX(BAD_STRING_CHAR, "bad character") \
    XX(BAD_STRING_ESCAPE, "bad escape") \
    XX(BAD_UNICODE_HEX, "bad unicode hex") \
    XX(BAD_UNICODE_SURROGATE, "bad unicode surrogate") \
    XX(MISS_QUOTATION_MARK, "miss quotation mark") \
    XX(MISS_COMMA_OR_SQUARE_BRACKET, "miss comma or square bracket") \
    XX(MISS_KEY, "miss key") \
    XX(MISS_COLON, "miss colon") \
    XX(MISS_COMMA_OR_CURLY_BRACKET, "miss comma or curly bracket") \
    XX(USER_STOPPED, "user stopped parse")

enum ParseError {
#define GEN_ERRNO(e, s) PARSE_##e,
    ERROR_MAP(GEN_ERRNO)
#undef GEN_ERRNO
};

class Exception : std::exception {
public:
    explicit Exception(ParseError err):m_err(err) {}
    const char* what();
    ParseError getError() {
      return m_err;
    }
private:
    ParseError m_err;
};

const char* ParseErrorStr(ParseError err);

}

#endif