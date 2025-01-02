#pragma once
#include <string>
#include <semaphore.h>

// Удаление гласных
std::string removeVowels(const std::string& input);

// Создание или открытие разделяемой памяти
int CreateShm(const char* name);

// Отображаемый файл (memory-mapped)
char* MapSharedMemory(int size, int fd);

// Создание/открытие именованного семафора
sem_t* CreateSemaphore(const char* name, int value);

// Бросаем ошибку при -1
void ErrorChecking(int result, const char* msg);

// --- Константы ---
constexpr int MMAP_SIZE = 4096; // размер области памяти (4 КБ)

// Имена для памяти (по одному на каждого ребёнка)
constexpr const char* SHM_NAME1 = "/myshm1";
constexpr const char* SHM_NAME2 = "/myshm2";

// Имена для семафоров (по 2 на каждого ребёнка: родитель->ребёнок, ребёнок->родитель)
constexpr const char* SEM_PARENT1 = "/sem_parent1";
constexpr const char* SEM_CHILD1  = "/sem_child1";

constexpr const char* SEM_PARENT2 = "/sem_parent2";
constexpr const char* SEM_CHILD2  = "/sem_child2";
