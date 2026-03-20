#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "util.h"

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <src> <dst>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *src_path = argv[1];
    const char *dst_path = argv[2];

    struct stat st;
    if (stat(src_path, &st) != 0)
        die("stat(src)");

    long long total_size = (long long)st.st_size;

    FILE *src = fopen(src_path, "rb");
    if (!src)
        die("fopen(src)");

    FILE *dst = fopen(dst_path, "wb");
    if (!dst)
        die("fopen(dst)");

    const size_t BUFSZ = 1 << 20; // 1 MiB
    char *buf = (char *)malloc(BUFSZ);
    if (!buf)
        die("malloc(buf)");

    struct timespec t0, t1;
    if (clock_gettime(CLOCK_MONOTONIC, &t0) != 0)
        die("clock_gettime(start)");

    size_t copied = 0;
    while (1)
    {
        size_t nread = fread(buf, 1, BUFSZ, src);
        if (nread > 0)
        {
            size_t nwritten = fwrite(buf, 1, nread, dst);
            if (nwritten != nread)
            {
                fprintf(stderr, "fwrite failed: wrote %zu/%zu bytes\n", nwritten, nread);
                exit(EXIT_FAILURE);
            }
            copied += nread;
        }

        if (nread < BUFSZ)
        {
            if (ferror(src))
                die("fread(src)");
            break; // EOF
        }
    }

    if (fflush(dst) != 0)
        die("fflush(dst)");

    if (fclose(dst) != 0)
        die("fclose(dst)");
    if (fclose(src) != 0)
        die("fclose(src)");

    if (clock_gettime(CLOCK_MONOTONIC, &t1) != 0)
        die("clock_gettime(end)");

    double elapsed = elapsed_seconds(t0, t1);
    printf("(stdio): copied %zu bytes (src size %lld) in %.6f s\n",
        copied, total_size, elapsed);

    free(buf);
    return 0;
}

