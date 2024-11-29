#include <cstdlib>
#include <ctime>
#include <pthread.h>
#include "multiply.hpp"

void fillMatrix(Complex* matrix, int rows, int cols) {
    srand(time(0));
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            matrix[i * cols + j] = Complex(rand() % 10, rand() % 10);
        }
    }
}

void printMatrix(const Complex* matrix, int rows, int cols) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            std::cout << "(" << matrix[i * cols + j].real << ", " << matrix[i * cols + j].imag << ") ";
        }
        std::cout << std::endl;
    }
}

struct ThreadData {
    int startRow;
    int endRow;
    const Complex* A;
    const Complex* B;
    Complex* C;
    int A_cols;
    int B_cols;
};

void* multiplyRows(void* arg) {
    ThreadData* data = static_cast<ThreadData*>(arg);

    for (int i = data->startRow; i < data->endRow; ++i) {
        for (int j = 0; j < data->B_cols; ++j) {
            Complex sum;
            for (int k = 0; k < data->A_cols; ++k) {
                sum = sum + data->A[i * data->A_cols + k] * data->B[k * data->B_cols + j];
            }
            data->C[i * data->B_cols + j] = sum;
        }
    }

    return nullptr;
}

void multiplyMatrices(const Complex* A, const Complex* B, Complex* C, int A_rows, int A_cols, int B_cols, int numThreads) {
    int rowsPerThread = A_rows / numThreads;
    pthread_t* threads = new pthread_t[numThreads];
    ThreadData* threadData = new ThreadData[numThreads];

    for (int i = 0; i < numThreads; ++i) {
        threadData[i].A = A;
        threadData[i].B = B;
        threadData[i].C = C;
        threadData[i].A_cols = A_cols;
        threadData[i].B_cols = B_cols;
        threadData[i].startRow = i * rowsPerThread;
        threadData[i].endRow = (i + 1) * rowsPerThread;
        if (i == numThreads - 1) {
            threadData[i].endRow = A_rows; 
        }
        pthread_create(&threads[i], nullptr, multiplyRows, &threadData[i]);
    }

    for (int i = 0; i < numThreads; ++i) {
        pthread_join(threads[i], nullptr);
    }

    delete[] threads;
    delete[] threadData;
}