add_library(cppjson STATIC 
    Document.cpp
    Exception.cpp
    FileReadStream.cpp
    FileWriteStream.cpp
    StringReadStream.cpp
    StringWriteStream.cpp
    Value.cpp
    Reader.cpp
    Writer.cpp
    )

install(TARGETS cppjson DESTINATION lib)

set(HEADERS
    Document.hpp
    Exception.hpp
    FileReadStream.hpp
    FileWriteStream.hpp
    Nocopyable.hpp
    PrettyWriter.hpp
    Reader.hpp
    StringReadStream.hpp
    StringWriteStream.hpp
    Value.hpp
    Writer.hpp)

install(FILES ${HEADERS} DESTINATION include/cppjson)
