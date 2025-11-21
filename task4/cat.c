/* cat.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int cat_stream(FILE *f, const char *name, int number_lines, long *line_counter) {
    char *line = NULL;
    size_t cap = 0;
    ssize_t len;

    (void)name; /* чтобы компилятор не ругася на неиспользованное имя */

    while ((len = getline(&line, &cap, f)) != -1) {
        if (number_lines) {
            printf("%6ld  ", *line_counter);
            (*line_counter)++;
        }
        fwrite(line, 1, (size_t)len, stdout);
    }

    free(line);
    return 0;
}

static int cat_file(const char *filename, int number_lines, long *line_counter) {
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        perror(filename);
        return 1;
    }

    cat_stream(f, filename, number_lines, line_counter);

    fclose(f);
    return 0;
}

int main(int argc, char *argv[]) {
    int number_lines = 0;
    int argi = 1;
    long line_counter = 1;
    int exit_code = 0;

    if (argi < argc && strcmp(argv[argi], "-n") == 0) {
        number_lines = 1;
        argi++;
    }

    if (argi >= argc) {
        /* нет имён файлов: читаем из stdin */
        cat_stream(stdin, NULL, number_lines, &line_counter);
        return 0;
    }

    for (; argi < argc; argi++) {
        if (cat_file(argv[argi], number_lines, &line_counter) != 0) {
            exit_code = 1;
        }
    }

    return exit_code;
}
