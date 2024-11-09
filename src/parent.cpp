#include "parent.hpp"
#include "functions.hpp"
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

using namespace std;

void ParentProcess(const char* pathToChild1, const char* pathToChild2, istream& streamIn) {
    int child1[2], child2[2];

    // создаю каналы и проверяю успешность их создания
    CreatePipe(child1);

    CreatePipe(child2);
    srand(time(nullptr));

    string file1="out1.txt", file2="out2.txt";
    //cout << "Введите имя файла для child1: ";
    //getline(streamIn, file1);
    //cout << "Введите имя файла для child2: ";
    //getline(streamIn, file2);


    int file1Descr = open(file1.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0777);
    int file2Descr = open(file2.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0777);
    if (file1Descr == -1) {
        cerr << "Ошибка открытия файла для child1: " << file1 << endl;
        exit(1);
    }
    if (file2Descr == -1) {
        cerr << "Ошибка открытия файла для child2: " << file2 << endl;
        exit(1);
    }

    // первый дочерний проц
    pid_t pid1 = CreateChild();
    if (pid1 == -1) {
        cerr << "Ошибка создания первого дочернего процесса." << endl;
        close(file1Descr);
        close(file2Descr);
        exit(1);
    } else if (pid1 == 0) {
        // Код для первого дочернего процесса
        close(child1[WRITE_END]);
        close(child2[READ_END]);
        close(child2[WRITE_END]);
        // STDIN_FILENO — это дескриптор файла для стандартного ввода.
        // dup2 используется для дублирования файлового дескриптора 
        //и перенаправления потока ввода или вывода одного дескриптора на другой.
        if (dup2(child1[READ_END], STDIN_FILENO) == -1) {
            perror("Ошибка перенаправления stdin для child1");
            exit(EXIT_FAILURE);
        }
        if (dup2(file1Descr, STDOUT_FILENO) == -1) {
            perror("Ошибка перенаправления stdout для child1");
            exit(EXIT_FAILURE);
        }
        close(file1Descr);
        Exec(pathToChild1); //заменяет текущий процесс на новый
    } else {
        // создаю вторй дочерний процесс
        pid_t pid2 = CreateChild();
        if (pid2 == -1) {
            cerr << "Ошибка создания второго дочернего процесса." << endl;
            close(file1Descr);
            close(file2Descr);
            exit(1);
        } else if (pid2 == 0) {
            // Код для второго дочернего процесса
            close(child2[WRITE_END]);       //оставляю на запись второй процесс
            close(child1[READ_END]);
            close(child1[WRITE_END]);

            if (dup2(child2[READ_END], STDIN_FILENO) == -1) {
                perror("Ошибка перенаправления stdin для child2");
                exit(EXIT_FAILURE);
            }
            if (dup2(file2Descr, STDOUT_FILENO) == -1) {
                perror("Ошибка перенаправления stdout для child2");
                exit(EXIT_FAILURE);
            }
            close(file2Descr);
            Exec(pathToChild2);
        } else {
            //выполняется только в родительском процессе, 
            //после того как он создал оба дочерних процесса.
            close(child1[READ_END]);
            close(child2[READ_END]);

            // Передача строк в один из каналов с вероятностью 80% на pipe1, 20% на pipe2
            string input;
            while (getline(streamIn, input)) {
                int length = input.size();
                int pipe_write_end;

                //       с вероятностью 80% отправляем в pipe1, иначе в pipe2
                if (rand() % 10 < 8) {
                    pipe_write_end = child1[WRITE_END];
                } else {
                    pipe_write_end = child2[WRITE_END];
                }

                // Отправка длины и содержимого строки
                if (write(pipe_write_end, &length, sizeof(length)) == -1) {
                    perror("Ошибка записи длины строки в канал");
                    break;
                }
                if (write(pipe_write_end, input.c_str(), length) == -1) {
                    perror("Ошибка записи строки в канал");
                    break;
                }
            }

            // выполноил закрытие дескрипторов записи после завершения передачи данных
            close(child1[WRITE_END]);
            close(child2[WRITE_END]);
            close(file1Descr);
            close(file2Descr);

            // ожидает завершения обоих дочерних процессов
            waitpid(pid1, nullptr, 0);
            waitpid(pid2, nullptr, 0);
        }
    }
}
