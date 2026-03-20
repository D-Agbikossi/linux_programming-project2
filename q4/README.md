# Q4 Keyword Search (Threads)

## Run
```sh
gcc -O2 -Wall -Wextra -std=c11 -c q4_search.c -o q4_search.o
gcc -O2 -Wall -Wextra -std=c11 -c keyword_counter.c -o keyword_counter.o
gcc -O2 -Wall -Wextra -std=c11 -o search q4_search.o keyword_counter.o -pthread

./search keyword output.txt file1.txt file2.txt ... number_of_threads
```

`output.txt` is overwritten each run. Each line in `output.txt` contains:
`filename occurrence_count`

## Timing + performance testing
Run with different thread counts and compare runtime:
```sh
./search keyword output.txt file1.txt file2.txt ... 2
./search keyword output.txt file1.txt file2.txt ... <num_cores>
./search keyword output.txt file1.txt file2.txt ... <max_threads>
```