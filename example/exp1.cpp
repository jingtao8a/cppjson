#include "../src/Document.hpp"
#include "../src/Writer.hpp"
#include "../src/StringWriteStream.hpp"
#include <iostream>

class Test {
public:
    static void show() {
        YU i = 1;
        std::cout << i;
    }
    typedef int YU;
};

int main() {
    // cppjson::Document doc;
    // auto i = doc.parse("[1, 2, 3]");
    // std::cout << i << std::endl;
    // std::cout << doc.getType();
    // auto& res = doc.getArray();
    // std::cout << res[2].getInt32();
    // cppjson::StringWriteStream os;
    // cppjson::Writer<cppjson::StringWriteStream> w(os);
    // w.StartArray();
    // w.Bool(true);
    // w.Double(1.0);
    // w.String("yuxintao");
    // w.EndArray();
    // std::cout << os.get();
    Test::show();
    return 0;
}