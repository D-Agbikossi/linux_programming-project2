# Q2 Large File Copy (syscalls vs stdio)

## Build
```sh
cd q2
make
```

## Run (copy >= 100MB)
```sh
cd q2
gcc -O2 -Wall -Wextra -std=c11 -c copy_syscalls.c -o copy_syscalls.o
gcc -O2 -Wall -Wextra -std=c11 -o copy_syscalls copy_syscalls.o

gcc -O2 -Wall -Wextra -std=c11 -c copy_stdio.c -o copy_stdio.o
gcc -O2 -Wall -Wextra -std=c11 -o copy_stdio copy_stdio.o
```

## Timing + `strace` counts
```sh
strace -c -f -o trace_syscalls.txt ./copy_syscalls bigfile.bin copy_syscalls.bin
strace -c -f -o trace_stdio.txt    ./copy_stdio    bigfile.bin copy_stdio.bin
```

