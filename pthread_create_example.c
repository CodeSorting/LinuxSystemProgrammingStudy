#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

void *start_func(void *arg) {
    int num = *(int *)arg;
    for (int i=0;i<num;++i) {
        printf("%d\n",i);
    }
    pthread_exit((void *)0);
}

int main() {
    int num = 10;
    pthread_t thread_t;
    int ret,status;
    ret = pthread_create(&thread_t,NULL,start_func,(void *)&num);
    if (ret) {
        printf("pthread_create error: %s\n",strerror(ret));
        return -1;
    }
    pthread_join(thread_t, (void **)&status);
    printf("Thread returned: %d\n",status);
    return 0;
}
/*
리눅스에서는 Pthread API를 통해 스레드를 지원하고 있다.
일반적으로 Pthread라고 하는 POSIX 스레드는 표준 POSIX.1c, 스레드 확장(IEEE std 1003.1c-1995)에 정의된 API이다.

Pthread API는 <pthread.h> 파일에 정의되어 있으며, API의 모든 함수는 pthread_로 시작한다.
Pthread API가 코드에 추가된 경우 gcc를 통해 컴파일할 때는 –pthread 플래그를 통하여 libpthread 라이브러리를 링크해 주어야 한다.

# gcc -pthread hello.c –o hello

다음으로 스레드 생성, 종료, 대기, 동기화하기 위한 API들을 알아보자.

--------------------------------------------------
#include <pthread.h>

int pthread_create (pthread_t *thread,
                    const pthread_attr_t *attr,
                    void *(*start_routine) (void *),
                    void *arg);
--------------------------------------------------

pthread_create()는 새로운 스레드를 생성한다.
생성되는 새로운 스레드는 arg를 인자로 하는 start_routine 함수를 수행한다.

- thread 인자: NULL이 아니라면, 새로 만든 스레드를 나타내는 스레드 ID를 저장한다.
- attr 인자: 스레드 생성 시 속성을 변경하기 위한 값이며, NULL일 경우 기본 속성을 따른다.

성공할 경우 0 리턴, 실패할 경우 0이 아닌 에러 코드를 직접 리턴한다.

[에러 코드(errno)]
- EAGAIN: 새로운 스레드를 만들기 위한 리소스 부족
- EINVAL: 유효하지 않은 attr 속성값
- EPERM: 권한 오류

--------------------------------------------------
void * start_routine(void *arg);
--------------------------------------------------

start_routine() 함수는 상기와 같은 형식을 가진다.
fork()와 유사하게 새로 생성된 스레드는 부모 스레드로부터 대부분의 속성과 기능, 상태 등을 상속받는다.
하지만 프로세스와는 다르게 스레드는 부모 스레드의 리소스를 공유한다.
생성된 스레드는 pthread_exit()을 호출하거나 start_routine에서 return할 경우 종료된다.

--------------------------------------------------
#include <pthread.h>

void pthread_exit (void *retval);
--------------------------------------------------

pthread_exit()는 현재 실행 중인 스레드를 종료시키기 위해 호출한다.
retval은 해당 스레드가 종료되기를 기다리는 다른 스레드에게 전달할 값이다.

#include <pthread.h>

int pthread_join (pthread_t thread, void **retval);

pthread_join()은 thread 인자로 명시한(스레드 ID를 가진) 스레드가 종료될 때까지 대기하도록 한다.
해당 스레드가 이미 종료되었다면 pthread_join()은 즉시 리턴된다.
retval은 NULL이 아닐 경우 종료한 스레드가 리턴한 값이다.

- 정상 종료 시 0 리턴, 에러 발생 시 에러 코드(errno) 리턴

[에러 코드(errno)]
- EDEADLK: 데드락 감지
- EINVAL: thread는 조인 불가한 스레드
- ESRCH: thread 인자가 유효하지 않음

--------------------------------------------------
int ret;
ret = pthread_join(thread, NULL);
if (ret) {
    printf("pthread_join error: %s\n", strerror(ret));
    return -1;
}
--------------------------------------------------

#include <pthread.h>

int pthread_detach (pthread_t thread);

기본적으로 스레드는 조인이 가능하도록 생성되지만,
pthread_detach()를 이용하면 조인이 가능하지 않도록 하는 것도 가능하다.
조인할 생각이 없는 스레드는 디태치하면 불필요한 시스템 자원을 낭비하지 않는다.
즉, "난 이 스레드의 결과값도 궁금하지 않고, 그냥 백그라운드에서 알아서 돌다 끝나길 원해"라고 생각할 때 사용한다.
- thread(스레드 ID)를 인자로 호출에 성공하면 0을 리턴
- 실패 시 thread 인자가 유효하지 않다는 의미로 ESRCH 리턴

*/
