#include "../src/Document.hpp"
#include "../src/Writer.hpp"
#include "../src/FileReadStream.hpp"
#include "../src/StringWriteStream.hpp"
#include <stdio.h>
#include <iostream>

int main() {
    FILE *fp = fopen("../.vscode/c_cpp_properties.json", "r");
    if (fp == NULL) {
        return 1;
    }
    cppjson::Document doc;
    cppjson::FileReadStream is(fp);
    doc.parseStream(is);

    cppjson::StringWriteStream os;
    cppjson::Writer<cppjson::StringWriteStream> w(os);
    w.fromValue(doc);

    std::cout << os.get();
}