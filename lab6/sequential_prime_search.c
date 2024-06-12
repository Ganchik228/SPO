#include <stdio.h>
#include <stdbool.h>
#include <math.h>

// Функция для проверки, является ли число простым
bool is_prime(int n) {
    if (n <= 1)
        return false;
    for (int i = 2; i * i <= n; ++i) {
        if (n % i == 0)
            return false;
    }
    return true;
}

// Последовательная версия поиска простых чисел
void sequential_prime_search(int start, int end) {
    printf("Prime numbers in range [%d, %d] (sequential):\n", start, end);
    for (int i = start; i <= end; ++i) {
        if (is_prime(i))
            printf("%d ", i);
    }
    printf("\n");
}

int main() {
    int start = 1;
    int end = 1000;

    // Запуск последовательной версии
    sequential_prime_search(start, end);

    return 0;
}
