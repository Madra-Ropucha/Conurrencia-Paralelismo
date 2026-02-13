#define DELAY 250 // time in microseconds
#define ITERATIONS 100
#define NUM_THREADS 10

#define ORDENV1 1
#define ORDENV2 2

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

struct SumSub {
    volatile int* a;
    volatile int* b;
    int delay;
    int iterations;
    pthread_mutex_t* mutex1;
    pthread_mutex_t* mutex2;
    int orden1;
    int orden2;
};

void* Func (void* data) {
    struct SumSub* sumSub = (struct SumSub*)data;

    for (int i = 0; i < sumSub->iterations; i++) {
        if (sumSub->orden1 < sumSub->orden2) {
            pthread_mutex_lock(sumSub->mutex1);
            pthread_mutex_lock(sumSub->mutex2);
        } else {
            pthread_mutex_lock(sumSub->mutex2);
            pthread_mutex_lock(sumSub->mutex1);
        }

        *(sumSub->a) = *(sumSub->a) + 1;
        usleep(sumSub->delay);
        *(sumSub->b) = *(sumSub->b) - 1;

        pthread_mutex_unlock(sumSub->mutex2);
        pthread_mutex_unlock(sumSub->mutex1);
    }
    return NULL;
}

void* PrintSumSub (void* data) {
    struct SumSub* sumSub = (struct SumSub*)data;
    while (1) {
        if (sumSub->orden1 < sumSub->orden2) {
            pthread_mutex_lock(sumSub->mutex1);
            pthread_mutex_lock(sumSub->mutex2);
        } else {
            pthread_mutex_lock(sumSub->mutex2);
            pthread_mutex_lock(sumSub->mutex1);
        }
        
        printf("a: %d, b: %d, sum: %d\n", *(sumSub->a), *(sumSub->b), *(sumSub->a) + *(sumSub->b));

        pthread_mutex_unlock(sumSub->mutex2);
        pthread_mutex_unlock(sumSub->mutex1);
        usleep(sumSub->delay);
    }
    return NULL;
}

int main() {
    int* a = malloc(sizeof(volatile int));
    int* b = malloc(sizeof(volatile int));
    pthread_mutex_t* mutex1 = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_t* mutex2 = malloc(sizeof(pthread_mutex_t));

    *a = 0;
    *b = 0;

    struct SumSub* data1 = malloc(sizeof(struct SumSub));
    struct SumSub data2 = {b, a, DELAY, ITERATIONS, mutex2, mutex1, ORDENV2, ORDENV1};

    data1->a = a;
    data1->b = b;
    data1->delay = DELAY;
    data1->iterations = ITERATIONS;
    data1->mutex1 = mutex1;
    data1->mutex2 = mutex2;
    data1->orden1 = ORDENV1;
    data1->orden2 = ORDENV2;
    
    pthread_t printThread;
    pthread_t threadsTypeA[NUM_THREADS];
    pthread_t threadsTypeB[NUM_THREADS];

    pthread_mutex_init(mutex1, NULL);
    pthread_mutex_init(mutex2, NULL);
    
    pthread_create(&printThread, NULL, (void* (*)(void*))PrintSumSub, data1);

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threadsTypeA[i], NULL, (void* (*)(void*))Func, data1);
        pthread_create(&threadsTypeB[i], NULL, (void* (*)(void*))Func, &data2);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threadsTypeA[i], NULL);
        pthread_join(threadsTypeB[i], NULL);
    }

    pthread_mutex_destroy(mutex1);
    pthread_mutex_destroy(mutex2);

    pthread_cancel(printThread);
    
    free(a);
    free(b);
    free(data1);
    free(mutex1);
    free(mutex2);

    return 0;
}