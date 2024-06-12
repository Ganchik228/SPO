#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>

#define NUM_THREADS 4

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

// Структура данных для передачи параметров в поток
struct ThreadData {
    int start;
    int end;
};

// Функция, которую будет выполнять каждый поток
void *thread_prime_search(void *arg) {
    struct ThreadData *data = (struct ThreadData *)arg;
    printf("Prime numbers in range [%d, %d] (thread):\n", data->start, data->end);
    for (int i = data->start; i <= data->end; ++i) {
        if (is_prime(i))
            printf("%d ", i);
    }
    printf("\n");
    pthread_exit(NULL);
}

// Многопоточная версия поиска простых чисел
void parallel_prime_search(int start, int end) {
    pthread_t threads[NUM_THREADS];
    struct ThreadData thread_data[NUM_THREADS];

    int range = (end - start + 1) / NUM_THREADS;
    for (int i = 0; i < NUM_THREADS; ++i) {
        thread_data[i].start = start + i * range;
        thread_data[i].end = (i == NUM_THREADS - 1) ? end : start + (i + 1) * range - 1;
        pthread_create(&threads[i], NULL, thread_prime_search, (void *)&thread_data[i]);
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }
}

int main() {
    int start = 1;
    int end = 1000;

    // Запуск многопоточной версии
    parallel_prime_search(start, end);

    return 0;
}
