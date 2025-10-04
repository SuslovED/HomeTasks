#include <stdio.h>

int main() {
    double eps;
    scanf("%lf", &eps);

    double x;
    while (scanf("%lf", &x) == 1) {
        if (x < 0.0) {
            fprintf(stderr, "Квадратный корень не может быть извлечен из отрицательного числа.\n");
            continue;
        }
        
        if (x == 0.0) {
            printf("%.10g\n", 0.0);
            continue;
        }
        
        double xi = 1.0;
        double xi_plus_1;
        
        while (1) {
            xi_plus_1 = 0.5 * (xi + x / xi);
            double diff = xi_plus_1 - xi;
            if (diff < 0) diff = -diff;
            if (diff < eps)
                break;
            xi = xi_plus_1;
        }
        
        printf("%.10g\n", xi_plus_1);
    }
    
    return 0;
}
