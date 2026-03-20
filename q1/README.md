# Q1 Pipeline

## Run
```sh
cd q1
gcc -O2 -Wall -Wextra -std=c11
./pipeline
```

Creates `q1_output.txt` (in the `q1/` folder) and prints a short preview.

## `strace` (capture pipeline + file ops)
```sh
cd q1
strace -f -o strace_q1.txt ./pipeline
```

