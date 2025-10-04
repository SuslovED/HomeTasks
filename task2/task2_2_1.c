#include <stdio.h>
int main() {
    double x, a;
    if (scanf("%lf", &x) != 1 )
        return 1;

    double res = 0.0;
    while (scanf("%lf", &a) == 1)
        res = res * x + a;

    printf("%lf\n", res);
    return 0;
}
