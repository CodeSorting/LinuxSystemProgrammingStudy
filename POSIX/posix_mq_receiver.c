#include <pthread.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

// 속성 확인 (mq_getattr): 큐에 담길 수 있는 최대 메시지 크기를 확인하여 메모리를 할당합니다.
// 수신 (mq_receive): 실제로 큐에서 데이터를 꺼내와 화면에 출력합니다.
static void tfunc(union sigval sv) {
    struct mq_attr attr;
    ssize_t nr;
    void *buf;
    mqd_t mqdes = *((mqd_t *) sv.sival_ptr);

    if (mq_getattr(mqdes, &attr) == -1)
        handle_error("mq_getattr");

    buf = malloc(attr.mq_msgsize);
    if (buf == NULL)
        handle_error("malloc");

    nr = mq_receive(mqdes, buf, attr.mq_msgsize, NULL);
    if (nr == -1)
        handle_error("mq_receive");

    printf("Read %zd bytes from MQ\n", nr);
    printf("message: %s\n", (char *)buf);
    free(buf);
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
    mqd_t mqdes;
    struct mq_attr attr;
    struct sigevent sev;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <mq-name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (argv[1][0] != '/') {
        fprintf(stderr, "mq-name(%s) must start with slash(/)\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    memset(&attr, 0, sizeof(attr));
    attr.mq_maxmsg = 2;
    attr.mq_msgsize = 1024;

    mqdes = mq_open(argv[1], O_CREAT | O_RDONLY, 0644, &attr);
    if (mqdes == (mqd_t) -1)
        handle_error("mq_open");

    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = tfunc;
    sev.sigev_notify_attributes = NULL;
    sev.sigev_value.sival_ptr = &mqdes; /* for tfunc argument */
    // sev.sigev_notify = SIGEV_THREAD 설정을 통해 메시지가 도착하면 새로운 스레드를 생성해서 특정 함수(tfunc)를 실행하라고 명령한다.
    if (mq_notify(mqdes, &sev) == -1)
        handle_error("mq_notify");

    pause();
}
