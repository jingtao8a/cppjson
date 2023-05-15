#ifndef CPPJSON_STRINGREADSTREAM_HPP
#define CPPJSON_STRINGREADSTREAM_HPP
#include "Nocopyable.hpp"
#include <string>

namespace cppjson {

class StringReadStream : public Nocopyable {
public:
    typedef std::string::iterator Iterator;
private:
    std::string m_json;
    Iterator m_iterator;

public:
    StringReadStream(std::string json) : m_json(json), m_iterator(m_json.begin()) {}
    bool hasNext();//判断是否有下一个字符
    char next();//返回当前字符,并且itretor++
    char peek();//返回当前字符
    const Iterator getIter();
    void assertNext(char ch);
};

}

#endif