#include <cppjson/Writer.hpp>
#include <cppjson/FileWriteStream.cpp>
#include <iostream>
using namespace cppjson;

int main()
{
    FileWriteStream os(stdout);
    Writer<FileWriteStream> writer(os);

    writer.StartArray();
    writer.Bool(true);
    writer.String("abcdefg"); // null character is OK
    writer.Bool(true);
    writer.EndArray();
    std::cout << std::endl;
    return 0;
}