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
    
    printf("%lf\n", poly);
    printf("%lf\n", deriv);
    
    return 0;
}
