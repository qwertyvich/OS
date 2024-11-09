#include <iostream>
#include <cstdlib>
#include "parent.hpp"

int main() {
    // получение путей
    const char* pathToChild1 = std::getenv("CHILD1_PATH");
    const char* pathToChild2 = std::getenv("CHILD2_PATH");

    if (!pathToChild1 || !pathToChild2) {
        std::cerr << "pathes undefined" << std::endl;
        return 1;
    }

    ParentProcess(pathToChild1, pathToChild2, std::cin);

    return 0;
}

// export CHILD1_PATH="/mnt/c/Users/user/Desktop/Labs/os/lab1/build/child1"
// export CHILD2_PATH="/mnt/c/Users/user/Desktop/Labs/os/lab1/build/child2"