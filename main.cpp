#include <iostream>
#include <cstdlib>
#include "parent.hpp"

int main() {
    // Получение путей к дочерним процессам из переменных окружения
    const char* pathToChild1 = std::getenv("CHILD1_PATH");
    const char* pathToChild2 = std::getenv("CHILD2_PATH");

    if (!pathToChild1 || !pathToChild2) {
        std::cerr << "Ошибка: пути к дочерним процессам не заданы в переменных окружения CHILD1_PATH и CHILD2_PATH." << std::endl;
        return 1;
    }

    // Запуск родительского процесса
    ParentProcess(pathToChild1, pathToChild2, std::cin);

    return 0;
}
