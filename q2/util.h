#ifndef UTIL_H
#define UTIL_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static inline void die(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

static inline double elapsed_seconds(struct timespec a, struct timespec b)
{
    double sec = (double)(b.tv_sec - a.tv_sec);
    double nsec = (double)(b.tv_nsec - a.tv_nsec) / 1e9;
    return sec + nsec;
}

#endif