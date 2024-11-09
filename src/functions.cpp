#include "functions.hpp"
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <fcntl.h>
#include <sys/wait.h>

using namespace std;

void CreatePipe(int pipeFd[2]) {
    if (pipe(pipeFd) == -1) {
        perror("Ошибка создания канала");
        exit(EXIT_FAILURE);
    }
}

pid_t CreateChild() {
    pid_t pid = fork();
    if (pid == -1) {
        perror("Ошибка создания дочернего процесса");
        exit(EXIT_FAILURE);
    }
    return pid;
}

void Exec(const char* pathToChild) {
    execl(pathToChild, pathToChild, nullptr);
    perror("Ошибка выполнения дочернего процесса");
    exit(EXIT_FAILURE);
}

void processChild() {
    string input;
    int length;
    while (read(STDIN_FILENO, &length, sizeof(length)) > 0) {
        char* buffer = new char[length + 1];
        read(STDIN_FILENO, buffer, length);
        buffer[length] = '\0';
        input = buffer;
        delete[] buffer;
        // удалаяю гласные и вывод результата
        string output = removeVowels(input);
        cout << output << endl;
    }
}

string removeVowels(const string& input) {
    string result;
    for (char ch : input) {
        if (ch != 'a' && ch != 'e' && ch != 'i' && ch != 'o' && ch != 'u' &&
            ch != 'A' && ch != 'E' && ch != 'I' && ch != 'O' && ch != 'U') {
            result += ch;
        }
    }
    return result;
}
