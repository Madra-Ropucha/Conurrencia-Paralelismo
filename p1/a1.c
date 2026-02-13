#define DELAY 250000 // time in microseconds
#define ITERATIONS 50
#define NUM_THREADS 10

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
};

void* Func (void* data) {
    struct SumSub* sumSub = (struct SumSub*)data;

    for (int i = 0; i < sumSub->iterations; i++) {
        *(sumSub->a) = *(sumSub->a) + 1;
        *(sumSub->b) = *(sumSub->b) - 1;
        
        usleep(sumSub->delay);
    }
    return NULL;
}

int main() {
    volatile int* a = malloc(sizeof(volatile int));
    volatile int* b = malloc(sizeof(volatile int));

    *a = 0;
    *b = 0;

    struct SumSub* data1 = malloc(sizeof(struct SumSub));

    data1->a = a;
    data1->b = b;
    data1->delay = DELAY;
    data1->iterations = ITERATIONS;

    pthread_t threadsTypeA[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threadsTypeA[i], NULL, (void* (*)(void*))Func, data1);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threadsTypeA[i], NULL);
    }
    
    free(a);
    free(b);
    free(data1);

    return 0;
}