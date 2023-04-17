#ifndef STRING_WRITESTREAM_HPP
#define STRING_WRITESTREAM_HPP

#include "Nocopyable.hpp"
#include <string>
#include <vector>

namespace cppjson {

class StringWriteStream : public Nocopyable {
private:
    std::vector<char> m_buffer;

public:
    void put(char ch);
    void put(std::string s);
    std::string get();
};

}

#endif