#include <stdio.h>
#include <unistd.h>

int main() {
    printf("daemon start\n");
    if (daemon(0, 0) == -1) {
        perror("daemon erro: ");
        return -1;
    }
    while (1) {
        printf("not print line!\n");
        sleep(1);
    }
    return 0;
}
/*
#include <unistd.h>
int daemon(int nochdir, int noclose);
현재 프로세스를 백그라운드로 수행할 수 있다.
nochdir 인자가 0이 아니면 현재 작업디렉토리를 루트디렉토리로 변경하지 않는다.
noclose 인자가 0이 아니면 열려있는 모든 파일디스크립터를 닫지 않는다.
일반적으로 모두 0으로 설정
성공하면 0 리턴, 실패할경우 -1 리턴(errno)
*/
