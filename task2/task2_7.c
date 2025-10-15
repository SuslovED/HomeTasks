#include <stdio.h>
#include <setjmp.h>

jmp_buf begin; /* точка начала диалога с пользователем */

char curlex; /* текущая лексема */

void getlex(void); /* выделяет из входного потока очередную лексему */

int expr(void); /* распознает выражение и вычисляет его значение */
int term(void); /* распознает слагаемое и вычисляет его значение */
int factor(void); /* распознает множитель и вычисляет его значение */
int power(void); /* распознает степень и вычисляет ее значение */
int number(void); /* распознает число и вычисляет его значение */

void error(void); /* сообщает об ошибке в выражении и передает управление
в начало функции main (точка begin) */

/* Функция для возведения в степень */
int my_pow(int base, int exponent) {
    int result = 1;
    for (int i = 0; i < exponent; i++) {
        result *= base;
    }
    return result;
}

int main()
{
    int result;
    setjmp(begin);
    printf("==>");
    getlex();
    result = expr();
    if (curlex != '\n') error();
    printf("\n%d\n",result);
    return 0;
}

void getlex()
{
    while ((curlex = getchar()) == ' ');
}

void error(void)
{
    printf("\nОШИБКА!\n");
    while(getchar() != '\n');
    longjmp(begin, 1);
}

/* <выражение> ::= <слагаемое> {(+|-) <слагаемое>} */
int expr()
{
    int e = term();
    while (curlex == '+' || curlex == '-')
    {
        char op = curlex;
        getlex();
        int t = term();
        if (op == '+')
            e += t;
        else
            e -= t;
    }
    return e;
}

/* <слагаемое> ::= <множитель> {(*|/) <множитель>} */
int term()
{
    int a = factor();
    while (curlex == '*' || curlex == '/')
    {
        char op = curlex;
        getlex();
        int f = factor();
        if (op == '*')
            a *= f;
        else {
            if (f == 0) error(); /* деление на ноль */
            a /= f;
        }
    }
    return a;
}

/* <множитель> ::= <степень> */
int factor()
{
    return power();
}

/* <степень> ::= <число> [^ <множитель>] */
int power()
{
    int p = number();
    if (curlex == '^')
    {
        getlex();
        int exponent = factor(); /* правоассоциативность */
        if (exponent < 0) error(); /* отрицательная степень */
        p = my_pow(p, exponent);
    }
    return p;
}

/* <число> ::= 0|1|2|3|4|5|6|7|8|9 | (<выражение>) */
int number()
{
    int m;
    switch(curlex){
        case '0': case '1': case '2': case '3': case '4': case '5':
        case '6': case '7': case '8': case '9': 
            m = curlex - '0';
            getlex();
            break;
        case '(': 
            getlex(); 
            m = expr();
            if (curlex == ')') 
                getlex();
            else 
                error(); /* нет закрывающей скобки */
            break;
        default: 
            error();
    }
    return m;
}
