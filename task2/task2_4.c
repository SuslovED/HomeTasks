#include <stdio.h>
#include <ctype.h>

double pow10_int(int exp) {
    double result = 1.0;
    if (exp > 0) {
        while (exp--) result *= 10.0;
    } else {
        while (exp++) result /= 10.0;
    }
    return result;
}

double str2double(char str[]) {
    int i = 0;
    int sign = 1;
    double value = 0.0;
    double frac_part = 0.0;
    int frac_digits = 0;
    int exp_sign = 1, exp_val = 0;
    int has_digit = 0;

    /* Пропуск пробелов */
    while (isspace((unsigned char)str[i])) i++;

    /* Знак числа */
    if (str[i] == '+' || str[i] == '-') {
        if (str[i] == '-') sign = -1;
        i++;
    }

    /* Целая часть */
    while (isdigit((unsigned char)str[i])) {
        has_digit = 1;
        value = value * 10 + (str[i] - '0');
        i++;
    }

    /* Десятичная точка */
    if (str[i] == '.') {
        i++;
        if (!isdigit((unsigned char)str[i]) && !has_digit) {
            fprintf(stderr, "Ошибка: нет цифр перед или после '.' в \"%s\"\n", str);
            return 0.0;
        }
        while (isdigit((unsigned char)str[i])) {
            has_digit = 1;
            frac_part = frac_part * 10 + (str[i] - '0');
            frac_digits++;
            i++;
        }
    }

    if (!has_digit) {
        fprintf(stderr, "Ошибка: нет цифр в \"%s\"\n", str);
        return 0.0;
    }

    /* Экспонента (E/e) */
    if (str[i] == 'E' || str[i] == 'e') {
        i++;
        if (str[i] == '+' || str[i] == '-') {
            if (str[i] == '-') exp_sign = -1;
            i++;
        }
        if (!isdigit((unsigned char)str[i])) {
            fprintf(stderr, "Ошибка: неверная экспонента в \"%s\"\n", str);
            return 0.0;
        }
        while (isdigit((unsigned char)str[i])) {
            exp_val = exp_val * 10 + (str[i] - '0');
            i++;
        }
    }

    /* Суффикс f/F/l/L не допускается */
    if (str[i] == 'f' || str[i] == 'F' ||
        str[i] == 'l' || str[i] == 'L') {
        fprintf(stderr, "Ошибка: недопустимый суффикс в \"%s\"\n", str);
        return 0.0;
    }

    /* Пропуск хвостовых пробелов */
    while (isspace((unsigned char)str[i])) i++;

    /* Любой лишний символ — ошибка */
    if (str[i] != '\0') {
        fprintf(stderr, "Ошибка: неверный символ '%c' в \"%s\"\n", str[i], str);
        return 0.0;
    }

    /*  Формируем итоговое значение */
    double result = value;
    if (frac_digits > 0) {
        double denom = pow10_int(frac_digits);
        result += frac_part / denom;
    }

    /* Применяем экспоненту */
    if (exp_val != 0)
        result *= pow10_int(exp_sign * exp_val);

    return sign * result;
}

int main(void) {
    char s[256];
    while (scanf("%255s", s) == 1) {
        double x = str2double(s);
        printf("%.10g\n", x);
    }
    return 0;
}

