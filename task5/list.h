#ifndef LIST_H
#define LIST_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>

typedef char ** List;

void clearlist(List *lst);
void printlist(List lst, int sizelist);
int symset(int c);
void changelist(List lst, int size);
int buildlist(List *lst, int *size, int *isthere);

#endif
