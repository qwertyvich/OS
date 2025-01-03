#include <gtest/gtest.h>
#include "functions.hpp"


// Тесты для функции E_calc (реализация 1)
TEST(Lib1_Ecalc, PositiveX) {
    // Проверим несколько значений
    // e(1) = (1 + 1/1)^1 = 2
    EXPECT_FLOAT_EQ(E_calc(1), 2.0f);

    // e(2) = (1 + 1/2)^2 = 2.25
    EXPECT_FLOAT_EQ(E_calc(2), 2.25f);

    // e(5) ~  (1 + 0.2)^5 = 2.48832
    // в пределах небольшой дельты
    EXPECT_NEAR(E_calc(5), 2.48832f, 0.0001f);
}

TEST(Lib1_Ecalc, ZeroOrNegativeX) {
    // Если x <= 0, по логике lib1 мы возвращаем -1
    EXPECT_FLOAT_EQ(E_calc(0), -1.0f);
    EXPECT_FLOAT_EQ(E_calc(-10), -1.0f);
}

// Тесты для функции translation (реализация 1 -> двоичная система)
TEST(Lib1_Translation, Zero) {
    // Перевод 0 в двоичную: "0"
    EXPECT_STREQ(translation(0), "0");
}

TEST(Lib1_Translation, PositiveNumber) {
    // 5 в двоичной = "101"
    EXPECT_STREQ(translation(5), "101");
    // 16 = "10000"
    EXPECT_STREQ(translation(16), "10000");
}

TEST(Lib1_Translation, NegativeNumber) {
    // В данной реализации для отрицательных чисел мы возвращаем "-1"
    EXPECT_STREQ(translation(-1), "-1");
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
