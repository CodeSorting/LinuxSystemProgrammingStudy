#include <stdio.h>
#include <stdlib.h> /* for atoi() */
#include <sys/types.h>
#include <signal.h>
// gcc -g kill_example.c -o kill_example
// ./signal_example
// ps aux | grep signal_example -> root 2381 0.0 0.0 4196 700 pts/0 S 14:23 0:00 ./signal_example
// ./kill_example 2 2381 -> SIGINT received! loop exit.
int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <signo> <pid>\n", argv[0]);
        return -1;
    }
    if (kill(atoi(argv[2]), atoi(argv[1])) == -1) {
        perror("kill error: ");
        return -1;
    }
    return 0;
}

