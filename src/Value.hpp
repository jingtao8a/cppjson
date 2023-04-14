#ifndef CPPJSON_VALUE_HPP
#define CPPJSON_VALUE_HPP

#include <atomic>
#include <cassert>
#include <vector>
#include <stdint.h>
#include <string>
#include <cstring>

namespace cppjson {

enum ValueType {
    TYPE_NULL,
    TYPE_BOOL,
    TYPE_INT32,
    TYPE_INT64,
    TYPE_DOUBLE,
    TYPE_STRING,
    TYPE_ARRAY,
    TYPE_OBJECT,
};

template <class T>
struct AddRefCount {
    template <typename ...Args>
    AddRefCount(Args&& ...args):data(std::forward<Args>(args)...) {}

    ~AddRefCount() {
        assert(m_count == 0);
    }

    int incrAndGet() {
        assert(m_count > 0);
        return ++m_count;
    }

    int decrAndGet() {
        assert(m_count > 0);
        return --m_count;
    }

    std::atomic_int m_count;
    T data;
};

struct Member;
class Document;

class Value {
    friend class Document;
protected:
    ValueType m_type;
    typedef AddRefCount<std::vector<char> > StringWithCount;
    typedef AddRefCount<std::vector<Value> > ArrayWithCount;
    typedef AddRefCount<std::vector<Member> > ObjectWithCount;

    union {
        bool     m_b;
        int32_t  m_i32;
        int64_t  m_i64;
        double   m_d;
        StringWithCount*  m_s;
        ArrayWithCount*   m_a;
        ObjectWithCount*  m_o;
    };

public:
    explicit Value(ValueType type = TYPE_NULL) : m_type(type) {}
    
    explicit Value(bool b) : m_type(TYPE_BOOL), m_b(b) {}

    explicit Value(int32_t i32) : m_type(TYPE_INT32), m_i32(i32) {}

    explicit Value(int64_t i64): m_type(TYPE_INT64), m_i64(i64) {}

    explicit Value(double d) : m_type(TYPE_DOUBLE), m_d(d) {}

    explicit Value(std::string s) : m_type(TYPE_STRING), m_s(new StringWithCount(s.begin(), s.end())) {}

    explicit Value(const char* s) : m_type(TYPE_STRING), m_s(new StringWithCount(s, s + strlen(s))) {}

    Value(const char* s, size_t len) : Value(std::string(s, len)) {}

    // Value(const Value& rhs);//拷贝构造

    // Value(Value&& rhs);//移动构造

    // Value& operator=(const Value& rhs);//拷贝=运算符重载

    // Value& operator=(Value&& rhs);//移动=运算符重载

    // ~Value();

    ValueType getType() { return m_type; }
    // size_t getSize();

    bool isNull() { return m_type == TYPE_NULL; }
    bool isBool() { return m_type == TYPE_BOOL; }
    bool isInt32() { return m_type == TYPE_INT32; }
    bool isInt64() { return m_type == TYPE_INT64 || m_type == TYPE_INT32; }
    bool isDouble() { return m_type == TYPE_DOUBLE; }
    bool isString() { return m_type == TYPE_STRING; }
    bool isArray()  { return m_type == TYPE_ARRAY; }
    bool isObject() { return m_type == TYPE_OBJECT; }

    // getter && setter
    bool getBool() {
        assert(m_type == TYPE_BOOL);
        return m_b;
    }

    int32_t getInt32() {
        assert(m_type == TYPE_INT32);
        return m_i32;
    }

    int64_t getInt64() {
        assert(m_type == TYPE_INT64 || m_type == TYPE_INT32);
        return m_type == TYPE_INT64 ? m_i64 : m_i32;
    }

    double getDouble() {
        assert(m_type == TYPE_DOUBLE);
        return m_d;
    }

    std::string getString() {
        assert(m_type == TYPE_STRING);
        return std::string(&*m_s->data.begin(), m_s->data.size());
    }

    const auto& getArray() const
    {
        assert(m_type == TYPE_ARRAY);
        return m_a->data;
    }

    const auto& getObject() const
    {
        assert(m_type == TYPE_OBJECT);
        return m_o->data;
    }
};

struct Member {
    Member(Value&& key, Value&& value): m_key(std::move(key)), m_value(std::move(value)) {}
    Member(std::string key, Value&& value): m_key(key), m_value(std::move(value)) {}
    Value m_key;
    Value m_value;
};


}


#endif