#include "parent.hpp"
#include "functions.hpp"
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <semaphore.h> 

using namespace std;

void ParentProcess(const char* pathToChild1, const char* pathToChild2, istream& streamIn) {
    // Создание shared memory для child1 и child2
    int shm_fd1 = CreateSharedMemory(SHM_CHILD1);
    int shm_fd2 = CreateSharedMemory(SHM_CHILD2);

    // Создание семафоров для child1 и child2
    sem_t* sem1 = CreateSemaphore(SEM_CHILD1);
    sem_t* sem2 = CreateSemaphore(SEM_CHILD2);

    srand(time(nullptr));

    // Имена output файлов могут быть заданы заранее или запрашиваться у пользователя
    string file1 = "out1.txt";
    string file2 = "out2.txt";

    // Создаем дочерний процесс 1
    pid_t pid1 = fork();
    if (pid1 == -1) {
        perror("Ошибка создания первого дочернего процесса");
        exit(EXIT_FAILURE);
    } else if (pid1 == 0) {
        // Дочерний процесс 1
        execl(pathToChild1, pathToChild1, nullptr);
        perror("Ошибка выполнения дочернего процесса 1");
        exit(EXIT_FAILURE);
    }

    // Создаем дочерний процесс 2
    pid_t pid2 = fork();
    if (pid2 == -1) {
        perror("Ошибка создания второго дочернего процесса");
        // Завершаем первый дочерний процесс перед выходом
        kill(pid1, SIGTERM);
        exit(EXIT_FAILURE);
    } else if (pid2 == 0) {
        // Дочерний процесс 2
        execl(pathToChild2, pathToChild2, nullptr);
        perror("Ошибка выполнения дочернего процесса 2");
        exit(EXIT_FAILURE);
    }

    // Родительский процесс продолжает выполнение
    string input;
    while (getline(streamIn, input)) {
        int pipe_choice = rand() % 10;
        if (pipe_choice < 8) {
            // Отправляем в child1
            WriteToSharedMemory(shm_fd1, input);
            sem_post(sem1);
        } else {
            // Отправляем в child2
            WriteToSharedMemory(shm_fd2, input);
            sem_post(sem2);
        }
    }

    // После завершения ввода, отправляем пустые строки для завершения детей
    WriteToSharedMemory(shm_fd1, "");
    sem_post(sem1);
    WriteToSharedMemory(shm_fd2, "");
    sem_post(sem2);

    // Ожидаем завершения дочерних процессов
    waitpid(pid1, nullptr, 0);
    waitpid(pid2, nullptr, 0);

    // Закрываем и удаляем shared memory и семафоры
    close(shm_fd1);
    close(shm_fd2);
    shm_unlink(SHM_CHILD1.c_str());
    shm_unlink(SHM_CHILD2.c_str());

    sem_close(sem1);
    sem_close(sem2);
    sem_unlink(SEM_CHILD1.c_str());
    sem_unlink(SEM_CHILD2.c_str());
}
