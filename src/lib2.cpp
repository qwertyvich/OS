#include <cmath>
#include <algorithm>
#include "functions.hpp"

static double factorial(int n) {
    // факториал
    if (n < 0) return 0.0;
    double f = 1.0;
    for (int i = 1; i <= n; ++i) {
        f *= i;
    }
    return f;
}

////e(x) = sum_{p=0..x} (1/p!)
float E_calc(int x) {
    if (x < 0) {
        return -1.0f;
    }
    // считаю сумму 1/p! от p = 0 до p = x
    double sum = 0.0;
    for (int p = 0; p <= x; p++) {
        double fact = factorial(p);
        if (fact == 0.0) {
            // на всякий случай от ошибок
            return -1.0f;
        }
        sum += 1.0 / fact;
    }
    return static_cast<float>(sum);//привёл к float
}

//перевод в троичную систему счисления
char* translation(long x) {
    static char buffer[65];
    int i = 0;

    if (x == 0) {
        buffer[i++] = '0';
        buffer[i] = '\0';
        return buffer;
    }

    if (x < 0) {
        static char errBuf[] = "-1";
        return errBuf;
    }

    while (x > 0) {
        buffer[i++] = (x % 3) + '0';
        x /= 3;
    }
    buffer[i] = '\0';

    for (int j = 0, k = i - 1; j < k; ++j, --k) {
        std::swap(buffer[j], buffer[k]);
    }
    return buffer;
}


