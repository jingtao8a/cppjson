#ifndef CPPJSON_DOCUMENT_HPP
#define CPPJSON_DOCUMENT_HPP

#include "Value.hpp"
#include "Reader.hpp"
#include "Exception.hpp"
#include <cassert>
#include <string>

namespace cppjson {

class Document: public Value {
public:
    ParseError parse(const char* json, size_t len) {
        return parse(std::string(json, len));
    }

    ParseError parse(std::string json) {
        StringReadStream is(json);
        return parseStream(is);
    }

    template <typename ReadStream>
    ParseError parseStream(ReadStream& is) {
        return Reader::parse(is, *this);
    }
public:
    bool Null() {
        addValue(Value(TYPE_NULL));
        return true;
    }

    bool Bool(bool b) {
        addValue(Value(b));
        return true;
    }

    bool Int32(int32_t i32) {
        addValue(Value(i32));
        return true;
    }

    bool Int64(int64_t i64) {
        addValue(Value(i64));
        return true;
    }

    bool Double(double d) {
        addValue(Value(d));
        return true;
    }

    bool String(std::string s) {
        addValue(Value(s));
        return true;
    }

    bool StartArray() {
        addValue(Value(TYPE_ARRAY));
        return true;
    }

    bool EndArray() {
        assert(!m_stack.empty());
        assert(m_stack.back()->m_type == TYPE_ARRAY);
        m_stack.pop_back();
        return true;
    }

    bool Key(std::string s) {
        m_key = std::move(Value(s));
        return true;
    }

    bool StartObject() {
        addValue(Value(TYPE_OBJECT));
        return true;
    }

    bool EndObject() {
        assert(!m_stack.empty());
        assert(m_stack.back()->m_type == TYPE_OBJECT);
        m_stack.pop_back();
        return true;
    }
private:
    void addValue(Value&& value);

private:
    std::vector<Value*> m_stack;
    Value m_key;
};

}

#endif