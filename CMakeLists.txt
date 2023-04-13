cmake_minimum_required(VERSION 3.7)
project("cppjson")

enable_testing()#启动测试

add_definitions("-std=c++14 -g")#定义编译器参数

set(CMAKE_BUILD_NO_EXAMPLE 0)
set(CMAKE_BUILD_TESTS 1)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

include_directories(${PROJECT_SOURCE_DIR})
add_subdirectory(src)

if (NOT CMAKE_BUILD_NO_EXAMPLE)
    add_subdirectory(example)
endif()

if (CMAKE_BUILD_TESTS)
    add_subdirectory(test)
endif()
