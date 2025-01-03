#include <cmath>
#include <algorithm>
#include "functions.hpp"

//e(x) = (1 + 1/x)^x
float E_calc(int x) {
    if (x <= 0) {
        return -1.0f; 
    }
    // static_cast оператор явновго присвоения типов
    double xd = static_cast<double>(x);
    return std::pow(1.0 + 1.0/xd, xd);
}
//перевод в двоичную систему счисления
char* translation(long x) {
    static char buffer[65];  // резулььаь в виде массива
    int i = 0;

    //случай когда х=0
    if (x == 0) {
        buffer[i++] = '0';
        buffer[i] = '\0';
        return buffer;
    }
    if (x < 0) {
        // Простая заглушка: вернём "-1"
        // Или можно добавить логику перевода отрицательных чисел
        static char errBuf[] = "-1";
        return errBuf;
    }
    while (x > 0) {
        buffer[i++] = (x % 2) + '0';
        x /= 2;
    }
    buffer[i] = '\0';
    // перевернул строку
    for (int j = 0, k = i - 1; j < k; ++j, --k) {
        std::swap(buffer[j], buffer[k]);
    }
    return buffer;
}

