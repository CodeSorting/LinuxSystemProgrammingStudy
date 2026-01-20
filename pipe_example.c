#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

/*
코드 중간중간 close()를 호출하는 이유는 크게 두 가지입니다.

데이터 흐름의 명확성: 읽기 전용으로 쓸 파이프의 쓰기 끝단을 닫지 않으면, 나중에 read() 함수가 "누군가 아직 데이터를 쓸 수도 있겠구나"라고 판단하여 무한 대기(Block) 상태에 빠질 수 있습니다.

자원 관리: 파일 디스크립터는 시스템 자원이므로 사용하지 않는 기능은 바로 닫아주는 것이 시스템 프로그래밍의 정석입니다.
*/
int main() {
    int fd1[2]; //parent->child write
    int fd2[2]; //child->parent write
    // fd[0]: 데이터를 꺼내는 곳 (Read - 읽기 전용)
    // fd[1]: 데이터를 집어넣는 곳 (Write - 쓰기 전용)
    pid_t p;
    if (pipe(fd1)==-1) {
        fprintf(stderr,"pipe error: %m");
        return -1;
    } 
    if (pipe(fd2)==-1) {
        fprintf(stderr,"pipe error: %m");
        return -1;
    }
    p = fork();
    if (p<0) {
        fprintf(stderr,"fork error: %m");
        return 1;
    } else if (p>0) { //parent
        char writebuff[] = "Did you understand?";
        char readbuff[128] = {0};
        
        close(fd1[0]); //close read of fd1
        write(fd1[1],writebuff,strlen(writebuff)+1);;
        close(fd1[1]);
        
        wait(NULL);

        close(fd2[1]); //close write of fd2
        read(fd2[0],readbuff,sizeof(readbuff));
        close(fd2[0]);

        printf("[Parent] read from Child: %s\n",readbuff);
    } else { //child
        char readbuff[128] = {0};
        char writebuff[] = "OK!";
 
        close(fd1[1]); //close write of fd1
        read(fd1[0],readbuff,sizeof(readbuff));;
        close(fd1[0]);
        
        printf("[Child] read from Parent: %s\n",readbuff);
        
        close(fd2[0]); //close read of fd2
        write(fd2[1],writebuff,strlen(writebuff)+1);
        close(fd2[1]);
        
        exit(0);
    }
    return 0;
}
/*
파이프란?
파이프 한 쪽에서 물을 흘리면 반대쪽 파이프에서 물이 흘러나오는 것과 같다.
한 쪽에서 데이터를 흘리면 다른 한 쪽에서 데이터를 받을 수 있다.

파이프에는 익명 파이프(anonymous pipe)와 지명 파이프(named pipe), 2종류가 있다.
익명 파이프는 서로 관련된 프로세스들만 공유할 수 있고,
지명 파이프는 관련이 없는 프로세스들도 공유할 수 있다.

지명 파이프를 유닉스에서 구현하면서 FIFO로 이름을 붙였기 때문에
FIFO라 하면 지명 파이프를 의미하고,
반대로 그냥 파이프라고 하면 익명 파이프를 의미한다.

쉘에서 익명 파이프
쉘에서 명령을 수행할 때 버티컬 바('|') 문자를 사용하여
하나의 프로세스 출력을 다른 프로세스의 입력으로 연결할 수 있다.

#include <unistd.h>
익명 파이프(Pipe)
int pipe(int pipefd[2]);

pipe()는 프로세스 간 통신에 사용할 수 있는 단방향 데이터 채널인 파이프를 생성합니다.
pipefd 배열은 파이프의 끝을 참조하는 두 개의 파일 디스크립터를 반환하는 데 사용됩니다.
pipefd[0]는 파이프의 읽기 끝을 나타내고, pipefd[1]은 파이프의 쓰기 끝을 나타냅니다.
파이프의 쓰기 끝에 기록된 데이터는 파이프의 읽기 끝에서 읽을 때까지 커널에 의해 버퍼링됩니다.
pipe()는 fork를 통한 부모, 자식 프로세스 간에 데이터를 교환할 필요가 있을 때 사용됩니다.

정상 수행 시 0을 리턴하며, 실패 시 -1 리턴 후 errno 설정
사용이 끝나면 다른 파일 디스크립터 사용과 동일하게 close(pipefd[0]), close(pipefd[1])를 통해 닫아주어야 합니다.
*/
