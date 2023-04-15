#include "Value.hpp"
#include <utility>
#include <algorithm>

namespace cppjson {
Value::Value(ValueType type) : m_type(type), m_i64(0) {
    switch (type) {
        case TYPE_STRING:
            m_s = new std::vector<char>();
            break;
        case TYPE_ARRAY:
            m_a = new std::vector<Value>();
            break;
        case TYPE_OBJECT:
            m_o = new std::vector<Member>();
            break;
    }
}
void Value::copyHelper(const Value& rhs) {
    m_type = rhs.m_type;
    switch(m_type) {
        case TYPE_NULL:
        case TYPE_BOOL:
        case TYPE_INT32:
        case TYPE_INT64:
        case TYPE_DOUBLE: 
            m_i64 = rhs.m_i64;
            break;
        case TYPE_STRING:
            m_s = new std::vector<char>(rhs.m_s->begin(), rhs.m_s->end());
            break;
        case TYPE_ARRAY:
            m_a = new std::vector<Value>(rhs.m_a->begin(), rhs.m_a->end());
            break;
        case TYPE_OBJECT:
            m_o = new std::vector<Member>(rhs.m_o->begin(), rhs.m_o->end());
            break;
    }
}

void Value::moveHelper(Value&& rhs) {
    m_type = rhs.m_type;
    switch(m_type) {
        case TYPE_NULL:
        case TYPE_BOOL:
        case TYPE_INT32:
        case TYPE_INT64:
        case TYPE_DOUBLE: 
            m_i64 = rhs.m_i64;
            break;
        case TYPE_STRING:
            m_s = rhs.m_s;
            rhs.m_s = nullptr;
            break;
        case TYPE_ARRAY:
            m_a = rhs.m_a;
            rhs.m_a = nullptr;
            break;
        case TYPE_OBJECT:
            m_o = rhs.m_o;
            rhs.m_o = nullptr;
            break;
    }
    rhs.m_type = TYPE_NULL;
}

Value::Value(const Value& rhs):m_type(rhs.m_type) {//拷贝构造 
    copyHelper(rhs);
}

Value::Value(Value&& rhs) {//移动构造
    moveHelper(std::move(rhs));
}

Value& Value::operator=(const Value& rhs) {//拷贝=运算符重载
    this->~Value();
    copyHelper(rhs);
    return *this;
}

Value& Value::operator=(Value&& rhs) {//移动=运算符重载
    this->~Value();
    moveHelper(std::move(rhs));
    return *this;
}

Value::~Value() {
    switch (m_type) {
        case TYPE_STRING:
            delete m_s;
            break;
        case TYPE_ARRAY:
            delete m_a;
            break;
        case TYPE_OBJECT:
            delete m_o;
            break;
    }
    m_type = TYPE_NULL;
    m_i64 = 0;
}
Value& Value::operator[] (std::string key) const{
    assert(m_type == TYPE_OBJECT);

    auto it = findMember(key);
    if (it != m_o->end()) {
        return it->m_value;
    }

    assert(false);//exit
    static Value fake(TYPE_NULL);
    return fake;
}

Value& Value::operator[](size_t i) const{
    assert(m_type == TYPE_ARRAY);
    return (*m_a)[i];
}

size_t Value::getSize() const {
     if (m_type == TYPE_ARRAY) {
        return m_a->size();
     } else if (m_type == TYPE_OBJECT) {
        return m_o->size();
     }
     return 1;
}

std::vector<Member>::iterator Value::findMember(std::string key) const{
    assert(m_type == TYPE_OBJECT);
    return std::find_if(m_o->begin(), m_o->end(), [key](Member& m)->bool {
        return m.m_key.getString() == key;
    });
}

void Value::addMember(const Value& key, const Value& value) {
    assert(m_type == TYPE_OBJECT);
    assert(key.m_type == TYPE_STRING);
    assert(findMember(key.getString()) == m_o->end());
    m_o->emplace_back(std::move(key), std::move(value));
}



Member::Member(const Member& member):m_key(member.m_key), m_value(member.m_value) {}

Member::Member(Member&& member):m_key(std::move(member.m_key)), m_value(std::move(member.m_value)) {}

Member& Member::operator=(const Member& member) {
    m_key.~Value();
    m_key = member.m_key;
    m_value.~Value();
    m_value = member.m_value;
    return *this;
}

Member& Member::operator=(Member&& member) {
    m_key.~Value();
    m_key = std::move(member.m_key);
    m_value.~Value();
    m_value = std::move(member.m_value);
    return *this;
}

}