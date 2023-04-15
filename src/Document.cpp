#include "Document.hpp"

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

}