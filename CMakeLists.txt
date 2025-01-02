cmake_minimum_required(VERSION 3.10)
project(lr3)

set(CMAKE_CXX_STANDARD 17)
add_definitions(-D_XOPEN_SOURCE=700)
# родитель
add_executable(lr3
    main.cpp
    src/parent.cpp
    src/functions.cpp
)
target_include_directories(lr3 PRIVATE include)

# два дочерних
add_executable(child1
    src/child1.cpp
    src/functions.cpp
)
target_include_directories(child1 PRIVATE include)

add_executable(child2
    src/child2.cpp
    src/functions.cpp
)
target_include_directories(child2 PRIVATE include)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(start_test
    tests/test.cpp
    src/parent.cpp
    src/functions.cpp
)
target_include_directories(start_test PRIVATE include)
target_link_libraries(start_test gtest gtest_main)
add_dependencies(start_test lr3 child1 child2)
