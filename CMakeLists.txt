cmake_minimum_required(VERSION 3.10)
project(kursovaya)

set(CMAKE_CXX_STANDARD 17)

add_executable(mcquzy
    src/mcquzy.cpp
    include/mcquzy.hpp
)
add_executable(pow2
    src/pow2.cpp
    include/pow2.hpp
)

