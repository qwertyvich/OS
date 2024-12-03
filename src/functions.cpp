#include "functions.hpp"
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <iostream>

// Создание shared memory
int CreateSharedMemory(const std::string& name) {
    int shm_fd = shm_open(name.c_str(), O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("Ошибка создания shared memory");
        exit(EXIT_FAILURE);
    }
    // Устанавливаем размер shared memory
    if (ftruncate(shm_fd, BUFFER_SIZE) == -1) {
        perror("Ошибка установки размера shared memory");
        exit(EXIT_FAILURE);
    }
    return shm_fd;
}

// Открытие существующего shared memory
int OpenSharedMemory(const std::string& name) {
    int shm_fd = shm_open(name.c_str(), O_RDWR, 0666);
    if (shm_fd == -1) {
        perror(("Ошибка открытия shared memory: " + name).c_str());
        exit(EXIT_FAILURE);
    }
    return shm_fd;
}

// Создание семафора
sem_t* CreateSemaphore(const std::string& name) {
    sem_t* sem = sem_open(name.c_str(), O_CREAT, 0666, 0);
    if (sem == SEM_FAILED) {
        perror(("Ошибка создания семафора: " + name).c_str());
        exit(EXIT_FAILURE);
    }
    return sem;
}

// Открытие существующего семафора
sem_t* OpenSemaphore(const std::string& name) {
    sem_t* sem = sem_open(name.c_str(), 0);
    if (sem == SEM_FAILED) {
        perror(("Ошибка открытия семафора: " + name).c_str());
        exit(EXIT_FAILURE);
    }
    return sem;
}

// Запись строки в shared memory
void WriteToSharedMemory(int shm_fd, const std::string& data) {
    if (data.size() >= BUFFER_SIZE) {
        std::cerr << "Ошибка: строка слишком длинная для буфера." << std::endl;
        return;
    }
    void* ptr = mmap(0, BUFFER_SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        perror("Ошибка отображения памяти для записи");
        exit(EXIT_FAILURE);
    }
    memset(ptr, 0, BUFFER_SIZE);
    memcpy(ptr, data.c_str(), data.size());
    if (munmap(ptr, BUFFER_SIZE) == -1) {
        perror("Ошибка размаппинга памяти после записи");
        exit(EXIT_FAILURE);
    }
}

// Чтение строки из shared memory
std::string ReadFromSharedMemory(int shm_fd) {
    void* ptr = mmap(0, BUFFER_SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        perror("Ошибка отображения памяти для чтения");
        exit(EXIT_FAILURE);
    }
    std::string data(static_cast<char*>(ptr));
    if (munmap(ptr, BUFFER_SIZE) == -1) {
        perror("Ошибка размаппинга памяти после чтения");
        exit(EXIT_FAILURE);
    }
    return data;
}

// Функция удаления гласных из строки
std::string removeVowels(const std::string& input) {
    std::string result;
    for (char ch : input) {
        if (ch != 'a' && ch != 'e' && ch != 'i' && ch != 'o' && ch != 'u' &&
            ch != 'A' && ch != 'E' && ch != 'I' && ch != 'O' && ch != 'U') {
            result += ch;
        }
    }
    return result;
}
