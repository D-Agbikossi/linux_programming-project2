#include <math.h>

#include "prime_utils.h"

int is_prime(int n)
{
    if (n < 2)
        return 0;
    if (n == 2)
        return 1;
    if (n % 2 == 0)
        return 0;

    int limit = (int)sqrt((double)n);
    for (int d = 3; d <= limit; d += 2)
    {
        if (n % d == 0)
            return 0;
    }
    return 1;
}
