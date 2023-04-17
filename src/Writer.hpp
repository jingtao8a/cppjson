#ifndef CPPJSON_WRITER_HPP
#define CPPJSON_WRITER_HPP

#include "Nocopyable.hpp"
#include "Reader.hpp"
#include <algorithm>

namespace cppjson {

unsigned itoa(int32_t val, char* buf);
unsigned itoa(int64_t val, char* buf);

template <class WriterStream>
class Writer : public Nocopyable {
public:
    explicit Writer(WriterStream& os) : m_os(os) {}

#define CALL(expr) do { if (!(expr)) return false; } while(false)
    
    bool fromValue(Value& value) {
        switch(value.getType()) {
            case TYPE_NULL:
                CALL(Null());
                break;
            case TYPE_BOOL:
                CALL(Bool(value.getBool()));
                break;
            case TYPE_INT32:
                CALL(Int32(value.getInt32()));
                break;
            case TYPE_INT64:
                CALL(Int64(value.getInt64()));
                break;
            case TYPE_DOUBLE:
                CALL(Double(value.getDouble()));
                break;
            case TYPE_STRING:
                CALL(String(value.getString()));
                break;
            case TYPE_ARRAY:
                CALL(StartArray());
                for (auto& val: value.getArray()) {
                    CALL(fromValue(val));
                }
                CALL(EndArray());
                break;
            case TYPE_OBJECT:
                CALL(StartObject());
                for (auto& mem : value.getObject()) {
                    CALL(Key(mem.m_key.getString()));
                    CALL(fromValue(mem.m_value));
                }
                CALL(EndObject());
                break;
            default:
                assert(false && "bad type");
        }
        return true;
    }

#undef CALL

    virtual bool Null() {
        prefix(TYPE_NULL);
        m_os.put("null");
        return true;
    }

    virtual bool Bool(bool b) {
        prefix(TYPE_BOOL);
        m_os.put(b ? "true":"false");
        return true;
    }

    virtual bool Int32(int32_t i32) {
        prefix(TYPE_INT32);
        char buf[11];
        unsigned cnt = itoa(i32, buf);
        m_os.put(std::string(buf, cnt));
        return true;
    }

    virtual bool Int64(int64_t i64) {
        prefix(TYPE_INT64);
        char buf[20];
        unsigned cnt = itoa(i64, buf);
        m_os.put(std::string(buf, cnt));
        return true;
    }

    virtual bool Double(double d) {
        prefix(TYPE_DOUBLE);
        char buf[32];
        if (std::isinf(d)) {
            strcpy(buf, "Infinity");
        } else if (std::isnan(d)) {
            strcpy(buf, "NaN");
        } else {
            int n = sprintf(buf, "%.17g", d);
            assert(n > 0 && n < 32);
            auto it = std::find_if_not(buf, buf + n, Reader::isDigit);
            if (it == buf + n) {
                strcat(buf, ".0");
            }
        }
        m_os.put(buf);
        return true;
    }

    virtual bool String(std::string s) {
        prefix(TYPE_STRING);
        m_os.put('\"');
        for (auto c : s) {
            auto u = static_cast<unsigned>(c);
            switch(u) {
                case '\"': m_os.put("\\\""); break;
                case '\b': m_os.put("\\b"); break;
                case '\f': m_os.put("\\f"); break;
                case '\n': m_os.put("\\n"); break;
                case '\r': m_os.put("\\r"); break;
                case '\t': m_os.put("\\t"); break;
                case '\\': m_os.put("\\\\"); break;
                default:
                    if (u < 0x20) {
                        char buf[7];
                        snprintf(buf, 7, "\\u%04X", u);
                        m_os.put(buf);
                    } else {
                        m_os.put(c);
                    }
                    break;
            }
        }
        m_os.put('\"');
        return true;
    }

    virtual bool StartArray() {
        prefix(TYPE_ARRAY);
        m_os.put('[');
        m_stack.emplace_back(TYPE_ARRAY);
        return true;
    }

    virtual bool EndArray() {
        assert(!m_stack.empty());
        assert(m_stack.back().m_type == TYPE_ARRAY);
        m_stack.pop_back();
        m_os.put(']');
        return true;
    }

    virtual bool Key(std::string s) {
        prefix(TYPE_STRING);
        m_os.put('\"');
        m_os.put(s);
        m_os.put('\"');
        return true;
    }

    virtual bool StartObject() {
        prefix(TYPE_OBJECT);
        m_os.put('{');
        m_stack.emplace_back(TYPE_OBJECT);
        return true;
    }

    virtual bool EndObject() {
        assert(!m_stack.empty());
        assert(m_stack.back().m_type == TYPE_OBJECT);
        m_stack.pop_back();
        m_os.put('}');
        return true;
    }

private:
    void prefix(ValueType type) {
        if (m_stack.empty()) {
            return;        
        }

        Node& top = m_stack.back();

        if (top.m_type == TYPE_ARRAY) {
            if (top.m_count > 0) {
                m_os.put(',');
            }
        } else if (top.m_type == TYPE_OBJECT) {
            if (top.m_count % 2 == 1) {
                m_os.put(':');
            } else {
                assert(type == TYPE_STRING && "mis quotation mark");
                if (top.m_count > 0) {
                    m_os.put(',');
                }
            }
        }
        top.m_count++;
    }

    struct Node {
        Node(ValueType type) : m_type(type), m_count(0) {}
        ValueType m_type;
        int m_count;
    };
    
protected:
    WriterStream& m_os;
private:
    std::vector<Node> m_stack;
};

}


#endif