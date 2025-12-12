#include "exec.h"
#include <setjmp.h>
jmp_buf buf;

extern int flag_exit;

static void handler_signal() {    
    printf("\n%sПришел сигнал SIGINT%s\n", "\033[1;32m", "\033[0m");
    siglongjmp(buf, 1);
}

int main() {
    signal(SIGINT, handler_signal);
    sigsetjmp(buf, 1);
    List lst;
    tree *tr;
    int sizelist, ifeof;
    int isthere;    /* для скобок */
    char *cwd = (char *) malloc(PATH_MAX * sizeof (char));
    while (1) {
            isthere = 0;
            if (getcwd(cwd, sizeof(char) * PATH_MAX) == NULL) {
                error("Невозможно получить текущий путь к $HOME!\0");
                cwd = getenv("HOME");
                chdir(cwd);
            } else 
                printf("%s%s%s$ ", "\033[1;36m", cwd, "\033[0m");
            ifeof = buildlist(&lst, &sizelist, &isthere);
            if (!isthere) {
                if (ifeof) {        /* конец */
                    printf("\n");
                    break;
                }
                if (lst == NULL) {  /* пустая строка */
                    fflush(stdin);
                    clearlist(&lst);
                    continue;
                }
                changelist(lst, sizelist);
                tr = create_tree(lst);
                print_tree(tr, 0);
                handler(tr);
                clear_tree(&tr);
            } else
                fprintf(stderr, "реализация скобок этой программой не предусмотрена\n");
            clearlist(&lst);
            if (flag_exit) 
                break;
    }
    free(cwd); 
    return 0;
}