# Q1 Pipeline (fork + execvp + pipe) with `strace`

This program builds a 2-stage pipeline:

- Child 1 runs `ps aux`
- Child 2 runs `grep root`
- The parent reads the `grep` output from a pipe and writes it to a file, then prints a small preview.

## Build

```sh
cd q1
gcc -O2 -Wall -Wextra -std=c11 pipeline.c -o pipeline
```

## Run

```sh
./pipeline
```

- Creates `output.txt` in the folder when executed.
- Prints the first bytes of the captured pipeline output to stdout.

## `strace` (process creation, pipe/dup2, exec, and file I/O)

Generate a focused trace (this keeps the analysis readable while still capturing the required calls):

```sh
cd q1
rm -f strace_q1.txt output.txt
strace -f -o strace_q1.txt \
  -e trace=pipe,pipe2,clone,fork,vfork,execve,dup2,openat,read,write,close,wait4 \
  ./pipeline
```

### What to look for in `strace_q1.txt`

Use `rg` to quickly filter the trace to the pipeline-relevant calls:

```sh
rg -n "pipe2\\(|clone\\(|dup2\\(|execve\\(|openat\\(AT_FDCWD, \"output\\.txt\"|wait4\\(|read\\(|write\\(|close\\(" strace_q1.txt
```

In the expected system-call sequence you should observe:

1. `pipe2(...)` (two pipes created: `ps -> grep`, and `grep -> parent`)
2. Two `clone(...)` entries (these are the `fork()` children being created)
3. In the `ps` child:
   - `dup2(<ps_pipe_write>, STDOUT_FILENO)` to redirect stdout into the pipe
   - `execve(...)` via `execvp("ps", ...)` (PATH search may attempt multiple `execve`s)
4. In the `grep` child:
   - `dup2(<ps_pipe_read>, STDIN_FILENO)` and `dup2(<grep_pipe_write>, STDOUT_FILENO)`
   - `execve(...)` via `execvp("grep", ...)`
5. In the parent:
   - `openat(..., "output.txt", O_WRONLY|O_CREAT|O_TRUNC, ...)`
   - `read(...)` from the pipe connected to `grep` output
   - `write(...)` to `output.txt`
   - `close(...)` for pipe ends and the output fd
   - `wait4(...)` (waiting for both children to exit)

### Report

- `pipe*`: establishes the byte stream between processes.
- `fork/clone`: creates the child processes that will later `exec`.
- `dup2`: connects the correct pipe endpoints to each child’s stdin/stdout.
- `execvp`/`execve`: replaces the child’s process image with `ps` and `grep`.
- `open/read/write/close`: parent persists the pipeline output into a regular file.
- `wait4`: ensures the parent collects child termination (and prevents zombies).

