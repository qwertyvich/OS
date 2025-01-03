#include <gtest/gtest.h>
#include "functions.hpp"


// Тесты для функции E_calc (реализация 2)
TEST(Lib2_Ecalc, PositiveX) {
    // e(0) = 1/0! = 1
    // но по коду, если x < 0, тогда -1
    // здесь x=0 => sum_{p=0..0}(1/p!)=1 => проверим
    EXPECT_FLOAT_EQ(E_calc(0), 1.0f);

    // e(1) = 1/0! + 1/1! = 1 + 1 = 2
    EXPECT_FLOAT_EQ(E_calc(1), 2.0f);

    // e(3) = 1/0! + 1/1! + 1/2! + 1/3! = 1 + 1 + 0.5 + 0.16666.. = 2.66666..
    EXPECT_NEAR(E_calc(3), 2.66666f, 0.0001f);

    // e(5) = ~ 2.71666..
    // (точное значение = 1 + 1 + 1/2 + 1/6 + 1/24 + 1/120 = 2.71666..)
    EXPECT_NEAR(E_calc(5), 2.71666f, 0.0001f);
}

TEST(Lib2_Ecalc, NegativeX) {
    // Если x < 0, по логике lib2 возвращаем -1
    EXPECT_FLOAT_EQ(E_calc(-5), -1.0f);
}

// Тесты для функции translation (реализация 2 -> троичная система)
TEST(Lib2_Translation, Zero) {
    // Перевод 0 в троичную: "0"
    EXPECT_STREQ(translation(0), "0");
}

TEST(Lib2_Translation, PositiveNumber) {
    // 5 в троичной = "12"
    // (5 / 3 = 1, ост.2) => 5 = 12(3)
    EXPECT_STREQ(translation(5), "12");

    // 16 в троичной = "121"
    // 16 / 3 = 5, ост.1; 5/3=1, ост.2; 1/3=0, ост.1 => "121"
    EXPECT_STREQ(translation(16), "121");
}

TEST(Lib2_Translation, NegativeNumber) {
    // По текущей реализации, для x < 0 => "-1"
    EXPECT_STREQ(translation(-10), "-1");
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
