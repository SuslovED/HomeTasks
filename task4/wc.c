#include <stdio.h>
#include <ctype.h>

static int process_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        perror(filename);
        return 1;
    }

    long lines = 0;
    long words = 0;
    long chars = 0;
    int c;
    int in_word = 0;

    while ((c = fgetc(f)) != EOF) {
        chars++;

        if (c == '\n') {
            lines++;
        }

        if (isspace((unsigned char)c)) {
            in_word = 0;
        } else {
            if (!in_word) {
                words++;
                in_word = 1;
            }
        }
    }

    fclose(f);

    printf("%s %ld %ld %ld\n", filename, lines, words, chars);
    return 0;
}

int main(int argc, char *argv[]) {
    int i;
    int exit_code = 0;

    if (argc < 2) {
        fprintf(stderr, "Usage: wc filename...\n");
        return 1;
    }

    for (i = 1; i < argc; i++) {
        if (process_file(argv[i]) != 0) {
            exit_code = 1;
        }
    }

    return exit_code;
}
