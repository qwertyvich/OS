#include "functions.hpp"
#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstring>
#include <cerrno>
#include <cstdlib>

//удаляю гласные
std::string removeVowels(const std::string& input) {
    std::string result;
    result.reserve(input.size()); // выделяет память для хранения элементов размера input.size
    for (char c : input) {
        switch(c) {
            case 'a': case 'e': case 'i': case 'o': case 'u':
            case 'A': case 'E': case 'I': case 'O': case 'U':
                break; // пропускаю гласную
            default:
                result.push_back(c);
        }
    }
    return result;
}

// создаю или открываю shm (shared_memory)
int CreateShm(const char* name) {
    int fd = shm_open(name, O_CREAT | O_RDWR, 0666);  // открываю именнованный объект
                      // имя для памяти // флаги // права доступа (rw для всех)
    if (fd == -1) {
        std::cerr << "shm_open error for " << name << ": " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    //устанавливаем размер
    //MMAP_SIZE = 4096;
    if (ftruncate(fd, MMAP_SIZE) == -1) {  //ftruncate(fd, MMAP_SIZE): устанавливаем размер 
                                            // объекта fd(который открыли)
        std::cerr << "ftruncate error for " << name << ": " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    return fd;
}

//функция является обёрткой над системным вызовом mmap, которая 
//создаёт указатель на разделяемый файл 
char* MapSharedMemory(int size, int fd) {
    void* addr = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    // отображает объект в адресное простравнство текущего процесса
    // mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)
    // mmap(адрес отображения, кол-во байт, режим доступа, видимость изменений, открытый объект)
    if (addr == MAP_FAILED) {
        std::cerr << "mmap failed: " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    return static_cast<char*>(addr);
}

// семафоры — это примитив синхронизации, помогающий предотвратить проблемы,
// такие как (race conditions) и взаимные блокировки (deadlocks), 
// которые могут возникнуть при конкурентном доступе к ресурсам.
sem_t* CreateSemaphore(const char* name, int value) {
    //              sem_open(имя семафора, флаг, rw для всех, начальное значение семафора)
    sem_t* sem = sem_open(name, O_CREAT, 0666, value);
    
    if (sem == SEM_FAILED) {
        std::cerr << "sem_open error for " << name << ": " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    return sem;
}

// вызов ошибки
void ErrorChecking(int result, const char* msg) {
    if (result == -1) {
        perror(msg);
        exit(EXIT_FAILURE);
    }
}
