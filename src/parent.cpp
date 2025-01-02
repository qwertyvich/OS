#include <cstring>      // memset, memcpy
#include <sys/mman.h>   // shm_unlink, mmap, munmap
#include <sys/stat.h>   // ftruncate, ...
#include <fcntl.h>      // shm_open, open, O_*...
#include <unistd.h>     // fork, close, dup2
#include <sys/wait.h>   // waitpid
#include <iostream>
#include <string>
#include <cstdlib>      // rand, srand, exit
#include <ctime>        // time
#include "parent.hpp"
#include "functions.hpp"

#define _XOPEN_SOURCE 700

void ParentProcess(const char* pathToChild1, const char* pathToChild2, std::istream& streamIn) {
                    // путь к исп. файлу child1    //путь к исп. файлу child2        //поток ввода

    std::string file1, file2;

    //читаю имя файла 1
    std::cout << "Введите имя файла для child1: ";
    if (!std::getline(streamIn, file1)) {
        std::cerr << "Ошибка чтения имени файла для child1\n";
        exit(EXIT_FAILURE);
    }

    //читаю имя файла 2
    std::cout << "Введите имя файла для child2: ";
    if (!std::getline(streamIn, file2)) {
        std::cerr << "Ошибка чтения имени файла для child2\n";
        exit(EXIT_FAILURE);
    }

    //открываю эти файлы на запись, куда в дальнешем будет направлен stdout дочерних процессов
    int fd1 = open(file1.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);  //открыть на запись, создать при отсутствии, 
                                                                        //очистить файл, если он уже есть.
    if (fd1 == -1) {
        perror("open file1");
        exit(EXIT_FAILURE);
    }
    int fd2 = open(file2.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);   //открыть на запись, создать при отсутствии, 
                                                                         //очистить файл, если он уже есть.
    if (fd2 == -1) {
        perror("open file2");
        close(fd1);
        exit(EXIT_FAILURE);
    }

    //SHM_NAME1, SHM_NAME2 имена для файлов прописанные в functions.hpp
    shm_unlink(SHM_NAME1); // предварительно очищаю объекты перед новым вызовом
    shm_unlink(SHM_NAME2); 
    int shmFd1 = CreateShm(SHM_NAME1);
    int shmFd2 = CreateShm(SHM_NAME2);

    // удаление семафора, тоже как бы очищаю память от предыдущего запуска
    sem_unlink(SEM_PARENT1);
    sem_unlink(SEM_CHILD1);
    sem_unlink(SEM_PARENT2);
    sem_unlink(SEM_CHILD2);

    sem_t* semParent1 = CreateSemaphore(SEM_PARENT1, 0);
    sem_t* semChild1  = CreateSemaphore(SEM_CHILD1, 0);
    sem_t* semParent2 = CreateSemaphore(SEM_PARENT2, 0);
    sem_t* semChild2  = CreateSemaphore(SEM_CHILD2, 0);

    // дочерний процесс #1
    pid_t pid1 = fork();
    ErrorChecking(pid1, "fork child1");
    if (pid1 == 0) {
        // перенаправляю stdout в fd1, т.е. работает дочерний процесс 1
        dup2(fd1, STDOUT_FILENO);
        close(fd1);
        close(fd2);
        //Выполняю execl, чтобы запустить внешний исполняемый файл child1
        execl(pathToChild1, pathToChild1, SEM_PARENT1, SEM_CHILD1, SHM_NAME1, nullptr);
        perror("exec child1");
        exit(EXIT_FAILURE);
    }

    // дочерний процесс #2
    pid_t pid2 = fork();
    ErrorChecking(pid2, "fork child2");
    if (pid2 == 0) {
        dup2(fd2, STDOUT_FILENO);
        close(fd2);
        close(fd1);
        execl(pathToChild2, pathToChild2, SEM_PARENT2, SEM_CHILD2, SHM_NAME2, nullptr);
        perror("exec child2");
        exit(EXIT_FAILURE);
    }

    // если мы тут, значит это всё ещё родитель.
    close(fd1);
    close(fd2);

    // генератор псевдослучайных чисел
    std::srand((unsigned)std::time(nullptr));

    // Цикл чтения строк из streamIn
    std::string line;
    while (true) {
        std::cout << "Введите строку (EOF для выхода): ";
        if (!std::getline(streamIn, line)) {
            //Если не можем прочитать строку -> EOF
            break;
        }
        if (line.empty()) {
            break;
        }
        //80% в child1, 20% в child2
        bool toChild1 = (std::rand() % 10 < 8);
        if (toChild1) {
            // отправяляю строку в child1
            char* addr = MapSharedMemory(MMAP_SIZE, shmFd1); // отобразить shm1 в память
            memset(addr, 0, MMAP_SIZE);                       // очистить
            memcpy(addr, line.c_str(), line.size());          // записать строку
            // даю сигнал child1, что есть данные
            sem_post(semParent1);                   // sem_post(sem_t *sem); увеличивает значение семафора
            // ожидаю что обратано
            sem_wait(semChild1);
            // высвободил память
            munmap(addr, MMAP_SIZE);
        } else {
            //то же самое в child2
            char* addr = MapSharedMemory(MMAP_SIZE, shmFd2);
            memset(addr, 0, MMAP_SIZE);
            memcpy(addr, line.c_str(), line.size());
            sem_post(semParent2);
            sem_wait(semChild2);
            munmap(addr, MMAP_SIZE);
        }
    }

    //cигналим детям завершение чтобы они вышли из своих циклов чтения
    {
        // child1
        char* addr1 = MapSharedMemory(MMAP_SIZE, shmFd1);
        addr1[0] = '\0'; // признак окончания
        sem_post(semParent1); //указываю ребёнку что строка пуста (увеличивает значение семафора)
        munmap(addr1, MMAP_SIZE);
        // child2
        char* addr2 = MapSharedMemory(MMAP_SIZE, shmFd2);
        addr2[0] = '\0';
        sem_post(semParent2);
        munmap(addr2, MMAP_SIZE);
    }

    // жду заверение чаилдов
    waitpid(pid1, nullptr, 0);
    waitpid(pid2, nullptr, 0);

    //закрываю всё
    close(shmFd1);
    close(shmFd2);
    shm_unlink(SHM_NAME1);
    shm_unlink(SHM_NAME2);

    sem_close(semParent1);
    sem_close(semChild1);
    sem_close(semParent2);
    sem_close(semChild2);
    sem_unlink(SEM_PARENT1);
    sem_unlink(SEM_CHILD1);
    sem_unlink(SEM_PARENT2);
    sem_unlink(SEM_CHILD2);
}
