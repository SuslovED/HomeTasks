#ifndef TREE_H
#define TREE_H
#include "list.h"

typedef struct tree {
    int argc;		// количество аргументов
    char **argv;	// список из имени команды и аргументов
    char *infile;	// переназначенный файл стандартного ввода
    char *outfile;	// переназначенный файл стандартного вывода
    int append;		// если записывать в конец файла, то = 1
    int backgrnd;	// если команда подлежит выполнению в фоновом режиме, то = 1
    int tnext;		// если ; = 1, если && = 2, если || = 3 
    struct tree *pipe;	// следующая команда после “|”
    struct tree *next;	// следующая команда после “;” , “&”, “&&”, “||”
} tree;

void error(char *);
void clear_tree(tree **);
tree *create_tree (List);
void print_tree (tree *, int);

#endif
