#include "Exception.hpp"
#include <cassert>

namespace cppjson {

static const char* tab[] = {
    #define GEN_STRERR(e, s) s,
        ERROR_MAP(GEN_STRERR)
    #undef GEN_STRERR
};

const char* ParseErrorStr(ParseError err) {
    assert(err >= 0 && err < sizeof(tab) / sizeof(tab[0]));
    return tab[err];
}

const char* Exception::what() {
    return ParseErrorStr(m_err);
}


}