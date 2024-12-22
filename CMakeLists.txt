cmake_minimum_required(VERSION 3.16)
project(GaussianElimination)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -O3")

include_directories(src/core)

add_executable(gem_tester
    src/main.cpp
    src/core/matrix.hpp
    src/core/system_of_equations.hpp
    src/core/permutations.cpp
)
