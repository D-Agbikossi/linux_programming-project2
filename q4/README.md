# Q4 Keyword Search (Threads) with synchronization

This program searches for a keyword across multiple text files using POSIX threads.

- Each thread repeatedly grabs one file index from a shared counter (protected by an `index_mutex`).
- The keyword counter for each file uses `fopen`/`fread` internally (in `keyword_counter.c`).
- Updates to the shared output file are protected by a second mutex (`out_mutex`) to prevent interleaving.

## Build

```sh
cd q4
gcc -O2 -Wall -Wextra -std=c11 -pthread search.c keyword_counter.c -o search
```

## Execution format

```sh
./search keyword output.txt file1.txt file2.txt ... number_of_threads
```

`output.txt` is overwritten each run. Each output line contains:

`filename occurrence_count`

## Generate a small multi-file dataset (for testing/benchmarking)

```sh
cd q4
rm -rf data && mkdir -p data
for i in 1 2 3 4 5 6 7 8; do yes 'root root keyword tail' | head -c 2000000 > "data/file${i}.txt"; done
```

## Benchmark required configurations

Let `N_CORES=$(nproc)` and `N_FILES` be the number of input files (here: 8), then run:

```sh
cd q4
N_CORES=$(nproc)
N_FILES=$(ls data/file*.txt | wc -l)

./search root output_2.txt      data/file*.txt 2
./search root output_cores.txt  data/file*.txt "$N_CORES"
./search root output_max.txt    data/file*.txt "$N_FILES"
```
