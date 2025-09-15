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
