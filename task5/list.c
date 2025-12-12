#include "list.h"
#define SIZE 16
static int c;           /*текущий символ*/
static char * buf;      /*буфер для накопления текущего слова*/
static int sizebuf;     /*размер буфера текущего слова*/
static int sizelist;    /*размер списка слов*/
static int curbuf;      /*индекс текущего символа в буфере*/
static int curlist;     /*индекс текущего слова в списке*/

static void nulllist(List *lst) {
    sizelist = 0;
    curlist = 0;
    *lst = NULL;
}

static void termlist(List *lst) {
    List t = *lst;
    if (t == NULL)
        return;
    if (curlist > (sizelist - 1))
        t = (List) realloc(t, (sizelist + 1)*sizeof(*t));   
    t[curlist] = NULL;
    t = (List) realloc(t, (sizelist = curlist + 1)*sizeof(*t));
    *lst = t;
}

void clearlist(List *lst) {
    List t = *lst;
    int i;
    sizelist = 0;
    curlist = 0;
    if (t == NULL) 
        return;
    for (i = 0; t[i] != NULL; i++)
        free(t[i]);
    free(t);
    *lst = NULL;
}

void printlist(List lst, int sizelist) {
    int i;
    if (lst == NULL) 
        return;
    for (i = 0; i < (sizelist - 1); i++)
        printf("%s ", lst[i]);
    printf("\n");
}

static void nullbuf() {
    buf = NULL;
    sizebuf = 0;
    curbuf = 0;
}

static void addword(List *lst) {
    List tlst = *lst;
    if (curbuf > (sizebuf - 1)) {
        char * t = realloc(buf, sizebuf += SIZE);
        if (t != NULL)
            buf = t;
    }
    buf[curbuf++] = '\0';
    buf = realloc(buf, sizebuf = curbuf);
    if (curlist > (sizelist - 1))
        tlst = realloc(tlst, (sizelist += SIZE)*sizeof(*tlst));
    tlst[curlist++] = buf;
    *lst = tlst;
}

static void addsym() {
    if (curbuf > (sizebuf - 1)) {
        char *t = realloc(buf, sizebuf += SIZE);
        if (t != NULL)
            buf = t;
    }
    buf[curbuf++] = c;
}

static char str[10];
static int ind = 0;

static void getsym() {
    if (str[ind] == '\0') {
        ind = 0;
        str[0] = '\n';
        fscanf(stdin, "%9[^\n]", str);
        c = getchar();
        if (c != EOF) {
            int i;
            for (i = 0; str[i] != '\0'; i++);
            str[i] = c;
            str[i+1] = '\0';
        } else
            return;
    }
    c = str[ind++];
}

int symset(int c) {
    return c != '\n' &&
           c != '\t' &&
           c != ' ' &&
           c != '>' &&
           c != '<' &&
           c != '&' &&
           c != '|' &&
           c != '(' &&
           c != ')' &&
           c != ';' &&
           c != EOF;
}

void changelist(List lst, int size) { /* меняем на реальные данные */
    if (lst == NULL) 
        return;
    for (int i = 0; i < (size - 1); i++) {
        if (!strcmp(lst[i], "$USER\0")) {
            struct passwd *temp;
            temp = getpwuid(geteuid());
            lst[i] = realloc(lst[i], strlen(temp->pw_name) + 1);
            strcpy(lst[i], temp -> pw_name);
        } else if (!strcmp(lst[i], "$EUID\0")) {  
            int uid = (int) geteuid(), len, temp;
            temp = uid;
            for (len = 0; temp > 9; len++) 
                temp = temp / 10;
            lst[i] = realloc(lst[i], len + 2);
            sprintf(lst[i], "%d", uid);
        } else if (!strcmp(lst[i], "$SHELL\0")) {
            int len = strlen(getenv("PWD"));
            lst[i] = realloc(lst[i], len + 1);
            strcpy(lst[i], getenv("PWD"));
        } else if (!strcmp(lst[i], "$HOME\0")) {
            int len = strlen(getenv("HOME"));
            lst[i] = realloc(lst[i], len + 1);
            strcpy(lst[i], getenv("HOME"));
        }
    }           
    return;
}

int buildlist(List *lst, int* size, int* isthere) {
    typedef enum {Start, Comment, Quot, Word, Greater, Greater2,
        Smaller, Vert, Vert2, Amp, Amp2, Sem, Par, Stop} vertex;
    vertex V = Start;
    for (int i = 0; i < 10; str[i] = '\0', i++);
    getsym();
    nulllist(lst);
    nullbuf();
    while (1) 
        switch(V) {
            case Start:
                if (c == ' ' || c == '\t') 
                    getsym();
                else if (c == '\n') {
                    termlist(lst);
                    V = Stop;
                } else if (c == '\"') {
                    V = Quot;
                    getsym();
                } else if (c == '#') {
                    V = Comment;
                    getsym();
                } else if (c == EOF) {
                    termlist(lst);
                    V = Stop;
                } else {  
                    nullbuf();
                    addsym();
                    switch(c) {
                        case '(': 
                        case ')':
                            V = Par;
                            break;
                        case '>':
                            V = Greater;
                            break;
                        case '<':
                            V = Smaller;
                            break;
                        case '|':
                            V = Vert;
                            break;
                        case '&':
                            V = Amp;
                            break;
                        case ';':
                            V = Sem;
                            break;
                        default:
                            V = Word;
                            break;
                    }
                    getsym();
                }
                break;
            case Comment:
                if (c == '#')
                    V = Start;
                getsym();
                break;
            case Quot:
                nullbuf();
                while (c != '\"' && c != EOF && c != '\n') {
                    addsym();
                    getsym();
                }
                V = Start;
                addword(lst);
                getsym();
                break;
            case Word:
                if (symset(c)) {
                    addsym();
                    getsym();
                } else {
                    V = Start;
                    addword(lst);
                }
                break;
            case Greater:
                if (c == '>') {
                    addsym();
                    getsym();
                    V = Greater2;
                } else {
                    V = Start;
                    addword(lst);
                }
                break;
            case Greater2:
                V = Start;
                addword(lst);
                break;
            case Smaller:
                V = Start;
                addword(lst);
                break;
            case Vert:
                if (c == '|') {
                    addsym();
                    getsym();
                    V = Vert2;
                } else {
                    V = Start;
                    addword(lst);
                }
                break;
            case Vert2:
                V = Start;
                addword(lst);
                break;
            case Amp:
                if (c == '&') {
                    addsym();
                    getsym();
                    V = Amp2;
                } else {
                    V = Start;
                    addword(lst);
                }
                break;
            case Amp2:
                V = Start;
                addword(lst);
                break;
            case Sem:
                V = Start;
                addword(lst);
                break;
            case Par:
                V = Stop;
                *isthere = 1;
                break;
            case Stop:
                *size = sizelist;
                return (c == EOF) ? 1 : 0;
                break;
        }
}