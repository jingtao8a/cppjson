#ifndef CPPJSON_FILEREADSTREAM_HPP
#define CPPJSON_FILEREADSTREAM_HPP

#include "Nocopyable.hpp"
#include <vector>
#include <cstdio>

namespace cppjson {
class FileReadStream : public Nocopyable{
public:
    typedef std::vector<char>::iterator Iterator;
private:
    std::vector<char> m_buffer;
    Iterator m_iterator;

public:
    explicit FileReadStream(FILE* input);
    bool hasNext();//判断是否有下一个字符
    char next();//返回当前字符,并且itretor++
    char peek();//返回当前字符
    const Iterator getIter();
    void assertNext(char ch);
};

}

#endif