#ifndef SOLVER_H
#define SOLVER_H

/* 求解 ax^2 + bx + c = 0 */
/* 返回值: 根的数量 (-1 表示无数解) */
int solve_quadratic(double a, double b, double c, double *x1, double *x2);

#endif
