#ifndef CPPJSON_DOCUMENT_HPP
#define CPPJSON_DOCUMENT_HPP

#include "Value.hpp"
#include "Reader.hpp"
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
    Document():m_seeValue(false) {}
    
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

private:
    Value* addValue(Value&& value) {
        ValueType type = value.getType();
        if (m_seeValue) {
            assert(!m_stack.empty() && "root not singular");
        } else {
            assert(m_type == TYPE_NULL);
            m_seeValue = true;
            m_type = value.m_type;
            m_i64 = value.m_i64;
            value.m_type = TYPE_NULL;
            value.m_i64 = 0;
            return this;
        }
        return nullptr;
    }
private:
    struct Level {
        explicit Level(Value* value): m_value(value), m_valueCount(0) {}
        Value* m_value;
        int m_valueCount;
    };
private:
    bool m_seeValue;
    std::vector<Level> m_stack;
    Value m_key;
};

}

#endif