#include <cppjson/Document.hpp>
#include <iostream>
#include <cppjson/Exception.hpp>
using namespace cppjson;

int main()
{
    Document document;
    ParseError err = document.parse("{"
    "    \"precision\": \"zip\","
    "    \"Latitude\": 37.766800000000003,"
    "    \"Longitude\": -122.3959,"
    "    \"Address\": \"\","
    "    \"City\": \"SAN FRANCISCO\","
    "    \"State\": \"CA\","
    "    \"Zip\": \"94107\","
    "    \"Country\": \"US\""
    "    }");

    if (err != PARSE_OK) {
        puts(ParseErrorStr(err));
        exit(1);
    }

    // get 'State' field
    Value& state = document["State"];
    std::cout << "State: " << state.getString() << '\n';

    // get 'Zip' field
    Value& zip = document["Zip"];
    std::cout << "Zip: " << zip.getString() << "\n";

    // set 'Zip' field
    zip.setInt32(9527);
    std::cout << "Zip: " << zip.getInt32() << "\n";
}