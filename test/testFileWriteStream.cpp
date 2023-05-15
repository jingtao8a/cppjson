#include "cppjson/Document.hpp"
#include "cppjson/FileWriteStream.hpp"
#include "cppjson/FileReadStream.hpp"
#include "cppjson/PrettyWriter.hpp"
#include <unistd.h>
#include <stdio.h>

using namespace cppjson;

int main() {
    Document doc;
    FILE* fp = fopen("../.vscode/c_cpp_properties.json", "r");
    if (fp == NULL) {
        return 1;
    }
    FileReadStream readStream(fp);
    auto res = doc.parseStream(readStream);
    if (res != PARSE_OK) {
        return 1;
    }
    
    FileWriteStream writeStream(stdout);
    PrettyWriter<FileWriteStream> writer(writeStream);

    writer.fromValue(doc);
    return 0;
}