#include <stdio.h>

int main() {
    double x, a;
    
    if (scanf("%lf", &x) != 1) {
        return 1;
    }
    
    double poly = 0.0, deriv = 0.0;
    
    while (scanf("%lf", &a) == 1) {
        deriv = deriv * x + poly;
        poly = poly * x + a;
    }
    
    printf("%.10g\n", poly);
    printf("%.10g\n", deriv);
    
    return 0;
}
