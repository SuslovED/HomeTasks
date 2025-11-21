#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>

struct options {
    int recursive;    /* флаг -R */
    int long_format;  /* флаг -l */
    int show_group;   /* флаг -g */
};

/* показать тип файла в первой позиции выходной строки */
static void display_file_type(int st_mode) {
    switch (st_mode & S_IFMT) {
        case S_IFDIR:  putchar('d'); break;
        case S_IFCHR:  putchar('c'); break;
        case S_IFBLK:  putchar('b'); break;
        case S_IFREG:  putchar('-'); break;
        case S_IFLNK:  putchar('l'); break;
        case S_IFSOCK: putchar('s'); break;
        case S_IFIFO:  putchar('p'); break;
        default:       putchar('?'); break;
    }
}

/* показать права доступа для владельца, группы и прочих пользователей */
static void display_permission(int st_mode) {
    static const char xtbl[10] = "rwxrwxrwx";
    char amode[10];
    int i;
    int j;

    for (i = 0, j = (1 << 8); i < 9; i++, j >>= 1) {
        amode[i] = (st_mode & j) ? xtbl[i] : '-';
    }
    if (st_mode & S_ISUID) {
        amode[2] = 's';
    }
    if (st_mode & S_ISGID) {
        amode[5] = 's';
    }
    if (st_mode & S_ISVTX) {
        amode[8] = 't';
    }
    amode[9] = '\0';
    printf("%s ", amode);
}

/* печать одной записи про файл в длинном формате (-l / -g) */
static void print_long_entry(const char *name,
                             const char *fullpath,
                             const struct stat *st,
                             const struct options *opt) {
    struct passwd *pw;
    struct group  *gr;

    display_file_type(st->st_mode);
    display_permission(st->st_mode);

    pw = getpwuid(st->st_uid);
    if (pw != NULL) {
        printf("%s ", pw->pw_name);
    } else {
        printf("%d ", (int)st->st_uid);
    }

    if (opt->show_group) {
        gr = getgrgid(st->st_gid);
        if (gr != NULL) {
            printf("%s ", gr->gr_name);
        } else {
            printf("%d ", (int)st->st_gid);
        }
    }

    printf("%lld ", (long long)st->st_size);

    printf("%s", name);

    /* если это символическая ссылка, показываем, куда она указывает */
    if (S_ISLNK(st->st_mode)) {
        char target[4096];
        ssize_t len = readlink(fullpath, target, sizeof(target) - 1);
        if (len >= 0) {
            target[len] = '\0';
            printf(" -> %s", target);
        }
    }

    putchar('\n');
}

/* печать одной записи про файл в обычном режиме (без -l) */
static void print_short_entry(const char *name) {
    printf("%s\n", name);
}

/* универсальная печать записи про файл (с учётом флагов) */
static void print_entry(const char *name,
                        const char *fullpath,
                        const struct stat *st,
                        const struct options *opt) {
    if (opt->long_format) {
        print_long_entry(name, fullpath, st, opt);
    } else {
        print_short_entry(name);
    }
}

/* компаратор имён для qsort */
static int namecmp(const void *a, const void *b) {
    const char *const *sa = (const char *const *)a;
    const char *const *sb = (const char *const *)b;
    return strcmp(*sa, *sb);
}

/* вывод содержимого одного каталога, с сортировкой имён */
static void list_dir(const char *path, const struct options *opt, int print_header) {
    DIR *dir;
    struct dirent *entry;
    char **names = NULL;
    size_t count = 0;
    size_t cap = 0;
    size_t i;

    dir = opendir(path);
    if (dir == NULL) {
        perror(path);
        return;
    }

    /* сначала читаем все имена и сохраняем их в массив */
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        if (count == cap) {
            size_t new_cap = (cap == 0) ? 16 : cap * 2;
            char **tmp = (char **)realloc(names, new_cap * sizeof(char *));
            if (tmp == NULL) {
                perror("realloc");
                break;
            }
            names = tmp;
            cap = new_cap;
        }

        names[count] = strdup(entry->d_name);
        if (names[count] == NULL) {
            perror("strdup");
            break;
        }
        count++;
    }

    closedir(dir);

    if (count == 0) {
        if (print_header) {
            printf("%s:\n", path);
        }
        free(names);
        return;
    }

    /* сортируем имена по алфавиту */
    qsort(names, count, sizeof(char *), namecmp);

    if (print_header) {
        printf("%s:\n", path);
    }

    /* теперь печатаем и, при необходимости, рекурсивно обходим подкаталоги */
    for (i = 0; i < count; i++) {
        char full[4096];
        struct stat st;

        snprintf(full, sizeof(full), "%s/%s", path, names[i]);

        if (lstat(full, &st) == -1) {
            perror(full);
            free(names[i]);
            continue;
        }

        print_entry(names[i], full, &st, opt);

        if (opt->recursive && S_ISDIR(st.st_mode)) {
            printf("\n");
            list_dir(full, opt, 1);
        }

        free(names[i]);
    }

    free(names);
}

/* разбор флагов командной строки: -R, -l, -g */
static int parse_options(int argc, char *argv[], struct options *opt) {
    int i;

    opt->recursive = 0;
    opt->long_format = 0;
    opt->show_group = 0;

    i = 1;
    while (i < argc && argv[i][0] == '-' && argv[i][1] != '\0') {
        const char *p = argv[i] + 1;
        while (*p != '\0') {
            if (*p == 'R') {
                opt->recursive = 1;
            } else if (*p == 'l') {
                opt->long_format = 1;
            } else if (*p == 'g') {
                opt->show_group = 1;
                opt->long_format = 1;
            } else {
                fprintf(stderr, "ls: unknown option -- %c\n", *p);
            }
            p++;
        }
        i++;
    }

    return i;
}

int main(int argc, char *argv[]) {
    struct options opt;
    int first_path;
    int i;
    int paths_count;

    first_path = parse_options(argc, argv, &opt);
    paths_count = argc - first_path;

    /* без путей: работаем с "." */
    if (first_path == argc) {
        int print_header = opt.recursive ? 1 : 0;
        list_dir(".", &opt, print_header);
        return 0;
    }

    /* с путями: каждый аргумент обрабатываем отдельно */
    for (i = first_path; i < argc; i++) {
        const char *path = argv[i];
        struct stat st;
        int print_header;

        if (lstat(path, &st) == -1) {
            perror(path);
            continue;
        }

        /* печатаем заголовок path: только если:
           - путей несколько, или
           - включена рекурсия */
        print_header = (paths_count > 1) || opt.recursive;

        if (S_ISDIR(st.st_mode)) {
            list_dir(path, &opt, print_header);
        } else {
            if (print_header) {
                printf("%s:\n", path);
            }
            print_entry(path, path, &st, &opt);
        }

        if (i + 1 < argc) {
            printf("\n");
        }
    }

    return 0;
}


