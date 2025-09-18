#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int is_prime(int n) {
    if (n <= 1) return 0;
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) return 0;
    }
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <start> <end> <children_count>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int start = atoi(argv[1]);
    int end = atoi(argv[2]);
    int children_count = atoi(argv[3]);

    int range_size = (end - start + 1) / children_count;
    pid_t pids[children_count];

    // This pipe will store primes found by children
    int pipes[children_count][2];
    for (int i = 0; i < children_count; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    // Creating child processes
    for (int i = 0; i < children_count; i++) {
        pids[i] = fork();
        if (pids[i] < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pids[i] == 0) {  // Child process
            int local_start = start + i * range_size;
            int local_end = (i == children_count - 1) ? end : local_start + range_size - 1;

            close(pipes[i][0]);  // Close reading end in the child process

            // Child is working on its range
            printf("Child PID %d checking range [%d, %d]\n", getpid(), local_start, local_end);
            for (int num = local_start; num <= local_end; num++) {
                if (is_prime(num)) {
                    write(pipes[i][1], &num, sizeof(int));  // Write found prime to the pipe
                }
            }
            close(pipes[i][1]);  // Close writing end in the child process
            exit(0);  // Terminate the child process
        }
    }

    // Parent process
    for (int i = 0; i < children_count; i++) {
        waitpid(pids[i], NULL, 0);  // Wait for each child to finish
    }

    printf("Parent: All children finished. Primes found: ");
    // Collect primes from each child
    for (int i = 0; i < children_count; i++) {
        close(pipes[i][1]);  // Close writing end in the parent process

        int prime;
        while (read(pipes[i][0], &prime, sizeof(int)) > 0) {
            printf("%d ", prime);  // Print primes found by the child
        }
        close(pipes[i][0]);  // Close reading end in the parent process
    }

    printf("\n");
    return 0;
}
