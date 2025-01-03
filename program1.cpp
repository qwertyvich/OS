// статическая линковка
#include <iostream>
#include "functions.hpp"

int main() {
    std::cout << "Программа №1 (статическая линковка библиотеки: lib1)\n";

    while (true) {
        std::cout << "\nВведите команду:\n";
        std::cout << "1 x - вычислить e \n";
        std::cout << "2 x - перевести число в двоичную систему\n";
        std::cout << "3 - выйти\n";
        
        int command;
        std::cin >> command;

        if (command == 3) {
            // Выход
            std::cout << "Завершение работы.\n";
            break;
        }

        if (command == 1) {
            int x;
            std::cin >> x;
            float result = E_calc(x);
            if (result < 0) {
                std::cout << "Ошибка вычисления. Возможно, x <= 0.\n";
            } else {
                std::cout << "e(" << x << ") = " << result << "\n";
            }
        } else if (command == 2) {
            long x;
            std::cin >> x;
            char* result = translation(x);
            std::cout << "Число в двоичной системе: " << result << "\n";
        } else {
            std::cout << "Неверная команда\n";
        }
    }

    return 0;
}
