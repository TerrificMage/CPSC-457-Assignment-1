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
if (argc != 4) {
    fprintf(stderr, "Usage: %s LOWER_BOUND UPPER_BOUND N\n", argv[0]);
    return EXIT_FAILURE;
}
long long L = atoll(argv[1]);
long long U = atoll(argv[2]);
int N = atoi(argv[3]);
