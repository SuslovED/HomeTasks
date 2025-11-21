#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    int i = 1;
    int newline = 1;  /* по умолчанию печатаем \n */

    /* Поддержка флага -n */
    if (i < argc && strcmp(argv[i], "-n") == 0) {
        newline = 0;
        i++;
    }

    /* Печатаем все оставшиеся аргументы через пробел */
    for (; i < argc; i++) {
        fputs(argv[i], stdout);
        if (i + 1 < argc) {
            putchar(' ');
        }
    }

    if (newline) {
        putchar('\n');
    }

    return 0;
}
