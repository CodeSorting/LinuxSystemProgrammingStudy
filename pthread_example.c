#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int total_money;

void *withdrawal(void *arg) {
    int money = *(int *)arg;
    
    pthread_mutex_lock(&mutex);
    printf("withdraw $%d from total balance\n",money);
    total_money -= money;
    sleep(3);
    printf("calculation finish. (withdraw $%d)\n",money);
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

int main() {
    pthread_t thread1,thread2;
    int money1 = 100, money2 = 200;
    total_money = 1000;
    printf("total money is $%d\n",total_money);

    pthread_create(&thread1,NULL,withdrawal,(void *)&money1);
    pthread_create(&thread2,NULL,withdrawal,(void *)&money2);

    pthread_join(thread1,NULL);
    pthread_join(thread2,NULL);
    
    printf("The remained balance is $%d\n",total_money);
    return 0;
}
/*
스레드 생성의 경우 프로세스 생성과 달리 한 프로세스 내에서 생성된 여러 스레드들은 메모리 주소 공간을 공유한다.

한 프로세스 내에서 생성된 여러 프로세스들이 동시에 특정 전역 변수를 참조하는 경우를 생각해보자.
예를 들어 예금 계좌 잔액을 나타내는 total 변수가 존재하고, 현재 잔고가 1000원이 있다고 가정할 때, 동시에 두 개의 스레드에서 total 변수를 참조해서 100원을 인출하는 상황을 생각해보자.

1. 첫 번째 스레드에서 현재 잔고가 1000원이므로 100원을 빼서 900원의 값을 total에 할당하는 중이다.
2. 두 번째 스레드도 현재 잔고가 1000원인 시점에 참조해서 100원을 빼서 900원의 값을 total에 할당한다면,
3. 실제로는 200원을 빼서 total 변수의 최종값이 800원이 되어야 하지만 900원이 되는 문제 상황이 발생할 수 있다.

위와 같이 동시에 공유 자원에 접근하는 상태를 "경쟁 상태(Race Condition)"라 하고, 동시에 참조되는 공유 자원을 참조하는 영역, 즉 스레드가 실행 중 다른 스레드가 끼어들지 말아야 하는 영역을 "크리티컬 섹션(Critical Section)"이라고 한다.

이 "크리티컬 섹션"을 "상호 배제(Mutual Exclusion)"하는 방식으로 접근을 동기화해야 한다.
Pthread에서 상호 배제를 위해 제공하는 락(lock) 매커니즘으로 뮤텍스(Mutex)가 존재한다.

--------------------------------------------------
#include <pthread.h>

// 뮤텍스 초기화 
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// 뮤텍스 락 걸기 
int pthread_mutex_lock(pthread_mutex_t *mutex);

// 뮤텍스 락 풀기 
int pthread_mutex_unlock(pthread_mutex_t *mutex);
--------------------------------------------------

뮤텍스는 pthread_mutex_t 자료구조로 표현된다.
정상 수행 시 0 리턴, 에러 발생 시 0이 아닌 값 리턴 후 errno를 설정한다.
(일반적으로 리턴 값을 검사하지 않는 경향이 있음)

[뮤텍스 사용 예시]
--------------------------------------------------
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *start_func(void *arg)
{
    pthread_mutex_lock(&mutex);

    // 크리티컬 섹션: 공유 자원에 접근하는 코드 

    pthread_mutex_unlock(&mutex);

    return NULL;
}
--------------------------------------------------
*/
