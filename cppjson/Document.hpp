#ifndef CPPJSON_DOCUMENT_HPP
#define CPPJSON_DOCUMENT_HPP

#include <cassert>
#include <string>
#include "Reader.hpp"

namespace cppjson {

class Document: public Value {
public:
    ParseError parse(const char* json, size_t len);
    ParseError parse(std::string json);

    template <typename ReadStream>
    ParseError parseStream(ReadStream& is) {
        return Reader::parse(is, *this);
    }
public:
    bool Null();
    bool Bool(bool b);
    bool Int32(int32_t i32);
    bool Int64(int64_t i64);
    bool Double(double d);
    bool String(std::string s);
    bool StartArray();
    bool EndArray();
    bool Key(std::string s);
    bool StartObject();
    bool EndObject();
private:
    void addValue(Value&& value);

private:
    std::vector<Value*> m_stack;
    Value m_key;
};

}

#endif