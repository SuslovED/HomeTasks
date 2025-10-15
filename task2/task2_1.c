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
        
        if (x < 0) x = -x;
        if (x < eps) {
            printf("%.10g\n", 0.0);
            continue;
        }
        
        double xi = 1.0;
        double xi_plus_1;
        double max_xi = xi;
        
        while (1) {
            xi_plus_1 = 0.5 * (xi + x / xi);
            if (xi_plus_1 > max_xi) max_xi = xi_plus_1;
            double diff = xi_plus_1 - xi;
            if (diff < 0) diff = -diff;
            if (diff < eps)
                break;
            xi = xi_plus_1;
        }
        
        printf("%.10g\n", xi_plus_1);
        printf("%.10g\n", max_xi);
    }
    
    return 0;
}
