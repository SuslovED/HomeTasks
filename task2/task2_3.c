#include <stdio.h>

/* Итеративная версия */
long long fib_iterative(int n) {
    if (n == 0) return 0;
    if (n == 1) return 1;
    
    long long a = 0, b = 1, c;
    for (int i = 2; i <= n; i++) {
        c = a + b;
        a = b;
        b = c;
    }
    return b;
}

/* Рекурсивная версия */
long long fib_recursive(int n) {
    if (n == 0) return 0;
    if (n == 1) return 1;
    return fib_recursive(n - 2) + fib_recursive(n - 1);
}

int main() {
    int i;
    
    while (scanf("%d", &i) == 1) {
        if (i < 0) {
            fprintf(stderr, "Ошибка: отрицательный индекс %d\n", i);
            continue;
        }
        
        long long iter_result = fib_iterative(i);
        printf("%lld\n", iter_result);
        
        long long rec_result = fib_recursive(i);
        printf("%lld\n", rec_result);
    }
    
    return 0;
}
