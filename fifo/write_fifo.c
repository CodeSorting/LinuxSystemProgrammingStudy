#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
/*
FIFO를 이용해 두 프로세스가 채팅하듯이 통신하는 프로그램을 실습해보자.

지명 파이프 실습 시나리오
1. FIFO 파일이 존재하지 않으면 생성한다.
2. 한 프로그램은 FIFO를 읽은 후 입력받은 문자열을 FIFO에 쓰고, (write_fifo)
3. 다른 프로그램은 FIFO에 입력받은 문자열을 쓴 후 FIFO를 읽는다. (read_fifo)
*/
int main() {
    int fd;
    char *fifofile = "/tmp/fifo_test";
    char inputbuf[1024];
    char readbuf[1024];

    if (access(fifofile, F_OK) != 0) {
        //0666 = 0->8진수, 666->소유자,그룹,그외 사용자 권한
        if (mkfifo(fifofile, 0666) == -1) {
            fprintf(stderr, "mkfifo error: %m\n");
            exit(1);
        }
        printf("FIFO file(%s) create OK.\n", fifofile);
    }
    
    while (1) {    
        printf("input message: ");
        fgets(inputbuf, sizeof(inputbuf), stdin);
        
        /* Write to FIFO */
        fd = open(fifofile, O_WRONLY);
        write(fd, inputbuf, strlen(inputbuf)+1);
        close(fd);
        
        /* Read from FIFO */
        fd = open(fifofile, O_RDONLY);
        read(fd, readbuf, sizeof(readbuf));
        close(fd);
        
        printf("read: %s", readbuf);
    }
    return 0;
}
/*
명명된 파이프(named pipe) 혹은 지명 파이프는 일반 파이프를 확장한 것으로, 프로세스 간 통신 기법 중 하나이다.

03 지명 파이프
생성된 FIFO 파일은 파이프 역할을 수행하며
해당 파일로 데이터를 보낼 수도 있고 (ex. cat file > fifo_pipe)
일반 파일처럼 삭제할 수도 있다. (ex. rm fifo_pipe)

#include <sys/types.h>
#include <sys/stat.h>
int mkfifo(const char *pathname, mode_t mode);

#include <fcntl.h>
#include <sys/stat.h>
 Definition of AT_* constants
int mkfifoat(int dirfd, const char *pathname, mode_t mode);

mkfifo()는 이름이 pathname 인 FIFO 특수 파일을 생성합니다. mode는 FIFO의 권한을 지정합니다.
FIFO 특수 파일을 만든 후에는 일반 파일과 같은 방식으로 모든 프로세스에서 파일을 읽거나 쓸 수 있습니다.
FIFO로부터 읽어들이는 수신 측은 O_RDONLY로 파일을 열고, 송신 측은 O_WRONLY로 파일을 열어야 합니다.
일반적으로 수신 측이 FIFO를 먼저 open 하는데 송신 측이 open 할 때까지 블록된다.

read, write를 통해서 읽고 쓰는 도중
수신 측에서 FIFO를 close 하면 송신 측 write는 SIGPIPE 시그널 수신,
송신 측에서 FIFO를 close 하면 수신 측 read는 0을 리턴

mkfifoat()은 파일 디스크립터 dirfd가 가리키는 디렉토리 위치에서 상대 경로로 파일을 열어주는 것 외에는
mkfifo()와 동일

성공 시 0 리턴, 실패 시 -1 리턴 후 errno 설정
*/
