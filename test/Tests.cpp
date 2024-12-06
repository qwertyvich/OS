#include <gtest/gtest.h>
#include <iostream>
#include <ctime>
#include "complex.hpp"
#include "multiply.hpp"

//Показывает корректность однопоточного варианта
TEST(MatrixMultiplicationTest, SingleThreadCorrectness) { 
    int A_rows = 3, A_cols = 3, B_rows = 3, B_cols = 3;
    Complex A[9], B[9], C[9], D[9];                     //объявил комлексные arrs чисekл
    fillMatrix(A, A_rows, A_cols);                      // заполенил матрицу
    fillMatrix(B, B_rows, B_cols);
    multiplyMatrices(A, B, C, A_rows, A_cols, B_cols, 1);       // перемножил с выводом в матрицы С и D
    multiplyMatrices(A, B, D, A_rows, A_cols, B_cols, 1);
    for (int i = 0; i < A_rows; ++i) {                          // чекаю что С==Д
        for (int j = 0; j < B_cols; ++j) {
            ASSERT_EQ(C[i * B_cols + j].real, D[i * B_cols + j].real);
            ASSERT_EQ(C[i * B_cols + j].imag, D[i * B_cols + j].imag);
        }
    }
}

//Показывает, что однопоточный вариант и вариант с 
//разным количеством потоков дают одни и те же результаты
TEST(MatrixMultiplicationTest, MultiThreadCorrectness) {
    int A_rows = 3, A_cols = 3, B_rows = 3, B_cols = 3;
    Complex A[9], B[9], C[9], D[9];
    fillMatrix(A, A_rows, A_cols);
    fillMatrix(B, B_rows, B_cols);
    multiplyMatrices(A, B, C, A_rows, A_cols, B_cols, 4);           // поделил на 4 потока
    multiplyMatrices(A, B, D, A_rows, A_cols, B_cols, 1);           // оставил 1
    for (int i = 0; i < A_rows; ++i) {
        for (int j = 0; j < B_cols; ++j) {
            ASSERT_EQ(C[i * B_cols + j].real, D[i * B_cols + j].real);
            ASSERT_EQ(C[i * B_cols + j].imag, D[i * B_cols + j].imag);
        }
    }
}

//Тест, который показывает, что многопоточный вариант быстрее однопоточного
TEST(MatrixMultiplicationTest, PerformanceTest) {
    int A_rows = 500, A_cols = 500, B_rows = 500, B_cols = 500;
    Complex* A = new Complex[A_rows * A_cols];
    Complex* B = new Complex[B_rows * B_cols];
    Complex* C = new Complex[A_rows * B_cols];
    fillMatrix(A, A_rows, A_cols);
    fillMatrix(B, B_rows, B_cols);
    clock_t start = clock();
    multiplyMatrices(A, B, C, A_rows, A_cols, B_cols, 1);       // для 1
    clock_t end = clock();
    double singleThreadTime = double(end - start) / CLOCKS_PER_SEC; // CLOCKS_PER_SEC константа  возвращает количество тактов процессора
    start = clock();
    multiplyMatrices(A, B, C, A_rows, A_cols, B_cols, 4);       // для 4
    end = clock();
    double multiThreadTime = double(end - start) / CLOCKS_PER_SEC;
    std::cout << "Однопоточный: " << singleThreadTime << " секунд" << std::endl;
    std::cout << "Многопоточный: " << multiThreadTime << " секунд" << std::endl;
    delete[] A;
    delete[] B;
    delete[] C;

    ASSERT_LT(multiThreadTime, singleThreadTime); 
}
