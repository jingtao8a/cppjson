#ifndef CPPJSON_PRETTY_WRITER_HPP
#define CPPJSON_PRETTY_WRITER_HPP

#include "Writer.hpp"
#include <string>

namespace cppjson {

template <class WriterStream> 
class PrettyWriter : public Writer<WriterStream> {
public:
    explicit PrettyWriter(WriterStream& os, std::string indent = "    ") : 
                Writer<WriterStream>(os), m_indent(indent), m_indentDepth(0), m_expectObjectValue(false) {}
    
    bool Null() override {
        Writer<WriterStream>::Null();
        keepIndent();
        return true;
    }

    bool Bool(bool b) override {
        Writer<WriterStream>::Bool(b);
        keepIndent();
        return true;
    }

    bool Int32(int32_t i32) {
        Writer<WriterStream>::Int32(i32);
        keepIndent();
        return true;
    }

    bool Int64(int64_t i64) override {
        Writer<WriterStream>::Int64(i64);
        keepIndent();
        return true;
    }

    bool Double(double d) override {
        Writer<WriterStream>::Double(d);
        keepIndent();
        return true;
    }

    bool String(std::string s) override {
        Writer<WriterStream>::String(s);
        keepIndent();
        return true;
    }

    bool StartObject() override {
        Writer<WriterStream>::StartObject();
        incrIndent();
        return true;
    }

    bool Key(std::string s) override {
        m_expectObjectValue = true;
        Writer<WriterStream>::Key(s);
        return true;
    }

    bool EndObject() override {
        decrIndent();
        Writer<WriterStream>::EndObject();
        return true;
    }

    bool StartArray() override {
        Writer<WriterStream>::StartArray();
        incrIndent();
        return true;
    }

    bool EndArray() override {
        decrIndent();
        Writer<WriterStream>::EndArray();
        return true;
    }

private:
    void incrIndent() {
        m_indentDepth++;
        putNewline();
        putIndent();
        if (m_expectObjectValue)
            m_expectObjectValue = false;
    }
    void decrIndent() {
        assert(m_indentDepth > 0);
        m_indentDepth--;
        putNewline();
        putIndent();
        if (m_expectObjectValue)
            m_expectObjectValue = false;
    }
    void keepIndent() {
        if (m_indentDepth > 0) {
            putNewline();
            putIndent();
        }
        if (m_expectObjectValue)
            m_expectObjectValue = false;
    }
    void putIndent() {
        for (int i = 0; i < m_indentDepth; i++)
            Writer<WriterStream>::m_os.put(m_indent);
    }
    void putNewline() {
        Writer<WriterStream>::m_os.put('\n');
    }
private:
    std::string m_indent;
    int m_indentDepth;
    bool m_expectObjectValue;
};

}

#endif