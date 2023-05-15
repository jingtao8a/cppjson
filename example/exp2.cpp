#include <cppjson/Document.hpp>
#include <cppjson/PrettyWriter.hpp>
#include <cppjson/FileReadStream.hpp>
#include <cppjson/StringWriteStream.hpp>
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
    cppjson::PrettyWriter<cppjson::StringWriteStream> w(os);
    w.fromValue(doc);

    std::cout << os.get();
}