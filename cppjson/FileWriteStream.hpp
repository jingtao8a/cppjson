#ifndef CPPJSON_FILEWRITESTREAM_HPP
#define CPPJSON_FILEWRITESTREAM_HPP

#include "Nocopyable.hpp"
#include <cstdio>
#include <string>

namespace cppjson {

class FileWriteStream : public Nocopyable {
private:
    FILE *m_output;
public:
    explicit FileWriteStream(FILE *output):m_output(output) {}

    void put(char c);//放入一个字符

    void put(const char* str);//放入一个字符串

    void put(std::string str);//放入一个string对象
};

}
#endif