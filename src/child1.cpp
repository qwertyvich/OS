#include "functions.hpp"
#include <iostream>
#include <string>
#include <unistd.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <cstdlib>

int main(int argc, char* argv[]) {
    // Ожидаем 3 аргумента:
    //   argv[1] - имя семафора родителя -> child1
    //   argv[2] - имя семафора child1 -> родитель
    //   argv[3] - имя shm
    if (argc < 4) {
        std::cerr << "Usage: child1 <semParent> <semChild> <shmName>\n";
        exit(EXIT_FAILURE);
    }

    const char* semParentName = argv[1];
    const char* semChildName  = argv[2];
    const char* shmName       = argv[3];

    sem_t* semParent = sem_open(semParentName, 0);
    sem_t* semChild  = sem_open(semChildName, 0);
    if (semParent == SEM_FAILED || semChild == SEM_FAILED) {
        std::cerr << "child1: sem_open failed\n";
        exit(EXIT_FAILURE);
    }

    int shmFd = shm_open(shmName, O_RDWR, 0666);
    if (shmFd == -1) {
        std::cerr << "child1: shm_open failed\n";
        exit(EXIT_FAILURE);
    }

    char* addr = MapSharedMemory(MMAP_SIZE, shmFd);

    while (true) {
        sem_wait(semParent);  // ждём "разрешения" от родителя

        if (addr[0] == '\0') { // пустая строка -> конец
            break;
        }
        std::string input(addr);
        std::string output = removeVowels(input);
        std::cout << output << std::endl;  // на stdout, перенаправленный в нужный файл
        sem_post(semChild); // сообщаем родителю, что обработали
    }

    munmap(addr, MMAP_SIZE);
    close(shmFd);
    sem_close(semParent);
    sem_close(semChild);

    return 0;
}
