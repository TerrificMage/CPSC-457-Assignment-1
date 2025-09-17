#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

static int is_prime(long long x) {
    if (x < 2) return 0;
    if (x % 2 == 0) return x == 2;
    for (long long d = 3; d * d <= x; d += 2) {
        if (x % d == 0) return 0;
    }
    return 1;
}
