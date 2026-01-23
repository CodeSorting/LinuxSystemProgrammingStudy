#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>

sem_t sem; // 전역 변수로 세마포어 선언

// [방법 1] sem_wait 사용: 자원이 생길 때까지 무조건 기다림 (Blocking)
void* thread_t1(void* arg) {
    int tidx = *(int *)arg;

    printf("[%d] 자원을 기다리는 중 (sem_wait)...\n", tidx);
    sem_wait(&sem); // P 연산: 값이 0보다 크면 -1하고 통과, 0이면 대기
    
    printf("[%d] 자원 획득! 임계 영역 진입\n", tidx);
    sleep(3); // 3초간 작업 수행
    
    printf("[%d] 작업 완료! 자원 반납 (sem_post)\n\n", tidx);
    sem_post(&sem); // V 연산: 값 +1하고 대기 중인 다른 스레드를 깨움
    
    return NULL;
}

// [방법 2] sem_trywait 사용: 자원이 없으면 바로 에러를 내뱉음 (Non-blocking)
void* thread_t2(void* arg) {
    int tidx = *(int *)arg;
    int ret;
retry:
    ret = sem_trywait(&sem); // 자원 획득 시도 (기다리지 않음)

    if (ret == -1) {
        if (errno == EAGAIN) {
            // 자원이 현재 없음 (누군가 사용 중)
            printf("[%d] 자원이 없네요. 1초 뒤에 다시 올게요...\n", tidx);
            sleep(1);
            goto retry; // 다시 시도
        }
        perror("sem_trywait 에러");
        exit(1);
    } 
    
    // 자원 획득 성공 시
    printf("[%d] 자원 획득 성공! 임계 영역 진입\n", tidx);
    sleep(3);
    printf("[%d] 작업 완료! 자원 반납 (sem_post)\n\n", tidx);
    sem_post(&sem);

    return NULL;
}

int main() {
    pthread_t t1, t2, t3;
    int tidx[] = {0, 1, 2};

    // 세마포어 초기화 (자원 개수: 1개)
    // 두 번째 인자 0: 같은 프로세스 내의 스레드 간 공유
    if (sem_init(&sem, 0, 1) == -1) {
        perror("sem_init 에러");
        exit(1);
    }

    // 스레드 생성
    pthread_create(&t1, NULL, thread_t1, (void *)&tidx[0]); // 0번 스레드 (wait 방식)
    pthread_create(&t2, NULL, thread_t1, (void *)&tidx[1]); // 1번 스레드 (wait 방식)
    pthread_create(&t3, NULL, thread_t2, (void *)&tidx[2]); // 2번 스레드 (trywait 방식)

    // 모든 스레드가 끝날 때까지 대기
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    // 세마포어 파괴 (정리)
    sem_destroy(&sem);

    printf("모든 스레드 종료.\n");
    return 0;
}
/*
■ POSIX 세마포어 (POSIX Semaphore)
POSIX 세마포어를 사용하면 프로세스와 스레드가 작업을 동기화할 수 있습니다.

■ POSIX 세마포어의 연산
- sem_wait (3): 세마포어 값을 1 감소시킵니다. (P) 세마포어의 값이 0이면 0보다 커질 때까지 차단됩니다.
- sem_post (3): 세마포어 값을 1 증가시킵니다. (V)

■ POSIX 세마포어의 종류
1. 명명된 세마포어 (Named Semaphore)
- /somename 형식의 이름으로 식별됩니다.
- 초기 슬래시로 구성되는 최대 NAME_MAX-4 (즉, 251) 개의 null로 끝나는 문자열입니다.
- 동일한 이름을 sem_open(3)에 전달하여 두 개의 프로세스가 동일한 이름의 세마포어에서 작동할 수 있습니다.
- sem_open(3) 함수는 새로운 명명된 세마포어를 생성하거나 기존의 명명된 세마포어를 엽니다.
- 세마포어를 연 후에는 sem_post(3) 및 sem_wait(3)을 사용하여 작동할 수 있습니다.
- 프로세스가 세마포어 사용을 마치면 sem_close(3)를 사용하여 세마포어를 닫을 수 있습니다.
- 모든 프로세스가 세마포어 사용을 마치면 sem_unlink(3)를 사용하여 시스템에서 제거할 수 있습니다.

2. 익명 세마포어 (Nameless Semaphore, 메모리 기반 세마포어)
- 이름이 없습니다.
- 대신 세마포어는 여러 스레드(스레드 공유 세마포어) 또는 프로세스(프로세스 공유 세마포어) 간에 공유되는 메모리 영역에 배치됩니다.
- 스레드 공유 세마포어는 프로세스의 스레드(예: 전역 변수) 간에 공유되는 메모리 영역에 배치됩니다.
- 프로세스 공유 세마포어는 공유 메모리 영역(예: shmget(2)을 사용하여 작성된 System V 공유 메모리 세그먼트 또는 shm_open(3)을 사용하여 작성된 POSIX 공유 메모리 오브젝트)에 배치해야 합니다.
- 생성한 프로세스가 종료되면 세마포어가 없어지는 임시 세마포어입니다.
- 사용하기 전에 익명 세마포어는 sem_init(3)을 사용하여 초기화해야 합니다.
- 그런 다음 sem_post(3) 및 sem_wait(3)을 사용하여 작동할 수 있습니다.
- 세마포어가 더 이상 필요하지 않은 경우, 세마포어가 있는 메모리를 할당 해제하기 전에 sem_destroy(3)를 사용하여 세마포어를 제거해야 합니다.

■ 주요 함수 요약
- sem_open: 명명된 세마포어를 생성하거나 오픈
- sem_close: 명명된 세마포어 닫기
- sem_unlink: 명명된 세마포어 제거
- sem_init: 익명 세마포어 생성 및 초기화
- sem_destroy: 익명 세마포어 제거
- sem_wait: P 연산
- sem_post: V 연산
* 주의 사항: 컴파일 시 Link with -pthread

--------------------------------------------------

#include <fcntl.h>    // For O_* constants 
#include <sys/stat.h> // For mode constants 
#include <semaphore.h>

sem_t *sem_open(const char *name, int oflag);
sem_t *sem_open(const char *name, int oflag, mode_t mode, unsigned int value);
int sem_close(sem_t *sem);
int sem_unlink(const char *name);

[명명된 세마포어 사용 예시]
sem_t *sem;
sem = sem_open("/test_sem", O_CREAT | O_EXCL, 0600, 1);
if (sem == SEM_FAILED) {
    if (errno != EEXIST) {
        perror("sem_open");
        exit(1);
    }
    sem = sem_open("/test_sem", 0);
}
...
sem_close(sem);
sem_unlink("/test_sem");

--------------------------------------------------

#include <semaphore.h>

int sem_init(sem_t *sem, int pshared, unsigned int value);
int sem_destroy(sem_t *sem);

[익명 세마포어 인자 설명]
- sem: 초기화 후 리턴받을 세마포어 객체
- pshared: 여러 프로세스에서 공유할 것인지 결정하는 플래그 (0일 경우 현재 프로세스만, 0이 아닐 경우 공유)
- value: 세마포어 초기값

* 익명 세마포어를 pshared를 통해 공유하려는 경우 sem 인수가 프로세스의 로컬이라면 공유는 안 되므로, 익명 세마포어를 공유하려면 sem 인수로 넘어오는 객체가 공유 메모리 공간에 위치해야 한다.

[익명 세마포어 사용 예시]
sem_t sem;
if (sem_init(&sem, 0, 1) == -1) {
    perror("sem_init");
    exit(1);
}
...
sem_destroy(&sem);

--------------------------------------------------

#include <semaphore.h>

int sem_wait(sem_t *sem);
int sem_trywait(sem_t *sem);
int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout);
int sem_post(sem_t *sem);

- sem_wait: P 연산
- sem_trywait: sem_wait에 넌블락킹 기능 추가 (EAGAIN)
- sem_timedwait: sem_wait에 타임아웃 기능 추가 (ETIMEOUT)
* sem_trywait이나 sem_timedwait을 이용하면 sem_wait이 무한대기에 빠질 가능성을 해소할 수 있다.

[sem_timedwait 사용 예시]
struct timespec ts;
ts.tv_sec = time(NULL) + 5;
ts.tv_nsec = 0;
if (sem_timedwait(sem, &ts) == -1) {
    if (errno == ETIMEDOUT) {
        // 5초 타임 아웃 
    } else {
        // 에러 처리 
    }
}
*/
