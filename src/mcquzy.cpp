#include <ctime>        // для clock()
#include <iostream>
#include <vector>
#include "../include/mcquzy.hpp"  // ваш заголовок

using namespace std;

int main() {
    McCuzyAllocator allocator1(1024 * 1024); //аллокатор на 1мб
    clock_t start, end;
    vector<void*> blocks(1000);

    start = clock();
    for (int i = 0; i < 1000; i++) {
        
        blocks[i] = allocator1.allocate(i + 1); // блоки размером i+1
    }
    end = clock();
    cout << "McCuzyAllocator:"<< endl;
    cout << "Общее время выделения: " << (double)(end - start)/(double)CLOCKS_PER_SEC << " sec. "<<endl;

    start = clock();
    for (int i = 0; i < 1000; i++) {
        allocator1.deallocate(blocks[i]);
    }
    end = clock();
    cout << "Общее время освобождения: " << (double)(end - start)/(double)CLOCKS_PER_SEC << " sec. " << endl;

    size_t sumRequested = 0;   // всего байтов
    size_t sumAllocated = 0;   // реально занято 

    for (int i = 0; i < 1000; i++) {
        size_t req = i + 1; // запрос

        void* p = allocator1.allocate(req);
        if (!p) {
            break;
        }
        blocks[i] = p;
        sumRequested += req; // байтов выделил
        size_t real_size = allocator1.getRealBlockSize(p); // беру реальный размер аллокатора
        sumAllocated += real_size;
    }

    // вычисляю фактор использования
    double factor = 0.0;
    if (sumAllocated > 0) {
        factor = (double)sumRequested / (double)sumAllocated;
    }
    cout << "Фактор использования: " << factor << endl;

    // высвободил
    for (int i = 0; i < 1000; i++) {
        if (blocks[i]) {
            allocator1.deallocate(blocks[i]);
        }
    }

    return 0;
}
