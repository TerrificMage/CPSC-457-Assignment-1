#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>

#define MAX_PRIMES_PER_CHILD 1000

int *shm_ptr;
int shmid;

int is_prime(int num) {
    if (num < 2) return 0;
    for (int i = 2; i <= sqrt(num); i++) {
        if (num % i == 0) return 0;
    }
    return 1;
}

void die(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s LOWER_BOUND UPPER_BOUND N\n", argv[0]);
        return EXIT_FAILURE;
    }

    int LOWER_BOUND = atoi(argv[1]);
    int UPPER_BOUND = atoi(argv[2]);
    int N = atoi(argv[3]);

    // Create shared memory
    shmid = shmget(IPC_PRIVATE, N * MAX_PRIMES_PER_CHILD * sizeof(int), IPC_CREAT | 0666);
    if (shmid < 0) die("shmget");
    shm_ptr = (int *) shmat(shmid, NULL, 0);
    if (shm_ptr == (int *)(-1)) die("shmat");

    shm_ptr[0] = 0; // shared index counter for primes

    // Fork N child processes
    for (int i = 0; i < N; ++i) {
        pid_t pid = fork();
        if (pid < 0) die("fork");
        if (pid == 0) {
            // Child process
            int start = LOWER_BOUND + i * ((UPPER_BOUND - LOWER_BOUND + 1) / N);
            int end = start + ((UPPER_BOUND - LOWER_BOUND + 1) / N) - 1;

            for (int num = start; num <= end; ++num) {
                if (is_prime(num)) {
                    int index = i * MAX_PRIMES_PER_CHILD + shm_ptr[0];
                    shm_ptr[index] = num;
                    shm_ptr[0]++;
                }
            }
            _exit(0); // Exit child
        }
    }

    // Parent process waits for all children
    for (int i = 0; i < N; ++i) {
        wait(NULL); // Wait for all children
    }

    // Parent reads and prints the primes from shared memory
    printf("Parent: All children finished. Primes found: ");
    for (int i = 1; i < shm_ptr[0]; ++i) {
    if (shm_ptr[i] != 0) {  // Print only valid primes
        printf("%d ", shm_ptr[i]);
    }
}
    printf("\n");


    // Clean up shared memory
    shmdt(shm_ptr);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}
