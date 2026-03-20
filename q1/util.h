#ifndef UTIL_H
#define UTIL_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

static inline void die(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

#endif

