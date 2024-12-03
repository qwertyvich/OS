// tests/tests.cpp
#include <gtest/gtest.h>
#include "functions.hpp"

// Тест 1: Базовый тест
TEST(RemoveVowelsTest, BasicTest) {
    std::string input = "Hello World";
    std::string expected = "Hll Wrld";
    EXPECT_EQ(removeVowels(input), expected);
}

// Тест 2: Пустая строка
TEST(RemoveVowelsTest, EmptyString) {
    std::string input = "";
    std::string expected = "";
    EXPECT_EQ(removeVowels(input), expected);
}

// Тест 3: Строка без гласных
TEST(RemoveVowelsTest, NoVowels) {
    std::string input = "bcdfg";
    std::string expected = "bcdfg";
    EXPECT_EQ(removeVowels(input), expected);
}

// Тест 4: Строка с разными регистрами гласных
TEST(RemoveVowelsTest, MixedCaseVowels) {
    std::string input = "AeIoU aeiou AEIOU";
    std::string expected = "  ";
    EXPECT_EQ(removeVowels(input), expected);
}

// Дополнительный тест: Строка с гласными в середине слов
TEST(RemoveVowelsTest, VowelsInMiddle) {
    std::string input = "Programming is fun";
    std::string expected = "Prgrmmng s fn";
    EXPECT_EQ(removeVowels(input), expected);
}
