cmake_minimum_required(VERSION 3.17)
project(Int128)

set(CMAKE_CXX_STANDARD 20)

include_directories(cpp)
include_directories(cpp/src)

add_executable(Int128
        cpp/src/cpp/program.cpp
        cpp/src/headers/info.txt)
