// /include/multiply.hpp

#pragma once
#include "complex.hpp"

// Объявление функций (без реализации)
void fillMatrix(Complex* matrix, int rows, int cols);
void printMatrix(const Complex* matrix, int rows, int cols);
void* multiplyRows(void* arg);
void multiplyMatrices(const Complex* A, const Complex* B, Complex* C, int A_rows, int A_cols, int B_cols, int numThreads);
