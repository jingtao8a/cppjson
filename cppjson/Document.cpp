#include "Document.hpp"
#include "StringReadStream.hpp"

namespace cppjson {
void Document::addValue(Value&& value) {
    if (m_stack.empty()) {
        assert(m_type == TYPE_NULL);
        //移动
        m_type = value.m_type;
        m_i64 = value.m_i64;
        value.m_type = TYPE_NULL;
        value.m_i64 = 0;
            
        if (m_type == TYPE_ARRAY || m_type == TYPE_OBJECT) {
            m_stack.push_back(this);
        }
        return;
    }

    auto tmpPtr = m_stack.back();
    if (tmpPtr->m_type == TYPE_ARRAY) {
        tmpPtr->m_a->push_back(value);
        if (value.m_type == TYPE_ARRAY || value.m_type == TYPE_OBJECT) {
            m_stack.push_back(&tmpPtr->m_a->back());
        }
    } else if (tmpPtr->m_type == TYPE_OBJECT) {
        if (m_key.m_type != TYPE_STRING) {
            throw Exception(PARSE_MISS_KEY);
        }
        tmpPtr->m_o->push_back(Member(m_key, value));
        m_key.~Value();
        if (value.m_type == TYPE_ARRAY || value.m_type == TYPE_OBJECT) {
            m_stack.push_back(&tmpPtr->m_o->back().m_value);
        }
    }
    return;
}

bool Document::Null() {
    addValue(Value(TYPE_NULL));
    return true;
}

bool Document::Bool(bool b) {
    addValue(Value(b));
    return true;
}

bool Document::Int32(int32_t i32) {
    addValue(Value(i32));
    return true;
}

bool Document::Int64(int64_t i64) {
    addValue(Value(i64));
    return true;
}

bool Document::Double(double d) {
    addValue(Value(d));
    return true;
}

bool Document::String(std::string s) {
    addValue(Value(s));
    return true;
}

bool Document::StartArray() {
    addValue(Value(TYPE_ARRAY));
    return true;
}

bool Document::EndArray() {
    assert(!m_stack.empty());
    assert(m_stack.back()->m_type == TYPE_ARRAY);
    m_stack.pop_back();
    return true;
}

bool Document::Key(std::string s) {
    m_key = std::move(Value(s));
    return true;
}

bool Document::StartObject() {
    addValue(Value(TYPE_OBJECT));
    return true;
}

bool Document::EndObject() {
    assert(!m_stack.empty());
    assert(m_stack.back()->m_type == TYPE_OBJECT);
    m_stack.pop_back();
    return true;
}

ParseError Document::parse(const char* json, size_t len) {
    return parse(std::string(json, len));
}

ParseError Document::parse(std::string json) {
    StringReadStream is(json);
    return parseStream(is);
}

}