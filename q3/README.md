# Q3 Prime Counter (16 threads)

## Run
```sh
gcc -O2 -Wall -Wextra -std=c11 -c q3_primes_mutex.c -o q3_primes_mutex.o
gcc -O2 -Wall -Wextra -std=c11 -c prime_utils.c -o prime_utils.o
gcc -O2 -Wall -Wextra -std=c11 -o q3_primes_mutex q3_primes_mutex.o prime_utils.o -pthread -lm

./primes_mutex
```

Prints the synchronized total and whether it matches `17984` (pi(200000)).