#ifndef CPPJSON_NOCOPYABLE_HPP
#define CPPJSON_NOCOPYABLE_HPP


namespace cppjson {

class Nocopyable {
public:
    Nocopyable(const Nocopyable&) = delete;
    void operator=(const Nocopyable&) = delete;
protected:
    Nocopyable() = default;
    ~Nocopyable() = default;
};


}

#endif



