#include <iostream>
#include <string>
#include "../src/Document.hpp"

int main() {
    cppjson::Document doc;
    doc.parse("\"\\u0000\"");
    auto res = doc.getString();
    std::cout << res.size();
    return 0;
}