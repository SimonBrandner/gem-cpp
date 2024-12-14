cmake_minimum_required(VERSION 3.16)
project(GaussianElimination)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -O3")

include_directories(src/gem_lib)

add_executable(gem_tester
    src/gem_tester/main.cpp
    src/gem_lib/matrix.hpp
    src/gem_lib/system_of_equations.hpp
    src/gem_lib/permutations.cpp
)
