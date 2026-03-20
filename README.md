# linux_programming-project2 (Performance & Concurrency on Linux)

This project contains four C programs focused on Linux process control, system-call tracing, I/O performance, and POSIX threading/synchronization.

## Prerequisites

- `gcc` (C11), `pthread` development support
- `strace` (for Q1 and Q2 system-call tracing)
- `time`/`/usr/bin/time` (for runtime measurement)

## Project layout

- `q1/` : `fork()` + `pipe()` + `execvp()` pipeline, with `strace` analysis
- `q2/` : large file copy (>= 100MB) using syscalls vs stdio, with `strace -c` comparison
- `q3/` : prime counting with 16 threads and `pthread_mutex_t`
- `q4/` : multithreaded keyword search across many files with safe shared output writes

## How to run everything

1. `Q1`:
   - Build: `cd q1 && gcc -O2 -Wall -Wextra -std=c11 pipeline.c -o pipeline`
   - Run: `./pipeline` (creates `q1/output.txt`)

2. `Q2`:
   - Create test data (>= 100MB): `cd q2 && dd if=/dev/zero of=bigfile.bin bs=1M count=120`
   - Build: `cd q2 && gcc -O2 -Wall -Wextra -std=c11 copy_syscalls.c -o copy_syscalls && gcc -O2 -Wall -Wextra -std=c11 copy_stdio.c -o copy_stdio`
   - Run: `./copy_syscalls bigfile.bin bigfile_syscalls.bin` and `./copy_stdio bigfile.bin bigfile_stdio.bin`

3. `Q3`:
   - Build: `cd q3 && gcc -O2 -Wall -Wextra -std=c11 prime_mutex.c prime_utils.c -o primes_mutex -pthread -lm`
   - Run: `./primes_mutex`

4. `Q4`:
   - Build: `cd q4 && gcc -O2 -Wall -Wextra -std=c11 -pthread search.c keyword_counter.c -o search`
   - Run: `./search keyword output.txt file1.txt file2.txt ... number_of_threads`
