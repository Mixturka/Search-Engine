cmake_minimum_required(VERSION 3.12)

project(labwork11)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(src)

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(googletest)

set(test_sources
    tests/test.cpp
)
