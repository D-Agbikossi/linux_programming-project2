#define _POSIX_C_SOURCE 200809L

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

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

    int src_fd = open(src_path, O_RDONLY);
    if (src_fd < 0)
        die("open(src)");

    struct stat st;
    if (fstat(src_fd, &st) != 0)
        die("fstat(src)");

    off_t total_size = st.st_size;

    int dst_fd = open(dst_path, O_CREAT | O_TRUNC | O_WRONLY, 0644);
    if (dst_fd < 0)
        die("open(dst)");

    const size_t BUFSZ = 8 * 1024; // syscall copy uses smaller buffer
    char *buf = (char *)malloc(BUFSZ);
    if (!buf)
        die("malloc");

    struct timespec t0, t1;
    if (clock_gettime(CLOCK_MONOTONIC, &t0) != 0)
        die("clock_gettime(start)");

    ssize_t r;
    off_t copied = 0;
    while ((r = read(src_fd, buf, BUFSZ)) > 0)
    {
        ssize_t off = 0;
        while (off < r)
        {
            ssize_t w = write(dst_fd, buf + off, (size_t)(r - off));
            if (w < 0)
                die("write(dst)");
            off += w;
        }
        copied += r;
    }
    if (r < 0)
        die("read(src)");

    if (clock_gettime(CLOCK_MONOTONIC, &t1) != 0)
        die("clock_gettime(end)");

    close(src_fd);
    close(dst_fd);
    free(buf);

    double elapsed = elapsed_seconds(t0, t1);
    printf("(syscalls): copied %lld bytes (src size %lld) in %.6f s\n",
        (long long)copied, (long long)total_size, elapsed);
    return 0;
}

