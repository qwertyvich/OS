#include "multiply.hpp"

int main(int argc, char** argv) {
    int numThreads = 2;  
    if (argc > 1) {
        numThreads = atoi(argv[1]);
    }

    int A_rows = 4, A_cols = 4, B_rows = 4, B_cols = 4;
    Complex* A = new Complex[A_rows * A_cols];
    Complex* B = new Complex[B_rows * B_cols];
    Complex* C = new Complex[A_rows * B_cols];

    fillMatrix(A, A_rows, A_cols);
    fillMatrix(B, B_rows, B_cols);

    std::cout << "Матрица A:" << std::endl;
    printMatrix(A, A_rows, A_cols);

    std::cout << "Матрица B:" << std::endl;
    printMatrix(B, B_rows, B_cols);

    clock_t start = clock();
    multiplyMatrices(A, B, C, A_rows, A_cols, B_cols, numThreads);
    clock_t end = clock();
    std::cout << "Матрица C (результат перемножения):" << std::endl;
    printMatrix(C, A_rows, B_cols);
    std::cout << "Время выполнения: " << double(end - start) / CLOCKS_PER_SEC << " секунд" << std::endl;

    delete[] A;
    delete[] B;
    delete[] C;

    return 0;
}