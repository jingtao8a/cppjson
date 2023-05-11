#include "FileWriteStream.hpp"

namespace cppjson {

void FileWriteStream::put(char c) {//放入一个字符
    putc(c, m_output);
}

void FileWriteStream::put(const char* str) {//放入一个字符串
    fputs(str, m_output);
}

void FileWriteStream::put(std::string str) {//放入一个string对象
    fprintf(m_output, "%.*s", static_cast<int>(str.length()), str.data());
}

}