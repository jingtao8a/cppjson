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



struct Member;
class Document;

class Value {
    friend class Document;
private:
    void copyHelper(const Value& rhs);
    void moveHelper(Value&& rhs);

public:
    explicit Value(ValueType type = TYPE_NULL);
    
    explicit Value(bool b) : m_type(TYPE_BOOL), m_b(b) {}

    explicit Value(int32_t i32) : m_type(TYPE_INT32), m_i32(i32) {}

    explicit Value(int64_t i64): m_type(TYPE_INT64), m_i64(i64) {}

    explicit Value(double d) : m_type(TYPE_DOUBLE), m_d(d) {}

    explicit Value(std::string s) : m_type(TYPE_STRING), m_s(new std::vector<char>(s.begin(), s.end())) {}

    explicit Value(const char* s) : m_type(TYPE_STRING), m_s(new std::vector<char>(s, s + strlen(s))) {}

    explicit Value(const char* s, size_t len) : Value(std::string(s, len)) {}

    Value(const Value& rhs);//拷贝构造

    Value(Value&& rhs);//移动构造

    Value& operator=(const Value& rhs);//拷贝=运算符重载

    Value& operator=(Value&& rhs);//移动=运算符重载

    ~Value();

    ValueType getType() const{ return m_type; }
    size_t getSize() const;

    bool isNull() const{ return m_type == TYPE_NULL; }
    bool isBool() const{ return m_type == TYPE_BOOL; }
    bool isInt32() const{ return m_type == TYPE_INT32; }
    bool isInt64() const{ return m_type == TYPE_INT64 || m_type == TYPE_INT32; }
    bool isDouble() const{ return m_type == TYPE_DOUBLE; }
    bool isString() const{ return m_type == TYPE_STRING; }
    bool isArray()  const{ return m_type == TYPE_ARRAY; }
    bool isObject() const{ return m_type == TYPE_OBJECT; }

    // getter && setter
    bool getBool() const { 
        assert(m_type == TYPE_BOOL);
        return m_b;
    }

    int32_t getInt32() const {
        assert(m_type == TYPE_INT32);
        return m_i32;
    }

    int64_t getInt64() const{
        assert(m_type == TYPE_INT64 || m_type == TYPE_INT32);
        return m_type == TYPE_INT64 ? m_i64 : m_i32;
    }

    double getDouble() const{
        assert(m_type == TYPE_DOUBLE);
        return m_d;
    }

    std::string getString() const{
        assert(m_type == TYPE_STRING);
        return std::string(&*m_s->begin(), m_s->size());
    }

    auto& getArray() const{
        assert(m_type == TYPE_ARRAY);
        return *m_a;
    }

    auto& getObject() const{
        assert(m_type == TYPE_OBJECT);
        return *m_o;
    }

    Value& setNull() {
        this->~Value();
        return *new (this) Value(TYPE_NULL);
    }

    Value& setBool(bool b) {
        this->~Value();
        return *new (this) Value(b);
    }

    Value& setInt32(int32_t i32) {
        this->~Value();
        return *new (this) Value(i32);
    }

    Value& setInt64(int64_t i64) {
        this->~Value();
        return *new (this) Value(i64);
    }

    Value& setDouble(double d) {
        this->~Value();
        return *new (this) Value(d);
    }

    Value& setString(std::string s) {
        this->~Value();
        return *new (this) Value(s);
    }

    Value& setArray() {
        this->~Value();
        return *new (this) Value(TYPE_ARRAY);
    }

    Value& setObject() {
        this->~Value();
        return *new (this) Value(TYPE_OBJECT);
    }

    Value& operator[] (std::string key) const;
    Value& operator[] (size_t i) const;

    std::vector<Member>::iterator findMember(std::string key) const;
    void addMember(const Value& key, const Value& value);
    
    template <typename T>
    void addValue(T&& value) {
        assert(m_type == TYPE_ARRAY);
        m_a->emplace_back(std::forward<T>(value));
    }

protected:
    ValueType m_type;
    union {
        bool     m_b;
        int32_t  m_i32;
        int64_t  m_i64;
        double   m_d;
        std::vector<char>*  m_s;
        std::vector<Value>*   m_a;
        std::vector<Member>*  m_o;
    };
};

struct Member {
    Member(const Value& key, const Value& value): m_key(key), m_value(value) {}
    Member(Value&& key, Value&& value): m_key(std::move(key)), m_value(std::move(value)) {}
    Member(std::string key, Value&& value): m_key(key), m_value(std::move(value)) {}

    Member(const Member& member);
    Member(Member&& member);

    Member& operator=(const Member& member);
    Member& operator=(Member&& member);

    Value m_key;
    Value m_value;
};


}


#endif