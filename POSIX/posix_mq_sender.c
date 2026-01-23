#include <pthread.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main(int argc, char *argv[]) {
    mqd_t mqdes;
    ssize_t nr;
    char *buf = "Hello, World!";

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <mq-name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (argv[1][0] != '/') {
        fprintf(stderr, "mq-name(%s) must start with slash(/)\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    mqdes = mq_open(argv[1], O_WRONLY);
    if (mqdes == (mqd_t) -1)
        handle_error("mq_open");

    nr = mq_send(mqdes, buf, strlen(buf) + 1, 0);
    if (nr == -1)
        handle_error("mq_send");

    exit(EXIT_SUCCESS);
}
