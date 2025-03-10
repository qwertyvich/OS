cmake_minimum_required(VERSION 3.10)
project(lab5-7)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lzmq")

find_package(Threads REQUIRED)
find_package(GTest REQUIRED)

add_compile_options(-Wno-error=deprecated-declarations -Wdeprecated-declarations)

add_executable(client 
    src/client.cpp 
    src/tree.cpp 
    src/manage_zmq.cpp 
    include/tree.hpp 
    include/manage_zmq.hpp
)
target_include_directories(client PRIVATE include)
target_link_libraries(client PRIVATE zmq)

add_executable(server 
    src/server.cpp 
    src/tree.cpp 
    src/manage_zmq.cpp 
    include/tree.hpp 
    include/manage_zmq.hpp
)
target_include_directories(server PRIVATE include)
target_link_libraries(server PRIVATE zmq)

add_executable(test_variant48
    tests/test.cpp
    src/tree.cpp
    src/manage_zmq.cpp
    include/tree.hpp
    include/manage_zmq.hpp
)
target_include_directories(test_variant48 PRIVATE include)
target_link_libraries(test_variant48 PRIVATE GTest::gtest GTest::gtest_main zmq)

enable_testing()
add_test(NAME Variant48Test COMMAND test_variant48)
