# Q3 Prime Counter (16 threads) with `pthread_mutex_t`

This program counts prime numbers between `1` and `200000` using exactly `16` POSIX threads.

- Workload is divided into 16 equal contiguous segments.
- Each thread counts primes in its segment locally.
- A `pthread_mutex_t` protects a shared counter (`shared_total`) to avoid data races.

## Build

```sh
cd q3
gcc -O2 -Wall -Wextra -std=c11 prime_mutex.c prime_utils.c -o primes_mutex -pthread -lm
```

## Run

```sh
./primes_mutex
```

Expected output:

- The synchronized total between `1` and `200000` is `17984` (pi(200000)).
- The program prints the total again as the last line for easy grading.