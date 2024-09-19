cmake_minimum_required(VERSION 3.12)

project(
    labwork8
    VERSION 1.0
    DESCRIPTION "C++ Year 2024 labwork8"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)


add_subdirectory(lib)


enable_testing()
add_subdirectory(tests)
