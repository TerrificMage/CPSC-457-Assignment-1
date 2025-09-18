#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <math.h>

void check_prime_range(int start, int end) {
    for (int num = start; num <= end; ++num) {
        int is_prime = 1;
        if (num <= 1) is_prime = 0;
        for (int i = 2; i <= sqrt(num); ++i) {
            if (num % i == 0) {
                is_prime = 0;
                break;
            }
        }
        if (is_prime) {
            printf("Child PID %d checking range [%d, %d] found prime: %d\n", getpid(), start, end, num);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <start> <end> <num_processes>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int start = atoi(argv[1]);
    int end = atoi(argv[2]);
    int num_processes = atoi(argv[3]);

    int range_size = (end - start + 1) / num_processes;

    pid_t pids[num_processes];
    for (int i = 0; i < num_processes; ++i) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {  // Child process
            int child_start = start + i * range_size;
            int child_end = (i == num_processes - 1) ? end : child_start + range_size - 1;
            printf("Child PID %d checking range [%d, %d]\n", getpid(), child_start, child_end);
            check_prime_range(child_start, child_end);
            exit(0);
        } else {  // Parent process
            pids[i] = pid;
        }
    }

    // Parent waits for all children to finish
    for (int i = 0; i < num_processes; ++i) {
        waitpid(pids[i], NULL, 0);
    }

    printf("Parent: All children finished. Primes found: ");
    // Here you could add functionality to collect results from the children, if necessary.
    printf("\n");

    return 0;
}
