#include "../src/Document.hpp"
#include "gtest/gtest.h"


#define TEST_BOOL(type, value, json) do { \
    cppjson::Document doc; \
    cppjson::ParseError err = doc.parse(json); \
    EXPECT_EQ(err, cppjson::PARSE_OK); \
    EXPECT_EQ(doc.getType(), type); \
    EXPECT_EQ(doc.getBool(), value); \
} while(false)

#define TEST_NULL(json) do { \
    cppjson::Document doc; \
    cppjson::ParseError err = doc.parse(json); \
    EXPECT_EQ(err, cppjson::PARSE_OK); \
    EXPECT_EQ(doc.getType(), cppjson::TYPE_NULL); \
} while(false)

#define TEST_DOUBLE(num, json) do { \
    cppjson::Document doc; \
    cppjson::ParseError err = doc.parse(json); \
    EXPECT_EQ(err, cppjson::PARSE_OK); \
    EXPECT_EQ(doc.getType(), cppjson::TYPE_DOUBLE); \
    if (std::isnan(num)) { EXPECT_TRUE(std::isnan(doc.getDouble())); } \
    else                 { EXPECT_EQ(doc.getDouble(), num); }\
} while(false)

#define TEST_INT32(num, json) do { \
    cppjson::Document doc; \
    cppjson::ParseError err = doc.parse(json); \
    EXPECT_EQ(err, cppjson::PARSE_OK); \
    EXPECT_EQ(doc.getType(), cppjson::TYPE_INT32); \
    EXPECT_EQ(doc.getInt32(), num); \
} while(false)

#define TEST_INT64(num, json) do { \
    cppjson::Document doc; \
    cppjson::ParseError err = doc.parse(json); \
    EXPECT_EQ(err, cppjson::PARSE_OK); \
    EXPECT_EQ(doc.getType(), cppjson::TYPE_INT64); \
    EXPECT_EQ(doc.getInt64(), num); \
} while(false)

#define TEST_STRING(str, json) do { \
    cppjson::Document doc; \
    cppjson::ParseError err = doc.parse(json); \
    EXPECT_EQ(err, cppjson::PARSE_OK); \
    EXPECT_EQ(doc.getType(), cppjson::TYPE_STRING); \
    EXPECT_EQ(doc.getString(), str); \
} while(false)

TEST(json_value, null) {
    TEST_NULL("null");
    TEST_NULL(" null ");
}

TEST(json_value, bool_) {
    TEST_BOOL(cppjson::TYPE_BOOL, true, "true");
    TEST_BOOL(cppjson::TYPE_BOOL, true, "\r\ntrue ");
    TEST_BOOL(cppjson::TYPE_BOOL, false, " false");
    TEST_BOOL(cppjson::TYPE_BOOL, false, "false ");
}

TEST(json_value, number) {
    TEST_DOUBLE(0.0, "0.0");
    TEST_DOUBLE(0.0, "-0.0e000");
    TEST_DOUBLE(876.543e21, "876.543e+00021");
    TEST_DOUBLE(-111.000e-010, "-111.000e-010");
/* https://en.wikipedia.org/wiki/Double-precision_floating-point_format */
    TEST_DOUBLE(1.0000000000000002, "1.0000000000000002");
    TEST_DOUBLE(-1.0000000000000002, "-1.0000000000000002");
    // fixme stod not pass edge case
//    TEST_DOUBLE(4.9406564584124654e-323, "4.9406564584124654E-323");
//    TEST_DOUBLE(4.9406564584124654e-324, "4.9406564584124654e-324");
//    TEST_DOUBLE(2.2250738585072009e-308, "2.2250738585072009e-308");
//    TEST_DOUBLE(-2.2250738585072009e-308, "-2.2250738585072009e-308");
    TEST_DOUBLE(2.2250738585072014e-308, "2.2250738585072014e-308");
    TEST_DOUBLE(-2.2250738585072014e-308, "-2.2250738585072014e-308");
    TEST_DOUBLE(1.7976931348623157e308, "1.7976931348623157e308");
    TEST_DOUBLE(-1.7976931348623157e308, "-1.7976931348623157e308");
    TEST_DOUBLE(INFINITY, "Infinity");
    TEST_DOUBLE(NAN, "NaN");

    TEST_INT32(0, "0");
    TEST_INT32(-0, "0");
    TEST_INT32(1, "1");
    TEST_INT32(-1, "-1");
    TEST_INT32(2147483647, "2147483647");
    TEST_INT32(-2147483648, "-2147483648");
    TEST_INT32(0, "0i32");
    TEST_INT32(-0, "0i32");
    TEST_INT32(1, "1i32");
    TEST_INT32(-1, "-1i32");
    TEST_INT32(2147483647, "2147483647i32");
    TEST_INT32(-2147483648, "-2147483648i32");

    TEST_INT64(2147483648LL, "2147483648");
    TEST_INT64(-2147483649LL, "-2147483649");
    TEST_INT64(std::numeric_limits<int64_t>::max(), "9223372036854775807");
    TEST_INT64(std::numeric_limits<int64_t>::min(), "-9223372036854775808");

    TEST_INT64(2147483648LL, "2147483648i64");
    TEST_INT64(-2147483649LL, "-2147483649i64");
    TEST_INT64(std::numeric_limits<int64_t>::max(), "9223372036854775807i64");
    TEST_INT64(std::numeric_limits<int64_t>::min(), "-9223372036854775808i64");

    TEST_INT64(0, "0i64");
    TEST_INT64(-0, "0i64");
    TEST_INT64(1, "1i64");
    TEST_INT64(-1, "-1i64");
    TEST_INT64(2147483647, "2147483647i64");
    TEST_INT64(-2147483648, "-2147483648i64");
}

TEST(json_value, string_)
{
    TEST_STRING("", "\"\"");
    TEST_STRING("abcd", "\"abcd\"");
    TEST_STRING("\\", "\"\\\\\"");
    TEST_STRING("\"", "\"\\\"\"");
    TEST_STRING("/\b\f\n\r\t", "\"\\/\\b\\f\\n\\r\\t\"");
    TEST_STRING(std::string("蛤\0蛤\0蛤", 11), "\"蛤\\u0000蛤\\u0000蛤\"");
    // TEST_STRING("蛤\0蛤\0蛤", "\"蛤\0蛤\0蛤\"");
    TEST_STRING(std::string("\0", 1), "\"\\u0000\"");
    TEST_STRING("\x24", "\"\\u0024\"");         			/* Dollar $ */
    TEST_STRING("\xC2\xA2", "\"\\u00A2\"");     			/* Cents ¢ */
    TEST_STRING("\xE2\x82\xAC", "\"\\u20AC\""); 			/* Euro € */
    TEST_STRING("\xF0\x9D\x84\x9E", "\"\\uD834\\uDD1E\""); /* G clef  𝄞 */
    TEST_STRING("\xF0\x9D\x84\x9E", "\"\\ud834\\udd1e\""); /* G clef  𝄞 */
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}