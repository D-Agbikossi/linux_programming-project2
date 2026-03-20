#define _POSIX_C_SOURCE 200809L

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "keyword_counter.h"

typedef struct
{
    const char *keyword;
    size_t keyword_len;
    char **files;
    int num_files;

    int next_index;
    pthread_mutex_t index_mutex;

    FILE *out;
    pthread_mutex_t out_mutex;
} shared_t;

static void die(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

static double elapsed_seconds(struct timespec a, struct timespec b)
{
    double sec = (double)(b.tv_sec - a.tv_sec);
    double nsec = (double)(b.tv_nsec - a.tv_nsec) / 1e9;
    return sec + nsec;
}

static void *worker(void *p)
{
    shared_t *s = (shared_t *)p;

    while (1)
    {
        int idx;
        pthread_mutex_lock(&s->index_mutex);
        idx = s->next_index++;
        pthread_mutex_unlock(&s->index_mutex);

        if (idx >= s->num_files)
            break;

        const char *file_path = s->files[idx];
        size_t count = count_keyword_in_file(file_path, s->keyword, s->keyword_len);

        pthread_mutex_lock(&s->out_mutex);
        fprintf(s->out, "%s %zu\n", file_path, count);
        fflush(s->out);
        pthread_mutex_unlock(&s->out_mutex);
    }

    return NULL;
}

int main(int argc, char **argv)
{
    // Required execution format:
    // ./search keyword output.txt file1.txt file2.txt ... number_of_threads
    if (argc < 5)
    {
        fprintf(stderr, "Usage: %s keyword output.txt file1.txt [file2.txt ...] number_of_threads\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *keyword = argv[1];
    const char *output_path = argv[2];
    int requested_threads = atoi(argv[argc - 1]);
    if (requested_threads <= 0)
    {
        fprintf(stderr, "number_of_threads must be > 0\n");
        return EXIT_FAILURE;
    }

    int num_files = argc - 4;
    char **files = &argv[3];
    if (num_files <= 0)
    {
        fprintf(stderr, "No input files provided.\n");
        return EXIT_FAILURE;
    }

    int num_threads = requested_threads;
    if (num_threads > num_files)
        num_threads = num_files;

    shared_t s;
    memset(&s, 0, sizeof(s));
    s.keyword = keyword;
    s.keyword_len = strlen(keyword);
    s.files = files;
    s.num_files = num_files;
    s.next_index = 0;

    if (pthread_mutex_init(&s.index_mutex, NULL) != 0)
        die("pthread_mutex_init(index_mutex)");
    if (pthread_mutex_init(&s.out_mutex, NULL) != 0)
        die("pthread_mutex_init(out_mutex)");

    s.out = fopen(output_path, "w");
    if (!s.out)
        die("fopen(output)");

    pthread_t *threads = (pthread_t *)malloc((size_t)num_threads * sizeof(pthread_t));
    if (!threads)
        die("malloc(threads)");

    struct timespec t0, t1;
    if (clock_gettime(CLOCK_MONOTONIC, &t0) != 0)
        die("clock_gettime(start)");

    for (int t = 0; t < num_threads; t++)
    {
        if (pthread_create(&threads[t], NULL, worker, &s) != 0)
            die("pthread_create");
    }

    for (int t = 0; t < num_threads; t++)
        (void)pthread_join(threads[t], NULL);

    if (clock_gettime(CLOCK_MONOTONIC, &t1) != 0)
        die("clock_gettime(end)");

    fclose(s.out);
    pthread_mutex_destroy(&s.index_mutex);
    pthread_mutex_destroy(&s.out_mutex);
    free(threads);

    double elapsed = elapsed_seconds(t0, t1);
    printf("Keyword \"%s\" processed %d file(s) with %d thread(s) in %.6f s\n",
        keyword, num_files, num_threads, elapsed);
    printf("Results written to %s\n", output_path);
    return 0;
}

