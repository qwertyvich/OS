cmake_minimum_required(VERSION 3.10)
project(lab1)

set(CMAKE_CXX_STANDARD 17)

# Основное приложение
add_executable(lab1 main.cpp src/parent.cpp src/functions.cpp)
target_include_directories(lab1 PRIVATE include)

# Процесс 1
add_executable(child1 src/child1.cpp src/functions.cpp)
target_include_directories(child1 PRIVATE include)

# Процесс 2
add_executable(child2 src/child2.cpp src/functions.cpp)
target_include_directories(child2 PRIVATE include)

# Подключаем FetchContent для Google Test
include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)

FetchContent_MakeAvailable(googletest)

# Включаем тестирование
enable_testing()

# Добавляем тесты
add_executable(my_tests tests/test.cpp src/parent.cpp src/functions.cpp)
target_link_libraries(my_tests gtest)  # Используем только gtest

# Добавляем include директорию для тестов
target_include_directories(my_tests PRIVATE include)

# Обеспечиваем, чтобы Google Test был загружен до сборки тестов
add_dependencies(lab1 child1 child2 my_tests)
