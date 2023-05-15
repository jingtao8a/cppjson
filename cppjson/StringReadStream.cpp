#include "StringReadStream.hpp"
#include <cassert>

namespace cppjson {

bool StringReadStream::hasNext() {//判断是否有下一个字符
    return m_iterator != m_json.end();
}

char StringReadStream::next() {//返回当前字符,并且itretor++
    if (hasNext()) {
        char ch = *m_iterator;
        m_iterator++;
        return ch;
    }
    return '\0';
}

char StringReadStream::peek() {//返回当前字符
    return hasNext() ? *m_iterator : '\0';
}

const StringReadStream::Iterator StringReadStream::getIter() {
    return m_iterator;
}

void StringReadStream::assertNext(char ch) {
    assert(next() == ch);
}

}