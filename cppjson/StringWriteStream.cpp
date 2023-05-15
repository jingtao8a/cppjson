#include "StringWriteStream.hpp"

namespace cppjson {
void StringWriteStream::put(char ch) {
    m_buffer.push_back(ch);
}

void StringWriteStream::put(std::string s) {
    m_buffer.insert(m_buffer.end(), s.begin(), s.end());
}

std::string StringWriteStream::get() {
    return std::string(&*m_buffer.begin(), m_buffer.size());
}

}