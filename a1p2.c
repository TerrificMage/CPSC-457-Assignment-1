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
size_t shm_bytes = total_ints * sizeof(int);
int shmid = shmget(IPC_PRIVATE, shm_bytes, IPC_CREAT | 0666);
if (shmid < 0) die("shmget");

int *shm = (int *)shmat(shmid, NULL, 0);
if (shm == (void *)-1) die("shmat");
pid_t pids[N];
for (int i = 0; i < N; ++i) {
    pid_t pid = fork();
    if (pid < 0) die("fork");
    if (pid == 0) {
        // Child prime-checking logic
    } else {
        pids[i] = pid;
    }
}
