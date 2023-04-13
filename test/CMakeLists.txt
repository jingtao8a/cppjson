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

add_library(gtest ${GOOGLETEST_SOURCES})
target_link_libraries(gtest pthread)

add_executable(test_error test_error.cpp)
target_link_libraries(test_error gtest)

set(TEST_DIR ${EXECUTABLE_OUTPUT_PATH})
add_test(test_error ${TEST_DIR}/test_error)