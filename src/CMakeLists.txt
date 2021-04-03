project(cjm-numerics-uint128)

cmake_minimum_required(VERSION 3.17)
project(Int128)

set(CMAKE_CXX_STANDARD 20)
set(GCC_COVERAGE_COMPILE_FLAGS "-Wall -Werror -Wextra -Wstrict-aliasing -Wno-unknown-pragmas -Wstrict-overflow")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )

include_directories(./include)
add_library(cjm-numerics-uint128 INTERFACE)
target_include_directories(cjm-numerics-uint128 INTERFACE ./include)
