cmake_minimum_required(VERSION 3.5)
project(std_vector)

set(CMAKE_CXX_STANDARD  20)

find_package(Catch2 3)
if (NOT Catch2_FOUND)
    add_subdirectory(contrib/Catch2)
endif ()


add_executable(tests "${CMAKE_SOURCE_DIR}/src/test.cpp")
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
target_include_directories(tests PRIVATE "${CMAKE_SOURCE_DIR}/include")

include(CTest)
include(Catch)
catch_discover_tests(tests)