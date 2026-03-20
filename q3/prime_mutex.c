#define _POSIX_C_SOURCE 200809L

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "prime_utils.h"

#define NUM_THREADS 16
#define MIN_N 1
#define MAX_N 200000
#define EXPECTED_PRIMES 17984

typedef struct
{
    int start;
    int end;
} thread_arg_t;

static pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;
static int64_t shared_total = 0;

static void *worker(void *arg)
{
    thread_arg_t *a = (thread_arg_t *)arg;
    int64_t local = 0;

    for (int n = a->start; n <= a->end; n++)
    {
        if (is_prime(n))
            local++;
    }

    pthread_mutex_lock(&counter_mutex);
    shared_total += local;
    pthread_mutex_unlock(&counter_mutex);

    return NULL;
}

int main(void)
{
    const int range_size = (MAX_N - MIN_N + 1);
    if (range_size % NUM_THREADS != 0)
    {
        fprintf(stderr, "Internal error: range not divisible by threads.\n");
        return EXIT_FAILURE;
    }

    const int seg_size = range_size / NUM_THREADS;

    pthread_t threads[NUM_THREADS];
    thread_arg_t args[NUM_THREADS];

    struct timespec t0, t1;
    if (clock_gettime(CLOCK_MONOTONIC, &t0) != 0)
    {
        perror("clock_gettime(start)");
        return EXIT_FAILURE;
    }

    for (int t = 0; t < NUM_THREADS; t++)
    {
        args[t].start = MIN_N + t * seg_size;
        args[t].end = args[t].start + seg_size - 1;

        if (pthread_create(&threads[t], NULL, worker, &args[t]) != 0)
        {
            perror("pthread_create");
            return EXIT_FAILURE;
        }
    }

    for (int t = 0; t < NUM_THREADS; t++)
        (void)pthread_join(threads[t], NULL);

    if (clock_gettime(CLOCK_MONOTONIC, &t1) != 0)
    {
        perror("clock_gettime(end)");
        return EXIT_FAILURE;
    }

    double elapsed = (double)(t1.tv_sec - t0.tv_sec) + (double)(t1.tv_nsec - t0.tv_nsec) / 1e9;

    printf("Primes between %d and %d using %d threads = %lld\n",
        MIN_N, MAX_N, NUM_THREADS, (long long)shared_total);
    printf("Expected primes pi(%d) = %d -> %s\n",
        MAX_N, EXPECTED_PRIMES,
        (shared_total == EXPECTED_PRIMES) ? "MATCH" : "MISMATCH");

    printf("Elapsed time = %.6f s\n", elapsed);

    // Print the synchronized total as the final line for easy grading.
    printf("%lld\n", (long long)shared_total);

    return 0;
}
