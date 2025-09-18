#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

enum { ROWS = 100, COLS = 1000, NOT_FOUND = 255 };
static int matrix[ROWS][COLS];

static void die(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(void) {
    // Read the 100x1000 matrix from stdin
    int one_count = 0;
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            int x;
            if (scanf("%d", &x) != 1) {
                fprintf(stderr, "Input error: expected %dx%d integers\n", ROWS, COLS);
                return EXIT_FAILURE;
            }
            if (x != 0 && x != 1) {
                fprintf(stderr, "Input error: values must be 0 or 1\n");
                return EXIT_FAILURE;
            }
            matrix[r][c] = x;
            if (x == 1) one_count++;
        }
    }
    // Comment out the debug line once confirmed
    // fprintf(stderr, "DEBUG: Matrix read complete, count of 1s = %d\n", one_count);

    int pipes[ROWS][2];
    pid_t pids[ROWS];

    for (int r = 0; r < ROWS; ++r) {
        if (pipe(pipes[r]) == -1) die("pipe");

        pid_t pid = fork();
        if (pid < 0) die("fork");

        if (pid == 0) {  // Child process
            // Log child creation and the row it is searching
            printf("Child PID: %d, searching row %d\n", getpid(), r);

            // Close unused pipe ends
            for (int i = 0; i < ROWS; ++i) {
                if (i != r) {
                    close(pipes[i][0]);
                    close(pipes[i][1]);
                }
            }
            close(pipes[r][0]);

            // Search for treasure (1) in row r
            for (int c = 0; c < COLS; ++c) {
                if (matrix[r][c] == 1) {
                    // Send the column number where the 1 was found to parent
                    if (write(pipes[r][1], &c, sizeof(int)) != sizeof(int)) {
                        _exit(NOT_FOUND);
                    }
                    close(pipes[r][1]);
                    _exit(r);  // Send row as exit code
                }
            }

            // If no treasure (1) found in the row
            close(pipes[r][1]);
            _exit(NOT_FOUND);
        } else {  // Parent process
            pids[r] = pid;
            close(pipes[r][1]);  // Close write-end in the parent process
        }
    }

    // Parent waits for each child to finish
    int found = 0;
    int found_row = -1;
    int found_col = -1;
    pid_t found_pid = -1;

    for (int k = 0; k < ROWS; ++k) {
        int status;
        pid_t pid = wait(&status);
        if (pid == -1) die("wait");

        if (WIFEXITED(status)) {
            int code = WEXITSTATUS(status);
            if (code != NOT_FOUND && !found) {
                found = 1;
                found_row = code;
                found_pid = pid;

                int col;
                if (read(pipes[found_row][0], &col, sizeof(int)) != sizeof(int)) {
                    fprintf(stderr, "Parent: Error reading from pipe\n");
                    exit(EXIT_FAILURE);
                }
                found_col = col;
            }
        }
    }

    // Output the final results
    if (found) {
        printf("Parent: The treasure was found by child with PID %d at row %d, column %d\n", (int)found_pid, found_row, found_col);
    } else {
        printf("Parent: No treasure was found in the matrix\n");
    }

    return 0;
}
