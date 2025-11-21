#include <stdio.h>
#include <string.h>
#include <unistd.h>

static void usage(void) {
    fprintf(stderr, "Usage: ln [-s] existing_file new_link\n");
}

int main(int argc, char *argv[]) {
    int symbolic = 0;
    int argi = 1;

    if (argi < argc && strcmp(argv[argi], "-s") == 0) {
        symbolic = 1;
        argi++;
    }

    if (argc - argi != 2) {
        usage();
        return 1;
    }

    const char *oldpath = argv[argi];
    const char *newpath = argv[argi + 1];

    if (symbolic) {
        if (symlink(oldpath, newpath) == -1) {
            perror("ln");
            return 1;
        }
    } else {
        if (link(oldpath, newpath) == -1) {
            perror("ln");
            return 1;
        }
    }

    return 0;
}
