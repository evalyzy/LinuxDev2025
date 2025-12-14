#include <math.h>
#include "solver.h"

int solve_quadratic(double a, double b, double c, double *x1, double *x2) {
    if (a == 0.0) {
        if (b == 0.0) return (c == 0.0) ? -1 : 0; // 线性退化
        *x1 = -c / b;
        return 1;
    }
    
    double d = b * b - 4 * a * c;
    if (d < 0) return 0;
    
    if (d == 0) {
        *x1 = -b / (2 * a);
        return 1;
    }
    
    *x1 = (-b + sqrt(d)) / (2 * a);
    *x2 = (-b - sqrt(d)) / (2 * a);
    return 2;
}
