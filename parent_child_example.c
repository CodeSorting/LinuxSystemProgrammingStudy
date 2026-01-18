#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    pid_t pid;
    int status;

    pid = fork(); // 자식 프로세스 생성

    if (pid == 0) {
        // 자식 프로세스 영역
        printf("자식: 3초 뒤에 종료합니다. (종료 코드 7)\n");
        sleep(3);
        exit(7); // 7이라는 숫자를 남기고 종료
    } else if (pid > 0) {
        // 부모 프로세스 영역
        printf("부모: 자식이 끝나기를 기다리는 중...\n");
        
        // wait()는 자식이 종료될 때까지 여기서 멈춤(Blocking)
        wait(&status); 
        /*
        // 자식이 끝날 때까지 1초마다 확인 (Non-blocking 방식)
        while (waitpid(pid, &status, WNOHANG) == 0) {
            printf("부모: 자식이 아직 안 끝났네. 1초만 더 딴짓하고 올게...\n");
            sleep(1);
        }
        */
        // status 정보 해독
        if (WIFEXITED(status)) {
            printf("부모: 자식이 정상 종료됨. 종료 코드: %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("부모: 자식이 시그널에 의해 강제 종료됨. 시그널 번호: %d\n", WTERMSIG(status));
        }
    }

    return 0;
}
