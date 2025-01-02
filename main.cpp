#include <iostream>
#include <cstdlib>
#include "parent.hpp"

int main() {
    const char* pathToChild1 = std::getenv("CHILD1_PATH");
    const char* pathToChild2 = std::getenv("CHILD2_PATH");

    if (!pathToChild1 || !pathToChild2) {
        std::cerr << "Ошибка: переменные окружения CHILD1_PATH / CHILD2_PATH не установлены.\n";
        return 1;
    }

    ParentProcess(pathToChild1, pathToChild2, std::cin);

    return 0;
}



// export CHILD1_PATH="/mnt/c/Users/user/Desktop/Labs/os/lr3/build/child1"
// export CHILD2_PATH="/mnt/c/Users/user/Desktop/Labs/os/lr3/build/child2"
