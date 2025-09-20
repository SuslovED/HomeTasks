#include <stdio.h>

void test_modulus(int a, int b) 
{
    int r = a % b;
    int q = a / b;
    
    printf("%d %% %d = %d\n", a, b, r);
    printf("Проверка: (%d / %d) * %d + %d %% %d = %d\n", 
           a, b, b, a, b, q * b + r);
}

int main() 
{
    printf("Исследование операции %% с отрицательными операндами:\n");
    printf("=====================================================\n\n");

    // Положительные числа
    printf("1. Положительные числа:\n");
    test_modulus(10, 3);
    printf("\n");

    // Отрицательное делимое
    printf("2. Отрицательное делимое:\n");
    test_modulus(-10, 3);
    test_modulus(-10, 4);
    printf("\n");

    // Отрицательный делитель
    printf("3. Отрицательный делитель:\n");
    test_modulus(10, -3);
    test_modulus(10, -4);
    printf("\n");

    // Оба операнда отрицательные
    printf("4. Оба операнда отрицательные:\n");
    test_modulus(-10, -3);
    test_modulus(-10, -4);
    printf("\n");

    // Крайние случаи
    printf("5. Крайние случаи:\n");
    test_modulus(5, 2);
    test_modulus(-5, 2);

    return 0;
}
