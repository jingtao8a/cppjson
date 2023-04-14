#include "../src/Document.hpp"
#include <iostream>

int main() {
    cppjson::Document doc;
    auto i = doc.parse("\"\"");
    std::cout << i << std::endl;
    std::cout << doc.getString();
    return 0;
}