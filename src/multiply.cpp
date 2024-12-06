#include <cstdlib>  // генерация случ. чисел
#include <ctime>    // генерация случ. чисел
#include <pthread.h>
#include "multiply.hpp"

void fillMatrix(Complex* matrix, int rows, int cols) {
    srand(time(0));
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            matrix[i * cols + j] = Complex(rand() % 10, rand() % 10); // matrix - одномерный массив
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
    int startRow;               // диапазон строк матрицы для обработки потоком 
    int endRow;
    const Complex* A;
    const Complex* B;
    Complex* C;                 // вывод в матрицу С
    int A_cols;                 // n столбцов в А
    int B_cols;                 // n столбцов в Б
};

void* multiplyRows(void* arg) {
    ThreadData* data = static_cast<ThreadData*>(arg);   //привожу к типу threaddata указателя на arg поскольку в аргументе дал void
    //static_cast<> — это оператор приведения типов
    for (int i = data->startRow; i < data->endRow; ++i) { //иду по строкам
        for (int j = 0; j < data->B_cols; ++j) {            // иду по столбцам
            Complex sum;
            for (int k = 0; k < data->A_cols; ++k) {  // суммирую произведения элементов А и Б
                sum = sum + data->A[i * data->A_cols + k] * data->B[k * data->B_cols + j];
            }
            data->C[i * data->B_cols + j] = sum; // запись в С
        }
    }

    return nullptr;
}

void multiplyMatrices(const Complex* A, const Complex* B, Complex* C, int A_rows, int A_cols, int B_cols, int numThreads) {
    int rowsPerThread = A_rows / numThreads;                    // кол-во строк матрицы А которые обрабатываюстя одним потоком  
    pthread_t* threads = new pthread_t[numThreads];             // пассив идентиф. потокв     
    ThreadData* threadData = new ThreadData[numThreads];          // массив структур struct ThreadData для каждого потока

    for (int i = 0; i < numThreads; ++i) {                        // задаю поля для кажого потока
        threadData[i].A = A;
        threadData[i].B = B;
        threadData[i].C = C;
        threadData[i].A_cols = A_cols;
        threadData[i].B_cols = B_cols;
        threadData[i].startRow = i * rowsPerThread;
        threadData[i].endRow = (i + 1) * rowsPerThread;
        if (i == numThreads - 1) {                              //учитываю неравномерное деление строк
            threadData[i].endRow = A_rows; 
        }
        pthread_create(&threads[i], nullptr, multiplyRows, &threadData[i]);
                                            //ук. на функцию; аргумент передаваемый в функцию привёднный к void
    }

    for (int i = 0; i < numThreads; ++i) {
        pthread_join(threads[i], nullptr);    // ожидаю завершение потоков
    }

    delete[] threads;
    delete[] threadData;
}