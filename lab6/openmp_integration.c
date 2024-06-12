#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <omp.h>

long double f(long double x) {
    if (0 <= x && x <= 1) {
        return cosl(x) * expl(-x*x);
    } else if (1 < x && x <= 2) {
        return logl(x + 1) - sqrtl(4 - x*x);
    } else {
        // В случае, если значение x находится вне области определения функции
        // Выбросим ошибку
        fprintf(stderr, "x is out of the domain of the function\n");
        exit(1);
    }
}

long double trapezoidal_integration(long double (*f)(long double), long double a, long double b, int n) {
    long double h = (b - a) / n;
    long double integral = (f(a) + f(b)) / 2.0;

#pragma omp parallel for reduction(+:integral)
    for (int i = 1; i < n; ++i) {
        integral += f(a + i * h);
    }

    integral *= h;
    return integral;
}

int main() {
    int n = 10000; // Количество подинтервалов

    // Интегрируем по [0, 1] и (1, 2]
    long double integral_part1 = trapezoidal_integration(f, 0.0L, 1.0L, n);
    long double integral_part2 = trapezoidal_integration(f, 1.0L, 2.0L, n);

    // Суммируем результаты
    long double total_integral = integral_part1 + integral_part2;

    printf("The computed value of the integral is: %.20Lf\n", total_integral);

    // Контрольное значение
    long double control_value = 0.33735;
    printf("Control value of the integral is: %.20Lf\n", control_value);
    printf("Difference: %.20Lf\n", fabsl(total_integral - control_value));

    return 0;
}
