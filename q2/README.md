# Q2 Large File Copy (syscalls vs stdio) with `strace -c`

This question compares two implementations of a large file copy utility:

- `copy_syscalls.c` : uses low-level `read()`/`write()` syscalls
- `copy_stdio.c`   : uses standard I/O `fread()`/`fwrite()` buffering

## Build

```sh
cd q2
gcc -O2 -Wall -Wextra -std=c11 copy_syscalls.c -o copy_syscalls
gcc -O2 -Wall -Wextra -std=c11 copy_stdio.c   -o copy_stdio
```

## Create a >= 100MB test file

This project uses 120 MiB for reproducible measurements:

```sh
cd q2
dd if=/dev/zero of=bigfile.bin bs=1M count=120 status=progress
```

## Run (copy >= 100MB)

```sh
./copy_syscalls bigfile.bin bigfile_syscalls.bin
./copy_stdio   bigfile.bin bigfile_stdio.bin
```

Each program prints its elapsed time and how many bytes it copied.

## System-call counting with `strace -c`

```sh
cd q2
strace -c -f -o trace_syscalls.txt ./copy_syscalls bigfile.bin bigfile_syscalls.bin
strace -c -f -o trace_stdio.txt   ./copy_stdio   bigfile.bin bigfile_stdio.bin
```

Interpretation of result:

- The syscall version uses a small user buffer (8 KiB), which increases the number of `read()`/`write()` calls.
- The stdio version uses a larger buffer (1 MiB), so it typically reduces the number of kernel transitions.
- In this run, the syscall version was still faster overall, indicating that kernel-call overhead is not the only factor; user-space buffering strategy, call structure, and flush/EOF behavior also affect performance.

## Optional: verify copied outputs

To sanity-check correctness quickly:

```sh
stat -c "%n %s bytes" bigfile.bin bigfile_syscalls.bin bigfile_stdio.bin
```

