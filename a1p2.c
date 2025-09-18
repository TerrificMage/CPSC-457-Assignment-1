#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <math.h>

#define MIN_NUM 100
#define MAX_NUM 249

// Function to check if a number is prime
int is_prime(int num) {
    if (num <= 1) return 0;
    for (int i = 2; i <= sqrt(num); ++i) {
        if (num % i == 0) return 0;
    }
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <start_range> <end_range> <num_children>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int start_range = atoi(argv[1]);
    int end_range = atoi(argv[2]);
    int num_children = atoi(argv[3]);

    int numbers_per_child = (end_range - start_range + 1) / num_children;
    pid_t pids[num_children];

    // Loop to create children
    for (int i = 0; i < num_children; ++i) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork failed");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {  // Child process
            int child_start = start_range + i * numbers_per_child;
            int child_end = (i == num_children - 1) ? end_range : (child_start + numbers_per_child - 1);

            // Print which range the child is searching
            printf("Child %d (PID: %d) searching range %d to %d\n", i, getpid(), child_start, child_end);

            for (int num = child_start; num <= child_end; ++num) {
                if (is_prime(num)) {
                    printf("Child %d found prime number: %d\n", i, num);
                }
            }

            exit(0);
        } else {
            pids[i] = pid;
        }
    }

    // Parent process waits for all children to finish
    for (int i = 0; i < num_children; ++i) {
        waitpid(pids[i], NULL, 0);
    }

    printf("Parent: All children finished.\n");

    return 0;
}
