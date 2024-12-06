#pragma once
#include "complex.hpp"
//заполняет матрицу случайными комплексными числами
void fillMatrix(Complex* matrix, int rows, int cols); 
//выводит матрицу на экран
void printMatrix(const Complex* matrix, int rows, int cols);    
//выполняет умножение определённых строк матрицы
void* multiplyRows(void* arg); 
//управляет созданием потоков для умножения матриц
void multiplyMatrices(const Complex* A, const Complex* B, Complex* C, int A_rows, int A_cols, int B_cols, int numThreads); 
