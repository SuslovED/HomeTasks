#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "words.h"

/* Внутренняя функция:
 * гарантирует, что capacity >= newSize + 1 (место под NULL).
 */
static int ensureListCapacity(StringList *list, int newSize) {
    if (newSize + 1 <= list->capacity) {
        return 0;
    }

    int newCap = list->capacity;
    if (newCap == 0) {
        newCap = LIST_BLOCK_SIZE;
    }
    while (newCap < newSize + 1) {
        newCap += LIST_BLOCK_SIZE;
    }

    char **tmp = realloc(list->data, (size_t)newCap * sizeof(char *));
    if (!tmp) {
        fprintf(stderr, "Ошибка: не удалось увеличить память под список слов\n");
        return -1;
    }

    list->data = tmp;
    list->capacity = newCap;
    return 0;
}

void initList(StringList *list) {
    list->data = NULL;
    list->size = 0;
    list->capacity = 0;
}

void clearList(StringList *list) {
    if (!list) return;

    if (list->data) {
        for (int i = 0; i < list->size; ++i) {
            free(list->data[i]);
        }
        free(list->data);
    }

    list->data = NULL;
    list->size = 0;
    list->capacity = 0;
}

int addWord(StringList *list, char *word) {
    if (!word) {
        return 0;
    }

    if (ensureListCapacity(list, list->size + 1) != 0) {
        free(word);
        return -1;
    }

    list->data[list->size++] = word;
    list->data[list->size] = NULL; /* завершающий NULL */
    return 0;
}

void finalizeList(StringList *list) {
    if (!list->data) {
        /* Пустой список: создаём массив из одного NULL */
        list->data = (char **)malloc(sizeof(char *));
        if (!list->data) {
            fprintf(stderr, "Ошибка: не удалось выделить память под пустой список\n");
            list->size = 0;
            list->capacity = 0;
            return;
        }
        list->data[0] = NULL;
        list->capacity = 1;
        return;
    }

    if (list->size + 1 < list->capacity) {
        char **tmp = realloc(list->data,
                             (size_t)(list->size + 1) * sizeof(char *));
        if (tmp) {
            list->data = tmp;
            list->capacity = list->size + 1;
        }
    }

    list->data[list->size] = NULL;
}

void printList(const StringList *list) {
    if (!list || !list->data) {
        printf("0\n");
        return;
    }

    printf("%d\n", list->size);
    for (int i = 0; i < list->size; ++i) {
        printf("%s\n", list->data[i]);
    }
}

void sortList(StringList *list) {
    if (!list || list->size <= 1 || !list->data) {
        return;
    }

    /* сортировка вставками */
    for (int i = 1; i < list->size; ++i) {
        char *key = list->data[i];
        int j = i - 1;
        while (j >= 0 && strcmp(list->data[j], key) > 0) {
            list->data[j + 1] = list->data[j];
            --j;
        }
        list->data[j + 1] = key;
    }

    list->data[list->size] = NULL;
}