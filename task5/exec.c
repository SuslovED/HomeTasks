#include "exec.h"

/* Флаг для корректного завершения shell */
int flag_exit = 0;

/* Команда смены директории */
static int cd(int argc, char **argv) {
    char *s;
    if (argc == 0) {
        s = getenv("HOME");
        if (s == NULL) {
            error("Ошибка смены директории!\0");
            return 1;
        } else {
            chdir(s);
            return 0;
        }
    } else if (argc > 2) {
        error("Ошибка: cd допускает 1 аргумент!\0");
        return 1;
    } else if (chdir(argv[1])) {
        error("Ошибка смены директории!\0");
        return 1;
    }
    return 0;
}

/* Обработка перенаправления ввода и вывода */
static void files_in_out(tree *root) {
    if (root->infile != NULL) {
        int file_in;
        file_in  = open(root->infile, O_RDONLY);
        if (file_in == -1) {
            error("Входного файла не существует!\0");
            exit(1);
        } else 
            dup2(file_in, 0);
        close(file_in);
    }
    if (root->outfile != NULL) {
        int file_out;
        if (root->append == 1)
            file_out = open(root->outfile, O_WRONLY | O_APPEND | O_CREAT, 0664);
        else
            file_out = open(root->outfile, O_WRONLY | O_TRUNC | O_CREAT, 0664);
        if (file_out == -1) {
            error("Невозможно перенаправить вывод!\0");
            exit(1);
        } else 
            dup2(file_out, 1);
        close(file_out);
    }
}

/* Вывод текущей директории */
static int pwd(tree *root) {
    int status;
    if (fork() == 0) {
        char *cwd = (char *) malloc(PATH_MAX * sizeof(char));
        files_in_out(root);
        if (getcwd(cwd, sizeof(char) * PATH_MAX) == NULL) {
            error("Невозможно получить текущий путь!\0");
            free(cwd);
            exit(1);
        } else
            printf("%s\n", cwd);
        free(cwd);
        exit(0);
    }
    wait(&status);
    return status;
} 

/* Обработка и выполнение команды */
static void cmd_processing(tree *cur_tree, int backgrnd) {
    files_in_out(cur_tree);
    if ((backgrnd) && (cur_tree->backgrnd)) {
        int f = open("/dev/null", O_RDONLY);
        dup2(f, 0);
        close(f);
    }
    execvp(cur_tree->argv[0], cur_tree->argv);
    fprintf(stderr, "\n%s: команда не найдена\n\n", *(cur_tree->argv));
    exit(1);
}

/* Обработка конвейеров и фоновых процессов */
static int pipe_processing(tree *root) {
/*  внук отвечает за выполнение программ,
    сын отвечает за фоновый режим (т.е. ждать внука или нет),
    отец ждет сына  */
    if (root == NULL)
        return 0;
    if (root->argv && !strcmp(root->argv[0], "exit")) {
        flag_exit = 1;
        return 0;
    }
    if (root->argv && !strcmp(root->argv[0], "cd")) {
        return cd(root->argc, root->argv);
    }
   if (root->argv && !strcmp(root->argv[0], "pwd")) {
        return pwd(root);
    }
    int ppid, res = 0;
    if ((ppid = fork()) == 0) {
        int pid;
        ppid = getpid();
        if ((pid = fork()) == 0) {
            tree* cur_tree = root;
            int fd_temp, fd[2];
            fd_temp = -1;
            if (root->backgrnd)
                signal(SIGINT, SIG_IGN);
            else
                signal(SIGINT, SIG_DFL);
            while (cur_tree != NULL) {
                if (cur_tree == root) {
                    if (cur_tree->pipe != NULL) {       /* первая, но не последняя программа */
                        if (pipe(fd) < 0)
                            exit(1);
                        if ((pid = fork()) == 0) {
                            dup2(fd[1], 1);
                            close(fd[0]);
                            close(fd[1]);
                            cmd_processing(cur_tree, 1);
                        }
                        close(fd[1]);
                        fd_temp = fd[0];
                    } else if ((pid = fork()) == 0)     /* первая и последняя программа */
                        cmd_processing(cur_tree, 1);
                } else if (cur_tree->pipe == NULL) {    /* не первая, но последняя программа */
                    if ((pid = fork()) == 0) {
                        dup2(fd_temp, 0);
                        close(fd_temp);
                        cmd_processing(cur_tree, 0);
                    }
                    close(fd_temp);
                } else {                                /* не первая и не последняя программа */
                    if (pipe(fd) < 0)
                        exit(1);
                    if ((pid = fork()) == 0) {
                        dup2(fd_temp, 0);
                        dup2(fd[1], 1);
                        close(fd[0]);
                        close(fd[1]);
                        close(fd_temp);
                        cmd_processing(cur_tree, 0);
                    }
                    close(fd[1]);
                    close(fd_temp);
                    fd_temp = fd[0];
                }
                cur_tree = cur_tree->pipe;
            }
            waitpid(pid, &res, 0);
            while (wait(NULL) != -1);
            if (WIFEXITED(res))
                res = WEXITSTATUS(res);
            exit(res);
        }
        if (root->backgrnd)
            exit(0);
        else {
            waitpid(pid, &res, 0);
            if (WIFEXITED(res))
                res = WEXITSTATUS(res);
            exit(res);
        }
    } 
    waitpid(ppid, &res, 0);
    if (WIFEXITED(res))
        res = WEXITSTATUS(res);
    return res;
}

/* Обработка всего списка команд */
void handler(tree *cur_tree) {
    if (cur_tree == NULL)
        return;
    int status_prev = 0;
    while (cur_tree != NULL) {
        if (cur_tree->tnext < 2)                            /* один конвейер или ; */
            status_prev = pipe_processing(cur_tree);
        else if (cur_tree->tnext == 2 && status_prev == 0)  /* && */
            status_prev = pipe_processing(cur_tree);
        else if (cur_tree->tnext == 3 && status_prev == 1)  /* || */
            status_prev = pipe_processing(cur_tree);
        if (flag_exit == 1)
            return;
        cur_tree = cur_tree->next;
    }
}