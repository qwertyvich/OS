// динамическая загрузка
#include <iostream>
#include <dlfcn.h>
#include <cstdlib>   // для getenv
#include <cstring>   // для ошибок dlerror()

// типы указателей на функции
using EFunc = float(*)(int);
using TranslationFunc = char*(*)(long);

int main() {
    std::cout << "Программа №2 (динамическая загрузка библиотек)\n";

    const char* pathToLib1 = std::getenv("PATH_TO_LIB1");
    if (!pathToLib1) {
        std::cerr << "Переменная окружения PATH_TO_LIB1 не установлена\n";
        return 1;
    }
    const char* pathToLib2 = std::getenv("PATH_TO_LIB2");
    if (!pathToLib2) {
        std::cerr << "Переменная окружения PATH_TO_LIB2 не установлена\n";
        return 1;
    }

    // сохраняю пути в массив
    const char* lib_paths[] = { pathToLib1, pathToLib2 };
    int current_lib = 0; // по умолчанию гружу lib1

    // гружу библиотеку
    void* handle = dlopen(lib_paths[current_lib], RTLD_LAZY);
                    //dlopen(const char* filename, int flag) LAZY загружать символы по мере необходимости.
    if (!handle) {
        std::cerr << "Ошибка загрузки библиотеки: " << dlerror() << "\n";
        return 1;
    }

    // ищем символы
    //              reinterpret_cast- приведение типа
    //                                      dlsym  - ищет символы в библиотеках динамических
    EFunc E_calc = reinterpret_cast<EFunc>(dlsym(handle, "E_calc"));
    TranslationFunc translation = reinterpret_cast<TranslationFunc>(dlsym(handle, "translation"));

    char* error = dlerror();
    if (error) {
        std::cerr << "Ошибка получения символа: " << error << "\n";
        dlclose(handle);
        return 1;
    }

    // основной цикл ввода команд
    while (true) {
        std::cout << "\nТекущая библиотека: " << lib_paths[current_lib] << "\n";
        std::cout << "Введите команду:\n";
        std::cout << "0 - переключить реализацию (между lib1 и lib2)\n";
        std::cout << "1 x - вычислить e\n";
        std::cout << "2 x - перевести число в другую систему счисления\n";
        std::cout << "3 - выйти\n";

        int command;
        std::cin >> command;

        if (command == 3) {
            std::cout << "Завершение работы.\n";
            break;
        }

        if (command == 0) {
            // переключаемся между библиотеками
            dlclose(handle); // закрываем предыдущую
            current_lib = 1 - current_lib; // переключаемся
            handle = dlopen(lib_paths[current_lib], RTLD_LAZY);
            if (!handle) {
                std::cerr << "Ошибка загрузки библиотеки: " << dlerror() << "\n";
                return 1;
            }
            E_calc = reinterpret_cast<EFunc>(dlsym(handle, "E_calc"));
            translation = reinterpret_cast<TranslationFunc>(dlsym(handle, "translation"));
            error = dlerror();
            if (error) {
                std::cerr << "Ошибка получения символа: " << error << "\n";
                dlclose(handle);
                return 1;
            }
        } 
        else if (command == 1) {
            // вычислить e
            int x;
            std::cin >> x;
            float result = E_calc(x);
            if (result < 0) {
                std::cout << "Ошибка вычисления e. Возможно x <= 0 для первой реализации "
                             "или x < 0 для второй.\n";
            } else {
                std::cout << "e(" << x << ") = " << result << "\n";
            }
        }
        else if (command == 2) {
            // перевести в другую систему счисления
            long x;
            std::cin >> x;
            char* result = translation(x);
            std::cout << "Перевод числа " << x << " = " << result << "\n";
        }
        else {
            std::cout << "Неверная команда\n";
        }
    }

    dlclose(handle);
    return 0;
}
