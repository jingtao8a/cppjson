#include <iostream>
#include <string>
int main() {
    const char * st = "hello";
    std::string str(st, 5);
    std::cout << str;
    return 0;
}