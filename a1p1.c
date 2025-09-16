#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

enum { ROWS = 100, COLS = 1000, NOT_FOUND = 255 };

static void die(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(void) {
    // Initial program setup, matrix reading to follow.
}
static int matrix[ROWS][COLS];

// Read a 100x1000 matrix of 0/1 from stdin (robust to whitespace/newlines)
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
    }
}
int pipes[ROWS][2];
for (int r = 0; r < ROWS; ++r) {
    if (pipe(pipes[r]) == -1) die("pipe");
}
pid_t pids[ROWS];
for (int r = 0; r < ROWS; ++r) {
    pid_t pid = fork();
    if (pid < 0) die("fork");
    if (pid == 0) {
        // Child process code
    } else {
        pids[r] = pid;
        close(pipes[r][1]);
    }
}
if (pid == 0) {
    for (int i = 0; i < ROWS; ++i) {
        close(pipes[i][0]);
        if (i != r) close(pipes[i][1]);
    }

    int found_col = -1;
    for (int c = 0; c < COLS; ++c) {
        if (matrix[r][c] == 1) { found_col = c; break; }
    }

    if (found_col >= 0) {
        if (write(pipes[r][1], &found_col, sizeof(found_col)) != sizeof(found_col)) {
            _exit(r & 0xFF); // success: return row index
        }
        close(pipes[r][1]);
        _exit(NOT_FOUND); // No treasure found
    }
}
for (int k = 0; k < ROWS; ++k) {
    int status;
    pid_t pid = wait(&status);
    if (pid == -1) die("wait");

    if (WIFEXITED(status)) {
        int code = WEXITSTATUS(status);
        if (code != NOT_FOUND && !found) {
            found = 1;
            found_row = code;  
            pid_t found_pid = pid;
            int col;
            read(pipes[found_row][0], &col, sizeof(col));
            found_col = col;
        }
    }
}
if (found && found_row >= 0 && found_col >= 0) {
    printf("Parent: The treasure was found by child with PID %d at row %d, column %d\n",
           (int)found_pid, found_row, found_col);
} else {
    printf("Parent: No treasure was found in the matrix\n");
}
return 0;
