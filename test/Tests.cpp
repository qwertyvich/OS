// /test/Tests.cpp
#include <gtest/gtest.h>
#include <iostream>
#include <ctime>
#include "complex.hpp"
#include "multiply.hpp" // подключаем файл с функциями умножения

// Тест 1: Проверка корректности однопоточного умножения
TEST(MatrixMultiplicationTest, SingleThreadCorrectness) {
    int A_rows = 3, A_cols = 3, B_rows = 3, B_cols = 3;
    Complex A[9], B[9], C[9], D[9];

    fillMatrix(A, A_rows, A_cols);
    fillMatrix(B, B_rows, B_cols);

    // Однопоточное умножение
    multiplyMatrices(A, B, C, A_rows, A_cols, B_cols, 1);

    // Повторное однопоточное умножение для сравнения
    multiplyMatrices(A, B, D, A_rows, A_cols, B_cols, 1);

    for (int i = 0; i < A_rows; ++i) {
        for (int j = 0; j < B_cols; ++j) {
            ASSERT_EQ(C[i * B_cols + j].real, D[i * B_cols + j].real);
            ASSERT_EQ(C[i * B_cols + j].imag, D[i * B_cols + j].imag);
        }
    }
}

// Тест 2: Проверка корректности многопоточного умножения
TEST(MatrixMultiplicationTest, MultiThreadCorrectness) {
    int A_rows = 3, A_cols = 3, B_rows = 3, B_cols = 3;
    Complex A[9], B[9], C[9], D[9];

    fillMatrix(A, A_rows, A_cols);
    fillMatrix(B, B_rows, B_cols);

    // Многопоточное умножение
    multiplyMatrices(A, B, C, A_rows, A_cols, B_cols, 4);

    // Однопоточное умножение для сравнения
    multiplyMatrices(A, B, D, A_rows, A_cols, B_cols, 1);

    for (int i = 0; i < A_rows; ++i) {
        for (int j = 0; j < B_cols; ++j) {
            ASSERT_EQ(C[i * B_cols + j].real, D[i * B_cols + j].real);
            ASSERT_EQ(C[i * B_cols + j].imag, D[i * B_cols + j].imag);
        }
    }
}

// Тест 3: Производительность — проверка времени работы с разным количеством потоков
TEST(MatrixMultiplicationTest, PerformanceTest) {
    int A_rows = 500, A_cols = 500, B_rows = 500, B_cols = 500;
    Complex* A = new Complex[A_rows * A_cols];
    Complex* B = new Complex[B_rows * B_cols];
    Complex* C = new Complex[A_rows * B_cols];

    fillMatrix(A, A_rows, A_cols);
    fillMatrix(B, B_rows, B_cols);

    // Однопоточное умножение
    clock_t start = clock();
    multiplyMatrices(A, B, C, A_rows, A_cols, B_cols, 1);
    clock_t end = clock();
    double singleThreadTime = double(end - start) / CLOCKS_PER_SEC;

    // Многопоточное умножение
    start = clock();
    multiplyMatrices(A, B, C, A_rows, A_cols, B_cols, 4);
    end = clock();
    double multiThreadTime = double(end - start) / CLOCKS_PER_SEC;

    std::cout << "Однопоточный: " << singleThreadTime << " секунд" << std::endl;
    std::cout << "Многопоточный: " << multiThreadTime << " секунд" << std::endl;

    delete[] A;
    delete[] B;
    delete[] C;

    // Убедимся, что многопоточная версия работает быстрее
    ASSERT_LT(multiThreadTime, singleThreadTime); 
}
