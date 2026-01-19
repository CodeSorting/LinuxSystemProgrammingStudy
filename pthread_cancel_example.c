#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

void *start_func(void *arg) {
    printf("start_func() just wait... \n");
    while (1) {
        sleep(1);
        printf("start_func() continue!\n");
    }
    return NULL;
}

int main() {
    pthread_t thread_t;
    int ret;
    // 스레드 만들기
    ret = pthread_create(&thread_t,NULL,start_func,NULL);
    if (ret) {
        printf("pthread_create erorr: %s\n",strerror(ret));
        return -1;
    }

    sleep(5);
    // 스레드 중단
    ret = pthread_cancel(thread_t);
    if (ret) {
        perror("pthread_cancel error: ");
        return -1;
    }
    
    printf("Thread canceled\n");
    return 0;
}
/*
#include <pthread.h>
int pthread_cancel (pthread_t thread);

pthread_cancel() 함수를 통해 다른 스레드를 취소시켜 종료할 수 있다.
인자 thread로 표현된 스레드ID를 가진 스레드에 취소요청을 보낸다.
성공시0 리턴, 실패시 thread가 유효하지않다는 ESRCH 리턴
취소요청을 받은 스레드는 pthread_exit(PTHREAD_CANCELED)를 수행한다.
pthread_create()를 통해 만들어 지는 스레드는 별다른 설정이 없을 경우
PTHREAD_CANCEL_ENABLE, PTHREAD_CANCEL_DEFERRED 상태로 만들어진다.
이 의미는 취소가 가능하며, 취소시점은 취소요청이 들어올 경우 안전한 시점에 종료를 한다는 의미이다.

int pthread_setcancelstate (int state, int *oldstate);
int pthread_setcancelgype (int type, int *oldtype);
상기 함수를 통해 취소상태와 타입변경이 가능
*/
