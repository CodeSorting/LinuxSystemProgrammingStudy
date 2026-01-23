#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

/* 메시지 구조체 정의 */
struct msgbuf {
    long mtype;       /* 메시지 타입 */
    char mtext[80];   /* 메시지 내용 */
};

/* 사용법 출력 함수 */
static void usage(char *prog_name, char *msg) {
    if (msg != NULL)
        fputs(msg, stderr);

    fprintf(stderr, "Usage: %s [options]\n", prog_name);
    fprintf(stderr, "Options are:\n");
    fprintf(stderr, "-s        send message using msgsnd()\n");
    fprintf(stderr, "-r        read message using msgrcv()\n");
    fprintf(stderr, "-t        message type (default is 1)\n");
    fprintf(stderr, "-k        message queue key (default is 1234)\n");
    exit(EXIT_FAILURE);
}

/* 메시지 전송 함수 */
static void send_msg(int qid, int msgtype) {
    struct msgbuf msg;
    time_t t;

    msg.mtype = msgtype;

    time(&t);
    snprintf(msg.mtext, sizeof(msg.mtext), "a message at %s", ctime(&t));

    /* msgsnd를 이용해 메시지 큐에 데이터 전송 */
    if (msgsnd(qid, (void *)&msg, sizeof(msg.mtext), IPC_NOWAIT) == -1) {
        perror("msgsnd error");
        exit(EXIT_FAILURE);
    }
    printf("sent: %s\n", msg.mtext);
}

/* 메시지 수신 함수 */
static void get_msg(int qid, int msgtype) {
    struct msgbuf msg;

    /* msgrcv를 이용해 메시지 큐에서 데이터 읽기 */
    if (msgrcv(qid, (void *)&msg, sizeof(msg.mtext), msgtype, MSG_NOERROR | IPC_NOWAIT) == -1) {
        if (errno != ENOMSG) {
            perror("msgrcv");
            exit(EXIT_FAILURE);
        }
        printf("No message available for msgrcv()\n");
    } else {
        printf("message received: %s\n", msg.mtext);
    }
}

int main(int argc, char *argv[]) {
    int qid, opt;
    int mode = 0;               /* 1 = send, 2 = receive */
    int msgtype = 1;
    int msgkey = 1234;

    /* 명령줄 인자 처리 (getopt) */
    while ((opt = getopt(argc, argv, "srt:k:")) != -1) {
        switch (opt) {
            case 's':
                mode = 1;
                break;
            case 'r':
                mode = 2;
                break;
            case 't':
                msgtype = atoi(optarg);
                if (msgtype <= 0)
                    usage(argv[0], "-t option must be greater than 0\n");
                break;
            case 'k':
                msgkey = atoi(optarg);
                break;
            default:
                usage(argv[0], "Unrecognized option\n");
        }
    }

    if (mode == 0)
        usage(argv[0], "must use either -s or -r option\n");

    /* 메시지 큐 생성 또는 기존 큐 열기 */
    qid = msgget(msgkey, IPC_CREAT | 0666);

    if (qid == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    if (mode == 2)
        get_msg(qid, msgtype);
    else
        send_msg(qid, msgtype);

    exit(EXIT_SUCCESS);
}
