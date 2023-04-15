#include "../src/Document.hpp"
#include <iostream>

int main() {
    cppjson::Document doc;
    auto i = doc.parse("[1, 2, 3]");
    std::cout << i << std::endl;
    std::cout << doc.getType();
    auto& res = doc.getArray();
    std::cout << res[2].getInt32();
    return 0;
}