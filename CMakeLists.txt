cmake_minimum_required(VERSION 3.10)
project(lab3)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Основное приложение
add_executable(lab3 main.cpp src/parent.cpp src/functions.cpp)
target_include_directories(lab3 PRIVATE include)

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

# Для Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

# Включаем тестирование
enable_testing()

# Добавляем тесты
add_executable(my_tests tests/tests.cpp src/functions.cpp)
target_link_libraries(my_tests gtest_main)  # Используем gtest_main вместо gtest

# Добавляем include директорию для тестов
target_include_directories(my_tests PRIVATE include)

# Добавляем тест для CTest
add_test(NAME my_tests COMMAND my_tests)

# Обеспечиваем, чтобы Google Test был загружен до сборки тестов
add_dependencies(lab3 child1 child2 my_tests)
