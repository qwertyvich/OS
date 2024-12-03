#include "functions.hpp"
#include <iostream>
#include <string>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <semaphore.h>

using namespace std;

int main() {
    // Открываем shared memory и семафор для child2
    int shm_fd = OpenSharedMemory(SHM_CHILD2);
    sem_t* sem = OpenSemaphore(SEM_CHILD2);

    // Открываем файл для записи
    string file2 = "out2.txt";
    int file_descr = open(file2.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0777);
    if (file_descr == -1) {
        perror(("Ошибка открытия файла " + file2).c_str());
        exit(EXIT_FAILURE);
    }

    while (true) {
        // Ожидаем сигнал от родителя
        sem_wait(sem);

        // Читаем строку из shared memory
        string input = ReadFromSharedMemory(shm_fd);

        // Если получена пустая строка, завершаем работу
        if (input.empty()) {
            break;
        }

        // Удаляем гласные
        string output = removeVowels(input);

        // Записываем результат в файл
        if (write(file_descr, output.c_str(), output.size()) == -1) {
            perror("Ошибка записи в файл");
            break;
        }
        if (write(file_descr, "\n", 1) == -1) {
            perror("Ошибка записи в файл");
            break;
        }

        // Выводим результат в стандартный вывод
        cout << "Child2: " << output << endl;
    }

    // Закрываем ресурсы
    close(file_descr);
    close(shm_fd);
    sem_close(sem);

    return 0;
}
