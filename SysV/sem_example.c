#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <pthread.h>

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

struct sembuf p = { 0, -1, SEM_UNDO }; //열쇠 가져가니 값 -1
struct sembuf v = { 0, +1, SEM_UNDO }; //열쇠 돌려주니 값 +1
#define SEM_P(ID) ({\
    if (semop(ID, &p, 1) < 0) {\
        perror("semop p");\
        exit(1);\
    }\
})

#define SEM_V(ID) ({\
    if (semop(ID, &v, 1) < 0) {\
        perror("semop v");\
        exit(1);\
    }\
})

int sem_use;
int semid;
char shared_buf[] = "Hello, World!\n";
int shared_index = 0;
void *print_one(void *arg) {
    int tidx = *(int *)arg;
    bool b = true;
    char c;
    while (b) {
        if (sem_use) SEM_P(semid); //열쇠 검사 (가져감)
        if (shared_index >= strlen(shared_buf)) {
            b = false;
        } else { //shared_index를 읽고 글자를 변환하고, 화면에 출력함.
            if (tidx) c = toupper(shared_buf[shared_index++]);
            else c = tolower(shared_buf[shared_index++]);
            sleep(rand()%2);
            putchar(c);
            fflush(stdout);
        }
        if (sem_use) SEM_V(semid); //열쇠 반납
    }
    pthread_exit((void *)0);
}

int main(int argc,char *argv[]) {
    key_t key;
    union semun u;
    int pid;
    pthread_t thread_t;
    int ret;
    int tidx[2] = {0,1};
    if (argc<2) {
        printf("Usage: %s [0(OFF)/1(ON)]\n",argv[0]);
        exit(1);
    }
    sem_use = atoi(argv[1]);
    key = ftok("shmtest",1234);
    semid = semget(key,1,0666 | IPC_CREAT);

    if (semid<0) {
        perror("semget");
        exit(1);
    }
    u.val = 1; //세마포어 초기값 1
    if (semctl(semid,0,SETVAL,u)<0) {
        perror("semctl");
        exit(1);
    }
    for (int i=0;i<=1;++i) {
        ret = pthread_create(&thread_t,NULL,print_one,(void *)&tidx[i]);
        if (ret) {
            perror("pthread_create");
            exit(1);
        }
    }
    pthread_join(thread_t,NULL);
    pthread_join(thread_t,NULL);
    return 0;
}
/*
■ 세마포어(Semaphore) 개념
세마포어(Semaphore)는 다익스트라(Dijkstra)가 고안한, 두 개의 원자적 함수로 조작되는 정수 변수로서, 멀티 프로그래밍 환경에서 공유 자원에 대한 접근을 제한하는 방법(상호 배제)으로 사용된다. – Wikipedia

■ 원리
세마포어 S는 정수 값을 가지는 변수이며, 다음과 같이 P와 V라는 명령에 의해서만 접근할 수 있습니다.
(P와 V는 각각 try와 increment를 뜻하는 네덜란드어의 머릿글자를 딴 것입니다. 영어로는 wait(), signal())

P는 임계 영역에 들어가기 전에 수행되고, V는 임계 영역에서 나올 때 수행됩니다.
이때 변수 값을 수정하는 연산은 모두 원자성을 만족해야 합니다.
즉, 한 프로세스에서 세마포어 값을 변경하는 동안 다른 프로세스가 동시에 이 값을 변경해서는 안 됩니다.

■ 종류
- 계수 세마포어(Counting Semaphore): 다수 개의 자원에 대해 카운팅이 가능한 세마포어 (0 ~ n)
- 이진 세마포어(Binary Semaphore): 1개의 자원에 대해 카운팅, 즉 세마포어 값은 0 또는 1을 가집니다.

■ 설명
계수 세마포어의 경우 3개의 자원이 있다고 가정할 때 (S = 3) 여러 프로세스가 자원에 접근을 시도합니다.
자원에 접근할 때마다 세마포어를 하나씩 감소시키고(P), 세마포어 값이 0이 되면 자원에 접근을 시도하는 다른 프로세스는 대기하게 됩니다.
자원에 접근했던 프로세스가 접근을 해제하면 세마포어를 다시 하나씩 증가시키고(V), 대기하던 프로세스는 다시 자원에 접근이 가능하게 됩니다.

이진 세마포어는 계수 세마포어의 자원 값이 1개인 경우 (S = 1)를 의미합니다. (뮤텍스와 유사)

■ SysV 세마포어의 주요 함수
- semget: 세마포어의 IPC ID를 생성하거나 가져옴
- semctl: 세마포어 조작 (초기화, 정보 획득, 제거)
- semop: 세마포어 증가/감소
- semtimedop: 타임아웃 기능이 추가된 semop

■ 흐름
1) semget()을 통해 ID 획득
2) 새로 생성된 경우 semctl()을 통해 세마포어 초기화
3) semop()를 이용해 세마포어 감소 (P)
4) 처리 (Critical Section)
5) semop()를 이용해 세마포어 증가 (V)
6) semctl()을 통해 세마포어 정리

--------------------------------------------------

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

int semget(key_t key, int nsems, int semflg);
- semget() 시스템 호출은 인수 키와 연관된 System V 세마포어 세트 ID를 리턴합니다.
- 키에 IPC_PRIVATE 값이 있거나 키와 연관된 기존 세마포어 세트가 없고 semflg에 IPC_CREAT가 지정된 경우 새로운 nsems 개의 세마포어 세트가 작성됩니다. (shmget과 동일)
- semflg에 IPC_CREAT와 IPC_EXCL을 모두 지정하고 키와 연관된 세마포어 세트가 이미 존재하는 경우 결과는 실패하고 errno가 EEXIST로 설정됩니다. (open(2)의 O_CREAT | O_EXCL 조합의 효과와 유사합니다.)
- semflg 인수의 최하위 9비트는 세마포어 세트에 대한 권한(소유자, 그룹, 기타에 대한 권한)을 정의합니다. 이러한 비트는 open(2)의 모드 인수와 동일한 형식 및 의미를 갖습니다. (실행 권한은 의미가 없음, 쓰기 권한은 세마포어 값을 변경할 수 있는 권한을 의미)
- 성공 시 양수 세마포어 세트 식별자 리턴, 실패 시 -1 리턴 후 errno 설정

예) 키 값으로 1234를 사용하고 접근 권한 666으로 세마포어를 새롭게 생성하는 경우
int sem_id = semget((key_t)1234, 1, 0666 | IPC_CREATE);

--------------------------------------------------

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

int semctl(int semid, int semnum, int cmd, ...);
- semctl()은 semid로 식별된 System V 세마포어 세트의 semnum 번째 세마포어에서 cmd로 지정된 제어 조작을 수행합니다. (세트의 세마포어는 0부터 시작하여 번호가 매겨집니다.)
- cmd의 종류에 따라 뒤에 semun 공용체 인자가 추가될 수 있습니다.

union semun {
    int val;                // Value for SETVAL 
    struct semid_ds *buf;   // Buffer for IPC_STAT, IPC_SET 
    unsigned short *array;  // Array for GETALL, SETALL 
    struct seminfo *__buf;  // Buffer for IPC_INFO (Linux-specific) 
};

- 실패 시 -1 리턴 후 errno 설정
- 성공 시 cmd에 따라 리턴 값 달라짐

[cmd 값 설명]
- IPC_STAT(*) : IPC 자원 정보 획득 (생성자, 생성 시간, 접근 권한 등)
- IPC_SET(*)  : IPC 자원 정보 설정
- IPC_RMID    : IPC 자원 제거
- IPC_INFO(*) : IPC 자원 시스템 설정값 획득 (리눅스 전용)
- SEM_INFO(*) : IPC_INFO와 유사, 시스템 전체 세마포어 수 획득 가능 (리눅스 전용)
- SEM_STAT(*) : IPC_STAT과 유사, 시스템 전체 세마포어 배열 인덱스 획득 (리눅스 전용)
- GETALL(*)   : 배열을 이용해 모든 세마포어를 한꺼번에 읽어들임
- GETNCNT     : semnum 위치의 세마포어의 semncnt 값을 리턴
- GETPID      : semnum 위치의 세마포어 sempid 값을 리턴
- GETVAL      : semnum 위치의 세마포어 값을 리턴
- GETZCNT     : semnum 위치의 세마포어 semzcnt 값을 리턴
- SETALL(*)   : 배열을 이용해 모든 세마포어를 한꺼번에 초기화
- SETVAL(*)   : semnum 위치의 세마포어 값을 초기화
(*) 표시된 cmd의 경우 4번째 인자로 semun 공용체 사용

--------------------------------------------------

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

int semop(int semid, struct sembuf *sops, size_t nsops);
int semtimedop(int semid, struct sembuf *sops, size_t nsops, const struct timespec *timeout);

- semop()은 semid로 표시된 세트에서 선택된 세마포어에 대한 작업을 수행합니다.
- sops가 가리키는 배열의 각 nsops 요소는 단일 세마포어에서 수행할 작업을 지정하는 구조입니다.

struct sembuf {
    unsigned short sem_num;  // semaphore number 
    short sem_op;           // semaphore operation 
    short sem_flg;          // operation flags 
};

- 작업은 sops 배열 순서로 진행되며, 각 작업은 세마포어 세트의 sem_num 번째 세마포어에서 수행됩니다.
- sem_op 값이 양수이면 세마포어 값에 추가(V)하고, 음수이면 세마포어 값에서 뺍니다(P).

[sem_flg 플래그]
- IPC_NOWAIT : 사용 가능한 자원이 없을 때 기다리지 않고 에러 리턴 (EAGAIN)
- SEM_UNDO   : 프로세스가 종료될 때 세마포어 조작을 모두 취소. 프로세스가 잠금을 한 채로 종료될 경우 발생할 수 있는 무한 대기 문제를 해결합니다.
*/
