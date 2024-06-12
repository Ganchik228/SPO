#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>

// Объявление функции, чтобы избежать ошибки
long double f(long double x);

// Структура для передачи параметров в поток
struct ThreadData {
    long double (*func)(long double); // Указатель на функцию, которую нужно интегрировать
    long double a;                    // Нижний предел интегрирования
    long double b;                    // Верхний предел интегрирования
    int n;                            // Количество подинтервалов
    long double result;               // Результат интегрирования
};

// Функция для интегрирования на подмножестве интервала
void *integrate(void *args) {
    struct ThreadData *data = (struct ThreadData *)args;
    long double h = (data->b - data->a) / data->n;
    long double integral = (data->func(data->a) + data->func(data->b)) / 2.0;

    for (int i = 1; i < data->n; ++i) {
        integral += data->func(data->a + i * h);
    }

    integral *= h;
    data->result = integral;
    pthread_exit(NULL);
}

int main() {
    int n = 10000; // Количество подинтервалов

    // Создаем два потока
    pthread_t thread1, thread2;

    // Параметры для первого и второго потоков
    struct ThreadData data1 = {f, 0.0L, 1.0L, n, 0.0L}; // Для интервала [0, 1]
    struct ThreadData data2 = {f, 1.0L, 2.0L, n, 0.0L}; // Для интервала (1, 2]

    // Запускаем потоки
    pthread_create(&thread1, NULL, integrate, (void *)&data1);
    pthread_create(&thread2, NULL, integrate, (void *)&data2);

    // Ждем завершения потоков
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    // Суммируем результаты
    long double total_integral = data1.result + data2.result;

    printf("The computed value of the integral is: %.20Lf\n", total_integral);

    // Контрольное значение
    long double control_value = 0.33735;
    printf("Control value of the integral is: %.20Lf\n", control_value);
    printf("Difference: %.20Lf\n", fabsl(total_integral - control_value));

    return 0;
}

// Определение функции f
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
