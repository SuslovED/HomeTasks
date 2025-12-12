## Реализация shell
### Суслов Егор
В проекте реализованы: конвейер, перенаправление ввода-вывода, запуск в фоновом режиме,
последовательное выполнение команд, &&, ||, cd, pwd, exit.
Проект состоит из:

1. list.c - отвечает за создание списка слов из данной строки
В нем реализованы следующие функции:
static void nulllist(List *lst); создает пустой список слов
static void termlist(List *lst); выделяет память под список слов
void clearlist(List *lst); освобождает память из под списка слов
void printlist(List lst, int sizelist); печатает список слов
static void nullbuf(); созадает пустой буфер
static void addword(List *lst); добавляет слово в список слов
static void addsym(); добавляет симлов в слово
static void getsym(); выдает следующий символ
int symset(int c); проверяет, это обычный или специальный символ
void changelist(List lst, int size); подставляет вместо переменных соответствующее им значение
int buildlist(List *lst, int* size, int* isthere); строит список

2. list.h - содержит необходимые для подключения в list.c библиотек, переопрелеление типа char **
и заголовки функций, которые используются в других программах

3. tree.c - отвечает за создание дерева по данному списку слов
В нем реализованы следующие функции:
void error(char *str); реализует вывод ошибок в stderr
static void clear_argv(char **argv); освобождает память из под аргументов
static void add_arg(List lst, tree *cur_tree); добавляет в дерево количество аргументов и сами аргументы
void clear_tree(tree **root); освобождает память из под дерева
static tree *null_tree(); создает пустое дерево
static void make_bgrnd(tree *t); устанавливает флаг, отвечающий за фоновый режим
static tree *redir_file(List plex); перенаправляет ввод-вывод
static char **conv_or_not(List plex); проверяет конвейер это или нет
static tree *conveyor(tree *cur_tree, List plex); обрабатывает конвейер
static tree *list_of_commmand(tree* cur_tree, List plex); заполняет tnext, backgrnd - поля в дереве 
tree *create_tree(List lst); создает текущее дерево
static void make_tab(int n); отвечает за табуляцию
static void print_argv(char **p, int shift); печатает аргументы
void print_tree(tree *t, int shift); печатает дерево

4. tree.h - содержит необходимые для подключения в tree.c библиотек, опрелеление типа tree
и заголовки функций, которые используются в других программах

5. exec.c - отвечает за реализацию работы заданной строки
В нем реализованы следующие функции:
static int cd(int argc, char **argv); отвечает за выполнение команды cd
static void files_in_out(tree *root); отвечает за перенаправление ввода-вывода
static int pwd(tree *root); отвечает за выполнение команды pwd
static void cmd_processing(tree *cur_tree, int backgrnd); отвечает за работу в фоновом и обычном режиме аргумента
static int pipe_processing(tree *root); отвечает за выполнение конвейера
void handler(tree *cur_tree); отвечает за обработку конвейера

6. exex.h - содержит необходимые для подключения в exec.c библиотек, и заголовок функции, которая
используется в других программах

7. main.c - отвечает за вывод текущего пути, затем запускает создание списка, потом проверяет на конец файла и
пустую строку, далее запускает создание дерева и его печать, после запускает реализацию

__Примеры работы shell:__
(base) egorsuslov@MacBook-Air-Egor-3 task5 % ./shell

/Users/egorsuslov/Desktop/3sem/task5$ pwd > current_dir.txt
number of arguments = 1
argument 0 = pwd
input from a file = no
output to a file = current_dir.txt
output to the begin of the file
working in normal mode
type = 0
conveyor = no
next = end

/Users/egorsuslov/Desktop/3sem/task5$ cd ..
number of arguments = 2
argument 0 = cd
argument 1 = ..
input from a file = no
output to a file = no
output to the begin of the file
working in normal mode
type = 0
conveyor = no
next = end

/Users/egorsuslov/Desktop/3sem$ pwd
number of arguments = 1
argument 0 = pwd
input from a file = no
output to a file = no
output to the begin of the file
working in normal mode
type = 0
conveyor = no
next = end
/Users/egorsuslov/Desktop/3sem

/Users/egorsuslov/Desktop/3sem$ echo homedir: $HOME username: $USER
number of arguments = 5
argument 0 = echo
argument 1 = homedir:
argument 2 = /Users/egorsuslov
argument 3 = username:
argument 4 = egorsuslov
input from a file = no
output to a file = no
output to the begin of the file
working in normal mode
type = 0
conveyor = no
next = end
homedir: /Users/egorsuslov username: egorsuslov

/Users/egorsuslov/Desktop/3sem$ yes | head
number of arguments = 1
argument 0 = yes
input from a file = no
output to a file = no
output to the begin of the file
working in normal mode
type = 0
conveyor:
     number of arguments = 1
     argument 0 = head
     input from a file = no
     output to a file = no
     output to the begin of the file
     working in normal mode
     type = 0
     conveyor = no
     next = end
next = end
y
y
y
y
y
y
y
y
y
y

/Users/egorsuslov/Desktop/3sem$ cd
number of arguments = 1
argument 0 = cd
input from a file = no
output to a file = no
output to the begin of the file
working in normal mode
type = 0
conveyor = no
next = end

Ошибка смены директории!

/Users/egorsuslov/Desktop/3sem$ ^C
Пришел сигнал SIGINT

/Users/egorsuslov/Desktop/3sem$ exit
number of arguments = 1
argument 0 = exit
input from a file = no
output to a file = no
output to the begin of the file
working in normal mode
type = 0
conveyor = no
next = end