#include "FileReadStream.hpp"
#include <cassert>

namespace cppjson {

FileReadStream::FileReadStream(FILE* input) {
    char buf[65536];
    while (true) {
        size_t n  = fread(buf, 1, sizeof(buf), input);
        if (n == 0) {//表示读取完了文件
            break;
        }
        m_buffer.insert(m_buffer.end(), buf, buf + n);
    }
    m_iterator = m_buffer.begin();
}
bool FileReadStream::hasNext() {
    return m_iterator != m_buffer.end();
}

char FileReadStream::next() {
    if (hasNext()) {
        char ch = *m_iterator;
        m_iterator++;
        return ch;
    }
    return '\0';
}

char FileReadStream::peek() {
    return hasNext() ? *m_iterator : '\0';
}

const FileReadStream::Iterator FileReadStream::getIter() {
    return m_iterator;
}

void FileReadStream::assertNext(char ch) {
    assert(next() == ch);
}

}