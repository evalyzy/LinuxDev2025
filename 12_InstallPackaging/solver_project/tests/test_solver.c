#include <assert.h>
#include <math.h>
#include <stdio.h>
#include "../src/solver.h"

#define EPSILON 0.00001

int main() {
    double x1, x2;
    int n;

    // 测试 1: x^2 - 3x + 2 = 0 -> x=1, x=2
    n = solve_quadratic(1.0, -3.0, 2.0, &x1, &x2);
    assert(n == 2);
    // 根的顺序可能不同，检查是否包含 1 和 2
    assert((fabs(x1 - 1.0) < EPSILON && fabs(x2 - 2.0) < EPSILON) ||
           (fabs(x1 - 2.0) < EPSILON && fabs(x2 - 1.0) < EPSILON));

    // 测试 2: x^2 = 0 -> x=0
    n = solve_quadratic(1.0, 0.0, 0.0, &x1, &x2);
    assert(n == 1);
    assert(fabs(x1) < EPSILON);

    // 测试 3: x^2 + 1 = 0 -> 无解
    n = solve_quadratic(1.0, 0.0, 1.0, &x1, &x2);
    assert(n == 0);

    printf("All tests passed!\n");
    return 0;
}
