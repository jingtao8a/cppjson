#include "../src/Document.hpp"
#include <iostream>

int main() {
    cppjson::Document doc;
    auto i = doc.parse("123242");
    std::cout << i << std::endl;
    std::cout << doc.getInt32();
    return 0;
}