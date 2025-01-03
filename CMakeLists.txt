cmake_minimum_required(VERSION 3.10)
project(lab4_variant_6_8)
set(CMAKE_CXX_STANDARD 17)

# Указываем, куда будут складываться скомпилированные библиотеки (.so/.dll/.dylib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/libs)#путь до скомпилированных библиотек
find_package(GTest REQUIRED)
#сбор библиотек
# --- Библиотека 1 (Realization 1) ---
add_library(lib1 SHARED src/lib1.cpp)
set_target_properties(lib1 PROPERTIES
    OUTPUT_NAME "lib1"
    PREFIX ""
)
target_include_directories(lib1 PRIVATE include)

# --- Библиотека 2 (Realization 2) ---
add_library(lib2 SHARED src/lib2.cpp)
set_target_properties(lib2 PROPERTIES
    OUTPUT_NAME "lib2"
    PREFIX ""
)
target_include_directories(lib2 PRIVATE include)

# ------------------------------------------------------------------------------
# Шаг 3: Компилируем программы
# ------------------------------------------------------------------------------
# Программа 1 (статическая линковка c lib1)
add_executable(program1 program1.cpp)
target_include_directories(program1 PRIVATE include)
target_link_libraries(program1 PRIVATE lib1 m)

# Программа 2 (динамическая загрузка lib1 / lib2)
add_executable(program2 program2.cpp)
target_include_directories(program2 PRIVATE include)
target_link_libraries(program2 PRIVATE dl m)

# ------------------------------------------------------------------------------
# Шаг 4: Тесты
# ------------------------------------------------------------------------------
# Тесты для lib1 (статическая линковка)
add_executable(test_lib1
    tests/test_lib1.cpp
)
target_include_directories(test_lib1 PRIVATE include)
target_link_libraries(test_lib1
    PRIVATE
        lib1                # статическая линковка библиотеки lib1
        GTest::GTest
        GTest::Main
        m
)

# Тесты для lib2 (статическая линковка)
add_executable(test_lib2
    tests/test_lib2.cpp
)
target_include_directories(test_lib2 PRIVATE include)
target_link_libraries(test_lib2
    PRIVATE
        lib2                # статическая линковка библиотеки lib2
        GTest::GTest
        GTest::Main
        m
)

# ------------------------------------------------------------------------------
# Шаг 5: Регистрируем тесты (ctest)
# ------------------------------------------------------------------------------
include(GoogleTest)
gtest_discover_tests(test_lib1)
gtest_discover_tests(test_lib2)
