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
