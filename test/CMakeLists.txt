set(GOOGLETEST_ROOT third_party/googletest/googletest CACHE STRING "Google Test source root")

# SYSTEM表示当前头文件搜索路径只在当前文件有效
include_directories(SYSTEM 
        ${PROJECT_SOURCE_DIR}/${GOOGLETEST_ROOT}
        ${PROJECT_SOURCE_DIR}/${GOOGLETEST_ROOT}/include)

set(GOOGLETEST_SOURCES
        ${PROJECT_SOURCE_DIR}/${GOOGLETEST_ROOT}/src/gtest-all.cc
        ${PROJECT_SOURCE_DIR}/${GOOGLETEST_ROOT}/src/gtest_main.cc
        )

foreach(_source ${GOOGLETEST_SOURCES})
    set_source_files_properties(${_source} PROPERTIES GENERATED 1)
endforeach()

add_library(gtest STATIC ${GOOGLETEST_SOURCES})
target_link_libraries(gtest pthread)

add_executable(test_error test_error.cpp)
target_link_libraries(test_error gtest cppjson)

add_executable(test_value test_value.cpp)
target_link_libraries(test_value gtest cppjson)

add_executable(test_roundrip test_roundrip.cpp)
target_link_libraries(test_roundrip gtest cppjson)


set(TEST_DIR ${EXECUTABLE_OUTPUT_PATH})
add_test(test_error ${TEST_DIR}/test_error)
add_test(test_value ${TEST_DIR}/test_value)
add_test(test_roundrip ${TEST_DIR}/test_roundrip)