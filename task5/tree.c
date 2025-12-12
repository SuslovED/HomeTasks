#include "tree.h"

/* Ошибка красным цветом */
void error(char *str) {
    fprintf(stderr, "\n%s%s%s\n\n", "\033[1;31m", str, "\033[0m"); 
}

/* Функция для освобождения памяти, занятой списком аргументов команды */
static void clear_argv(char **argv) { 
    if (argv == NULL)
        return;
    for (int i = 0; argv[i] != NULL; i++)
        free(argv[i]);
    free(argv);
    argv = NULL;
}

/* Добавляет аргументы из списка lst в структуру tree */
static void add_arg(List lst, tree *cur_tree) { 
    char **term = NULL;
    int size_term = 0;
    char *lex = *lst;
    while (lex != NULL && symset(*lex)) {
        if (term == NULL) {
            term = malloc(2*sizeof(char*));
            *term = NULL;
        } else 
            term = realloc(term, (size_term + 2)*sizeof(char*));
        size_term++;
        term[size_term - 1] = malloc((strlen(lex) + 1)*sizeof(char));
        term[size_term] = NULL;
        strcpy(term[size_term - 1], lex);
        lst++;
        lex = *lst;
    }
    cur_tree->argc = size_term;
    cur_tree->argv = term;
}

/* Очищает память, выделенную для дерева команд */
void clear_tree(tree **root) {
    tree *t_tree = *root;
    if (t_tree == NULL)
        return;
    clear_argv(t_tree->argv);
    clear_tree(&t_tree->pipe);
    clear_tree(&t_tree->next);
    free(t_tree);
    *root = NULL;
}

/* Создает новый узел дерева команд с пустыми значениями */
static tree *null_tree() {   
    tree *cur = malloc(sizeof(tree));
    cur->argc = 0;
    cur->argv = NULL;
    cur->infile = NULL;
    cur->outfile = NULL;
    cur->append = 0;
    cur->backgrnd = 0;
    cur->tnext = 0;
    cur->pipe = NULL;
    cur->next = NULL;
    return cur;
}

/* Устанавливает флаг фоновости (backgrnd = 1) для всех узлов дерева */
static void make_bgrnd(tree *t) {
    if (t == NULL)
        return;
    t->backgrnd = 1;
    make_bgrnd(t->pipe);
}

/* Обрабатывает операторы перенаправления ввода/вывода */
static tree *redir_file(List plex) {
    tree *cur_tree = null_tree();
    add_arg(plex, cur_tree);
    plex = plex + cur_tree->argc ;
    if (*plex != NULL && **plex == '<') {
        plex++;
        if (*plex == NULL) {
            error("Отсутствует аргумент для ввода из файла!\0");
            clear_tree(&cur_tree);
            return NULL;
        }
        cur_tree->infile = *plex;
        plex++;
    }
    if (*plex != NULL && **plex == '>') {
        if ((*plex)[1] == '>')
            cur_tree->append = 1;
        plex++;
        if (*plex == NULL) {
            error("Отсутствует аргумент для вывода в файл!\0");
            clear_tree(&cur_tree);
            return NULL;
        }
        cur_tree->outfile = *plex;
    }
    return cur_tree;
} 

/* Проверяет, является ли текущий элемент списка оператором */
static char **conv_or_not(List plex) {
    while(*plex != NULL && strcmp(*plex, "|"))
        if (!(strcmp(*plex, "||") && strcmp(*plex, "&&") && strcmp(*plex, ";") && strcmp(*plex, "&")))
            return NULL;
        else
            plex++;
    if (*plex == NULL)
        return NULL;
    return plex;
}

/* Строит конвейер команд */
static tree *conveyor(tree *cur_tree, List plex) {
    tree *prev_tree = cur_tree;
    cur_tree = redir_file(plex);
    tree *root = cur_tree;
    while ((plex = conv_or_not(plex))) { 
        plex++;
        if (*plex == NULL) {
            error("Конвейер не завершен!\0");
            return NULL;
        }
        prev_tree = cur_tree;
        cur_tree = redir_file(plex);
        prev_tree->pipe = cur_tree;
    }
    return root;
}

/* Строит дерево команд с учётом оператора ;, &&, || и конвейеров */
static tree *list_of_command(tree* cur_tree, List plex) {
    tree *prev_tree = cur_tree;
    cur_tree = conveyor(cur_tree, plex);
    tree *root = cur_tree;
    int next;
    while (*plex != NULL) {
        if (cur_tree == NULL)
            return NULL;
        if (cur_tree->argv != NULL) {
            while (*plex != NULL) {
                if (**plex == '&' || **plex == ';'  || !strcmp(*plex, "||"))
                    break;
                plex++;
            }
            if  (*plex == NULL)
                break;
            int flag = 0;
            if(!strcmp(*plex, ";")) {
                next = 1;
                flag = 1;
                plex++;
            } else if (!strcmp(*plex, "&")) {
                flag = 1;
                make_bgrnd(cur_tree);
                plex++;
            } else if (!strcmp(*plex, "&&")) {
                next = 2;
                plex++;
            } else if (!strcmp(*plex, "||")) {
                next = 3;
                plex++;
            }
            if (*plex == NULL && flag)
                break;
            else if (*plex == NULL) {
                error("Отсутствует команда после && или ||!\0");
                clear_tree(&cur_tree);
                return NULL;
            }
        } else {
            error("Отсутствуют аргументы!\0");
            clear_tree(&cur_tree);
            return NULL;
        }
        prev_tree = cur_tree;
        cur_tree = conveyor(prev_tree, plex);
        if (cur_tree)
            cur_tree->tnext = next;
        prev_tree->next = cur_tree;
    }
    return root;
}

/* Функция для создания дерева команд из списка */
tree *create_tree(List lst) {
    tree *cur_tree = NULL;
    cur_tree = list_of_command(cur_tree, lst);
    return cur_tree;
}


static void make_tab(int n) {
    while (n--)
        printf(" ");
}

static void print_argv(char **p, int shift) {
    char **q = p;
    if (p != NULL) {
        while (*p != NULL) {
             make_tab(shift);
             printf("argument %d = %s\n",(int) (p-q), *p);
             p++;
        }
    }
}


void print_tree(tree *t, int shift) {
    if (t == NULL)
        return;
    char **p;
    p = t->argv;
    make_tab(shift);
    printf("%snumber of arguments = %d\n","\033[1;35m", t->argc);
    if (p != NULL)
        print_argv(p, shift);
    make_tab(shift);
    if (t->infile == NULL)
        printf("input from a file = no\n");
    else
        printf("input from a file = %s\n", t->infile);
    make_tab(shift);
    if (t->outfile == NULL)
        printf("output to a file = no\n");
    else
        printf("output to a file = %s\n", t->outfile);
    make_tab(shift);
    printf("output to the %s of the file\n", t->append == 0 ? "begin" : "end");
    make_tab(shift);
    printf("working in %s\n", t->backgrnd == 0 ? "normal mode" : "the background");
    make_tab(shift);
    printf("type = %s\n", t->tnext == 1 ? "NXT" : t->tnext == 2 ? "AND" : t->tnext == 3 ? "OR" : "0");
    printf("%s", "\033[0m");
    make_tab(shift);
    if (t->pipe == NULL)
        printf("%sconveyor = no%s\n", "\033[1;35m", "\033[0m");
    else {
        printf("%sconveyor:%s\n", "\033[1;35m", "\033[0m");
        print_tree(t->pipe, shift + 5);
    }
    make_tab(shift);
    if (t->next == NULL)
        printf("%snext = end%s\n", "\033[1;35m", "\033[0m");
    else {
        printf("%snext:%s\n", "\033[1;35m", "\033[0m");
        print_tree(t->next, shift + 5);
    }
}