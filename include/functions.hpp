#pragma once
#include <string>
#include <semaphore.h>

// Имена для shared memory и семафоров
const std::string SHM_CHILD1 = "/shm_child1";
const std::string SHM_CHILD2 = "/shm_child2";
const std::string SEM_CHILD1 = "/sem_child1";
const std::string SEM_CHILD2 = "/sem_child2";

// Размер буфера для строк
const size_t BUFFER_SIZE = 1024;

// Функции для создания и открытия shared memory и семафоров
int CreateSharedMemory(const std::string& name);
int OpenSharedMemory(const std::string& name);
sem_t* CreateSemaphore(const std::string& name);
sem_t* OpenSemaphore(const std::string& name);

// Функции для записи и чтения строк в shared memory
void WriteToSharedMemory(int shm_fd, const std::string& data);
std::string ReadFromSharedMemory(int shm_fd);

// Функция для удаления гласных из строки
std::string removeVowels(const std::string& input);
