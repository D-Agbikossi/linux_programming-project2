#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "util.h"

/*
 *   Create two child processes with fork()
 *   Connect them with pipe()
 *   Use execvp() to run a pipeline: ps aux | grep root
 *   Parent captures the pipeline output into a file and prints a preview.
 */

#define OUTPUT_FILE "output.txt"
#define PREVIEW_BYTES 2048

static void write_all(int fd, const char *buf, size_t n)
{
    size_t off = 0;
    while (off < n)
    {
        ssize_t w = write(fd, buf + off, n - off);
        if (w < 0)
            die("write");
        off += (size_t)w;
    }
}

int main(void)
{
    int ps_to_grep[2];
    int grep_to_parent[2];

    if (pipe(ps_to_grep) < 0)
        die("pipe(ps_to_grep)");
    if (pipe(grep_to_parent) < 0)
        die("pipe(grep_to_parent)");

    pid_t ps_pid = fork();
    if (ps_pid < 0)
        die("fork(ps)");

    if (ps_pid == 0)
    {
        // ps child: stdout -> ps_to_grep[1]
        if (dup2(ps_to_grep[1], STDOUT_FILENO) < 0)
            die("dup2(ps_to_grep[1])");

        close(ps_to_grep[0]);
        close(ps_to_grep[1]);
        close(grep_to_parent[0]);
        close(grep_to_parent[1]);

        char *const cmd_argv[] = {"ps", "aux", NULL};
        execvp(cmd_argv[0], cmd_argv);
        die("execvp(ps)");
    }

    pid_t grep_pid = fork();
    if (grep_pid < 0)
        die("fork(grep)");

    if (grep_pid == 0)
    {
        // grep child:
        // stdin  <- ps_to_grep[0]
        // stdout -> grep_to_parent[1]
        if (dup2(ps_to_grep[0], STDIN_FILENO) < 0)
            die("dup2(ps_to_grep[0])");
        if (dup2(grep_to_parent[1], STDOUT_FILENO) < 0)
            die("dup2(grep_to_parent[1])");

        close(ps_to_grep[0]);
        close(ps_to_grep[1]);
        close(grep_to_parent[0]);
        close(grep_to_parent[1]);

        char *const cmd_argv[] = {"grep", "root", NULL};
        execvp(cmd_argv[0], cmd_argv);
        die("execvp(grep)");
    }

    // Parent: close unused ends.
    close(ps_to_grep[0]);
    close(ps_to_grep[1]);
    close(grep_to_parent[1]);

    int out_fd = open(OUTPUT_FILE, O_CREAT | O_TRUNC | O_WRONLY, 0644);
    if (out_fd < 0)
        die("open(OUTPUT_FILE)");

    // Capture grep output into output file.
    char buf[4096];
    ssize_t r;
    while ((r = read(grep_to_parent[0], buf, sizeof(buf))) > 0)
        write_all(out_fd, buf, (size_t)r);

    if (r < 0)
        die("read(grep_to_parent)");

    close(out_fd);
    close(grep_to_parent[0]);

    // Wait for both children.
    int status = 0;
    (void)waitpid(ps_pid, &status, 0);
    (void)waitpid(grep_pid, &status, 0);

    // Read and display part of the output.
    out_fd = open(OUTPUT_FILE, O_RDONLY);
    if (out_fd < 0)
        die("open(OUTPUT_FILE for read)");

    char preview[PREVIEW_BYTES + 1];
    ssize_t pr = read(out_fd, preview, PREVIEW_BYTES);
    if (pr < 0)
        die("read(preview)");
    preview[pr] = '\0';

    close(out_fd);

    printf("Pipeline output -> %s\n", OUTPUT_FILE);
    printf("Preview (first %d bytes):\n", PREVIEW_BYTES);
    printf("%s\n", preview);

    return 0;
}

