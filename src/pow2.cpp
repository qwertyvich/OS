#include <ctime>            
#include <iostream>         
#include <vector>         
#include "../include/pow2.hpp"  
using namespace std;

int main() {
    vector<size_t> k = {16, 16, 32, 64, 128, 256, 512};

    Allocator_pow_of_2 alloc(k);
    clock_t start, end;
    vector<void*> blocks(1000);

    start = clock();
    for (int i = 0; i < 1000; i++) {
        blocks[i] = alloc.allocate(i + 1); // округляеттся, берётся из списка
    }
    end = clock();
    cout << "Allocator 2^n:"<< endl;
    cout << "Общее время выделения: " << (double)(end - start)/(double)CLOCKS_PER_SEC << " sec. "<<endl;


    start = clock();
    for (int i = 0; i < 1000; i++) {
        alloc.deallocate(blocks[i]);
    }
    end = clock();
    cout << "Общее время освобождения: " << (double)(end - start)/(double)CLOCKS_PER_SEC << " sec. " << endl;


    //фактор использования
    size_t sumRequested = 0;
    size_t sumAllocated = 0;

    for (int i = 0; i < 1000; i++) {
        size_t req = i + 1;
        void* p = alloc.allocate(req);
        if (!p) {
            break;
        }
        blocks[i] = p;

        sumRequested += req;

        size_t aligned = Allocator_pow_of_2::align(req);  // размер к ближайшему допустимому блоку 2^N
        size_t overhead = sizeof(Allocator_pow_of_2::Header); // размер заголовка

        sumAllocated += aligned + overhead;
    }

    double factor = 0.0;
    if (sumAllocated > 0) {
        factor = double(sumRequested) / double(sumAllocated);
    }
    cout << "Фактор использования: " << factor << endl;

    // освобождаем 
    for (int i = 0; i < 1000; i++) {
        if (blocks[i]) {
            alloc.deallocate(blocks[i]);
        }
    }

    return 0;
}
