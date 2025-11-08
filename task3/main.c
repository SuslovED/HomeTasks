#include <stdio.h>
#include <stdlib.h>
#include "words.h"

/* Размер блока для чтения из stdin */
#define INPUT_BLOCK_SIZE 64
/* Шаг увеличения буфера текущего слова */
#define BUF_BLOCK_SIZE 16

/* Макросы для "%Nc" */
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

/* Текущий символ (int, чтобы уметь хранить EOF) */
static int currentChar;

/* Блок ввода */
static char inputBlock[INPUT_BLOCK_SIZE];
static int inputPos = 0;   /* текущая позиция в блоке */
static int inputLen = 0;   /* количество валидных символов в блоке */
static int eofReached = 0; /* флаг EOF */

/* Буфер для накопления текущего слова */
static char *buf = NULL;
static int bufSize = 0;
static int bufPos = 0;

/* Вершины L-графа */
typedef enum {
    Start,
    Simple,
    Pipe,
    Amp,
    Greater,
    Stop
} Vertex;


static int getSym(void);
static void nullBuf(void);
static int addSym(int ch);
static int addWordFromBuf(StringList *list);
static int isSimpleChar(int ch);

static Vertex start(StringList *list);
static Vertex simple(StringList *list);
static Vertex pipe(StringList *list);
static Vertex amp(StringList *list);
static Vertex greater(StringList *list);

/* ================= main ================= */

int main(void) {
    StringList list;
    initList(&list);
    nullBuf();

    currentChar = getSym();
    Vertex v = Start;

    while (1) {
        switch (v) {
            case Start:
                v = start(&list);
                break;
            case Simple:
                v = simple(&list);
                break;
            case Pipe:
                v = pipe(&list);
                break;
            case Amp:
                v = amp(&list);
                break;
            case Greater:
                v = greater(&list);
                break;
            case Stop:
                clearList(&list);
                return 0;
        }
    }
}


/*
 * getSym
 * Считывает вход блоками по INPUT_BLOCK_SIZE символов с помощью fscanf("%Nc").
 * Для последнего (неполного) блока дочитывает одиночные символы до EOF.
 * Возвращает очередной символ или EOF.
 */
static int getSym(void) {
    if (eofReached) {
        return EOF;
    }

    if (inputPos >= inputLen) {
        inputPos = 0;
        inputLen = 0;

        int r = fscanf(stdin, "%" STR(INPUT_BLOCK_SIZE) "c", inputBlock);
        if (r == 1) {
            inputLen = INPUT_BLOCK_SIZE;
        } else {
            int ch;
            while (inputLen < INPUT_BLOCK_SIZE &&
                   (ch = fgetc(stdin)) != EOF) {
                inputBlock[inputLen++] = (char)ch;
            }

            if (inputLen == 0) {
                eofReached = 1;
                return EOF;
            }
        }
    }

    return (unsigned char)inputBlock[inputPos++];
}

/* Обнулить буфер текущего слова */
static void nullBuf(void) {
    buf = NULL;
    bufSize = 0;
    bufPos = 0;
}

/* Добавить символ в буфер текущего слова */
static int addSym(int ch) {
    if (bufPos + 1 > bufSize) {
        int newSize = bufSize + BUF_BLOCK_SIZE;
        char *tmp = realloc(buf, (size_t)newSize);
        if (!tmp) {
            fprintf(stderr, "Ошибка: не удалось выделить память для слова\n");
            free(buf);
            buf = NULL;
            bufSize = 0;
            bufPos = 0;
            return -1;
        }
        buf = tmp;
        bufSize = newSize;
    }
    buf[bufPos++] = (char)ch;
    return 0;
}

/* Завершить слово из буфера и добавить его в список */
static int addWordFromBuf(StringList *list) {
    if (!buf || bufPos == 0) {
        return 0;
    }

    if (bufPos + 1 > bufSize) {
        int newSize = bufPos + 1;
        char *tmp = realloc(buf, (size_t)newSize);
        if (!tmp) {
            fprintf(stderr, "Ошибка: не удалось завершить слово (нет памяти)\n");
            free(buf);
            buf = NULL;
            bufSize = 0;
            bufPos = 0;
            return -1;
        }
        buf = tmp;
        bufSize = newSize;
    }

    buf[bufPos] = '\0';

    char *word = realloc(buf, (size_t)(bufPos + 1));
    if (!word) {
        word = buf;
    }

    if (addWord(list, word) != 0) {
        buf = NULL;
        bufSize = 0;
        bufPos = 0;
        return -1;
    }

    buf = NULL;
    bufSize = 0;
    bufPos = 0;
    return 0;
}

/* Простое слово: буквы, цифры, $, '_', '/', '.' */
static int isSimpleChar(int ch) {
    return ((ch >= '0' && ch <= '9') ||
            (ch >= 'A' && ch <= 'Z') ||
            (ch >= 'a' && ch <= 'z') ||
            ch == '$' || ch == '_' || ch == '/' || ch == '.');
}

/* ============ Реализация L-графа ============ */

/* Start: между словами / начало строки */
static Vertex start(StringList *list) {
    /* пропускаем пробелы и табы */
    while (currentChar == ' ' || currentChar == '\t') {
        currentChar = getSym();
    }

    /* EOF: печатаем только если есть хотя бы одно слово */
    if (currentChar == EOF) {
        if (bufPos > 0) {
            if (addWordFromBuf(list) != 0) {
                clearList(list);
                return Stop;
            }
        }
        if (list->size > 0) {
            finalizeList(list);
            printList(list);
            sortList(list);
            printList(list);
            clearList(list);
        }
        return Stop;
    }

    /* конец строки: печатаем только если список не пустой/есть слово */
    if (currentChar == '\n') {
        if (bufPos > 0) {
            if (addWordFromBuf(list) != 0) {
                clearList(list);
                return Stop;
            }
        }
        if (list->size > 0) {
            finalizeList(list);
            printList(list);
            sortList(list);
            printList(list);
            clearList(list);
        }
        nullBuf();
        currentChar = getSym();
        return Start;
    }

    /* простое слово */
    if (isSimpleChar(currentChar)) {
        nullBuf();
        if (addSym(currentChar) != 0) {
            clearList(list);
            return Stop;
        }
        currentChar = getSym();
        return Simple;
    }

    /* спецслова, начинающиеся с |, &, > */
    if (currentChar == '|') {
        nullBuf();
        if (addSym(currentChar) != 0) {
            clearList(list);
            return Stop;
        }
        currentChar = getSym();
        return Pipe;
    }

    if (currentChar == '&') {
        nullBuf();
        if (addSym(currentChar) != 0) {
            clearList(list);
            return Stop;
        }
        currentChar = getSym();
        return Amp;
    }

    if (currentChar == '>') {
        nullBuf();
        if (addSym(currentChar) != 0) {
            clearList(list);
            return Stop;
        }
        currentChar = getSym();
        return Greater;
    }

    /* односимвольные спец: ; < ( ) */
    if (currentChar == ';' || currentChar == '<' ||
        currentChar == '(' || currentChar == ')') {

        nullBuf();
        if (addSym(currentChar) != 0 || addWordFromBuf(list) != 0) {
            clearList(list);
            return Stop;
        }
        currentChar = getSym();
        return Start;
    }

    /* недопустимый символ: молча пропускаем */
    currentChar = getSym();
    return Start;
}

/* Simple: внутри простого слова */
static Vertex simple(StringList *list) {
    if (currentChar == EOF ||
        currentChar == '\n' ||
        currentChar == ' ' || currentChar == '\t' ||
        currentChar == '|' || currentChar == '&' ||
        currentChar == ';' || currentChar == '>' ||
        currentChar == '<' || currentChar == '(' ||
        currentChar == ')') {

        if (addWordFromBuf(list) != 0) {
            clearList(list);
            return Stop;
        }
        return Start;
    }

    if (!isSimpleChar(currentChar)) {
        if (addWordFromBuf(list) != 0) {
            clearList(list);
            return Stop;
        }
        currentChar = getSym();
        return Start;
    }

    if (addSym(currentChar) != 0) {
        clearList(list);
        return Stop;
    }
    currentChar = getSym();
    return Simple;
}

/* Pipe: уже есть '|' → максимум '||' */
static Vertex pipe(StringList *list) {
    if (currentChar == '|') {
        if (addSym(currentChar) != 0) {
            clearList(list);
            return Stop;
        }
        currentChar = getSym();
    }
    if (addWordFromBuf(list) != 0) {
        clearList(list);
        return Stop;
    }
    return Start;
}

/* Amp: уже есть '&' → максимум '&&' */
static Vertex amp(StringList *list) {
    if (currentChar == '&') {
        if (addSym(currentChar) != 0) {
            clearList(list);
            return Stop;
        }
        currentChar = getSym();
    }
    if (addWordFromBuf(list) != 0) {
        clearList(list);
        return Stop;
    }
    return Start;
}

/* Greater: уже есть '>' → максимум '>>' */
static Vertex greater(StringList *list) {
    if (currentChar == '>') {
        if (addSym(currentChar) != 0) {
            clearList(list);
            return Stop;
        }
        currentChar = getSym();
    }
    if (addWordFromBuf(list) != 0) {
        clearList(list);
        return Stop;
    }
    return Start;
}
