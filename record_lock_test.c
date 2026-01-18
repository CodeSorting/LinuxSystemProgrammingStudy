#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define LOCK_T 0
#define UNLOCK_T 1
#define BUFSIZE 1024
//이 파일의 몇 번지부터 몇 번지는 누가 찜했다. 라고 커널에 알리는 함수
int record_lock(int type,int fd,int start,int len) {
    int ret;
    struct flock lock; //커널에게 전달 (잠금 테이블)
    lock.l_type = (type==LOCK_T) ?  : F_UNLCK; //WRLCK 쓰기 잠금, F_RDLCK 읽기 잠금, F_UNCLK 치우기
    lock.l_start = start; //시작지점
    lock.l_whence = SEEK_SET; //어디서부터 잠글 것인가
    lock.l_len = len; //길이
    ret = fcntl(fd,F_SETLK,&lock); //잠그기
    return ret;
}

int main(int argc,char **argv) {
    int fd;
    int record_start,record_len;
    char buf[BUFSIZE] = {0,};
    int i;
    if (argc<4) {
        //파일명, 시작위치, 길이 ex) ./program test.txt 0 10
        printf("Usage: %s [record file] [record start] [record length]\n",argv[0]);
        exit(0);
    }
    fd = open(argv[1],O_RDWR); // 파일 읽고 쓰기로 열기
    if (fd==-1) {
        perror("file open error: ");
        exit(0);
    }
    record_start = atoi(argv[2]); //시작지점
    record_len = atoi(argv[3]); //쓸 길이
    if (record_len>BUFSIZE) {
        printf("record_len(%d) cannot over %d\n",record_len,BUFSIZE);
        exit(0);
    }
    /* record lock */
    if (record_lock(LOCK_T,fd,record_start,record_len)==-1) {
        perror("record lock error: ");
        exit(0);
    }
    /* process data */
    lseek(fd,record_start,SEEK_SET);
    if (read(fd,buf,record_len)<0) { //현재 위치부터 record_len 바이트 읽고 buf에 저장, 커서는 그만큼 이동
        perror("read error: ");
        exit(0);
    }
    printf("record data = %s\n",buf);
    /* data modify */
    for (int i=0;i<record_len;++i) {
        //특정 위치의 데이터가 0이거나 9일 때만 x로 수정하기
        if (buf[i]=='0' || buf[i]=='9') {
            buf[i] = 'x';
        }
    }
    lseek(fd,record_start,SEEK_SET); //파일 커서 시작(SET)부터 10바이트 옮기기
    write(fd,buf,record_len); //record_len 만큼 현재 위치부터 쓰기
    /* delay 20 sec */
    sleep(20);
    printf("record lock process done\n");

    /* record unlock */
    if (record_lock(UNLOCK_T,fd,record_start,record_len)==-1) {
        perror("record unlock error: ");
        exit(0);
    }
    close(fd);
    return 0;
}
/*
어떤 데이터가 저장되어있는 파일을 열어 특정부분의 데이터를 수정을 시도하고,
수정을 시도하는 동안 해당파일의 수정중인 데이터부분에 잠금을 걸어 다른 프로세스가 참조할 수 없도록 수행

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int open(const char *pathname, int flags);
int open(const char *pathname, int flags, mode_t mode);

open은 시스템호출로, 파일을 열거나 생성할때 사용한다.
성공하면 해당파일을 지시하는 int형의 파일디스크립터를 되돌려준다.
path_name은 생성하거나 열고자하는 파일이름을 나타낸다.
보통 full path 이름을 적어주며, 단지 파일이름만 적을경우에는 현재경로에서 찾는다.
flag는 파일을 어떠한 모드로 열것인지를 결정하기 위해서 사용한다.
＂읽기전용＂, ＂쓰기전용＂, ＂읽기/쓰기＂모드로 열수있다.
이들 모드선택을 위해서 O_RDONLY, O_WRONLY, O_RDWR 이존재한다.
open은 mode 인자가 붙은 형식과 붙지않은 형식 둘다 유효하다.
mode 인자는 파일을 생성시 파일의 권한(소유권)을 나타낸다.
파일을 생성(O_CREAT)하지 않으면 mode 인자는 무시된다.
반대로 O_CREAT으로 파일을 생성시 파일 권한이 정의되지 않는다면 종종 골치아픈일을 겪는다.
정상수행시 파일디스크립터 리턴, 에러발생시-1 리턴(errno)

[flags]
O_CREAT : 만약 pathname 파일이 존재하지 않을 경우 파일을 생성한다.
O_EXCL : O_CREAT 를 이용해서 파일을 생성하고자 할 때, 이미 파일이 존재한다면 에러 발생
O_APPEND : 파일이 추가모드로 열린다. 파일의 위치는 파일의 끝이 된다.
O_NONBOLOCK, O_NDELAY : 파일이 비봉쇄(Nonblock) 모드로 열린다.
O_NOFOLLOW : 경로명이 심볼릭 링크라면, 파일 열기에 실패한다.
O_DIRECTORY : 경로명이 디렉토리가 아니라면 파일 열기에 실패한다.
O_SYNC : 입출력 동기화 모드로 열린다. 모든 write 는 데이터가 물리적인 하드웨어에 기록될 때까지 호출 프로세스를 블록

[mode]
S_IRWXU : 00700 모드로 파일 소유자에게 읽기, 쓰기, 쓰기실행권한을 준다.
S_IRUSR : 00400 으로 사용자에게 읽기권한을 준다.
S_IWUSR : 00200 으로 사용자에게 쓰기권한을 준다.
S_IXUSR : 00100 으로 사용자에게 실행권한을 준다.
S_IRWXG : 00070 으로 그룹에게 읽기, 쓰기, 실행권한을 준다.
S_IRGRP : 00040 으로 그룹에게 읽기권한을 준다.
S_IWGRP : 00020 으로 그룹에게 쓰기권한을 준다.
S_IXGRP : 00010 으로 그룹에게 실행권한을 준다.
S_IRWXO : 00007 으로 기타사용자에게 읽기, 쓰기, 실행권한을 준다.
S_IROTH : 00004 으로 기타사용자에게 읽기권한을 준다.
S_IWOTH : 00002 으로 기타사용자에게 쓰기권한을 준다.
S_IXOTH : 00001 으로 기타사용자에게 실행권한을 준다.

[errno]
EEXIST : O_CREAT 와 O_EXECL 이 같이 사용되었을 경우 발생한다. 이미 경로 파일이 존재할 경우 발생된다.
EACCES : 파일 접근이 거부될 경우이다. 주로 권한 문제 때문에 발생한다.
ENOENT : 경로명의 디렉토리가 없거나, 심볼릭 링크가 깨져있을 때.
ENODEV : 경로명이 장치 파일을 참고하고, 일치하는 장치가 없을 때.
EROFS : 경로명이 read-only 파일 시스템을 참조하면서, 쓰기로 열려고 할 때.
EFAULT : 경로명이 접근할 수 없는 주소강간을 가리킬 때
ELOOP : 심볼릭 링크가 너무 많을 때.

#include <unistd.h>
int close(int fd);

close()는 파일 디스크립터를 닫아서 더 이상 파일을 참조하지 않고 재사용할 수 있도록 합니다.
프로세스와 연관되고 프로세스가 소유한 파일에 보유된 모든 레코드 잠금이 제거됩니다.
fd가 열린 마지막 파일 디스크립터인 경우 연관된 자원이 해제됩니다.
파일 디스크립터가 unlink를 사용하여 제거된 파일에 대한 마지막 참조인 경우 파일이 삭제됩니다.
정상수행시 파일디스크립터 리턴, 에러발생시 -1 리턴(errno)

[errno]
EBADF : fd가 유효한 파일 디스크립터가 아닌 경우
EINTR : 시그널 호출에 의해 인터립트된 경우
EIO : I/O 에러 발생된 경우
ENOSPC, SDQUOT : NFS에서 스토리지 공간 초과시 write, fsync, close에 대해 보고

#include <unistd.h>
#include <fcntl.h>
int fcntl(int fd, int cmd, ...);

fcntl()은 파일 디스크립터를 조작합니다.
fcntl()은 열린 파일 디스크립터 fd에 대해 아래 설명된 작업 중 하나를 수행합니다.
작업은 cmd에 의해 결정됩니다.
선택적으로 세 번째 인수를 취할 수 있으며, 필요 여부는 cmd에 의해 결정됩니다.
인수가 필요하지 않은 경우 void가 지정

[CMD]
파일 디스크립터 복제
F_DUPFD (int) : 복사된 파일 디스크립터는 잠금, 파일 위치 포인터, 플래그등을 공유한다.
lseek 등으로 위치 변경 시 복제된 모든 파일 디스크립터도 변경됨

파일 상태 플래그
F_GETFL (void) : 파일 디스크립터에 대한 플래그값(open 호출시 지정한 플래그)을 되돌려준다.
F_SETFL (int) : arg 에 지정된 값으로 파일 디스크립터 fd의 플래그를 재설정한다.
현재는 단지O_APPEND, O_ASYNC, O_DIRECT, O_NONBLOCK 만을 설정할 수 있다.
다른 플래그들(파일액세스 플래그O_WRONLY 와같은, 파일생성 플래그O_CREAT 와 같은) 은 영향을 받지않는다.

레코드잠금
F_SETLK (struct flock *) : 잠금을 획득하거나 잠금을 풀기위해서사용
F_SETLKW (struct flock *) : F_SETLK 과 같은 일을 하지만, 에러 리턴하는 대신 잠금이 풀릴 때까지대기(block)
F_GETLK (struct flock *) : 잠금이 있는지 없는지 검사한다.
struct flock {
    short int l_type; // 잠김 타입: F_RDLCK, F_WRLCK, or F_UNLCK.  
    short int l_whence; // 파일의 절대적 위치 
    __off_t l_start; // 파일의 offset 
    __off_t l_len; // 잠그고자하는 파일의 길이
    __pid_t l_pid; // 잠금을 얻은 프로세스의 pid
};
그외 많은 기능들이 존재한다.


#include <unistd.h>
ssize_t read(int fd, void *buf, size_t count);

socket() 이나 open() 등으로 열린 파일 디스크립터에서 데이터를 읽어들인다.
fd에 읽을 데이터가 있다면 buf에 담아서 가져온다.
count는 buf에 한 번에 가져올 데이터의 크기를 의미한다.
성공할 경우 0 이상의 값을 반환한다. (읽어들인 buf의 크기)
0 이라면 파일의 끝을 의미하며, 데이터를 가져오는데 성공했다면 파일 포인터의 위치는 읽은 데이터 크기만큼 이동
에러가 발생할 경우 -1 리턴(errno)

#include <unistd.h>
ssize_t write(int fd, const void *buf, size_t count);

open()이나 socket() 등으로 열린 파일 디스크립터가 가리키는 파일에 쓴다.
buf는 쓸 데이터이며, count는 쓸 데이터의 크기이다.
성공할 경우 쓰여진 바이트 크기만큼 리턴된다.
0이면 쓰여진 것이 없음을 나타내며, -1일 경우 에러 발생(errno)

#include <sys/types.h>
#include <unistd.h>
off_t leek(int fd, off_t offset, int whence);

lseek()은 열린 파일 디스크립터 fd의 위치 포인터를 offset 만큼 위치를 변경한다.
위치 변경 시 whence를 이용해 기준점을 정할 수 있다.
SEEK_SET - 파일의 처음을 기준으로 offset을 계산
SEEK_CUR - 파일의 현재 위치를 기준으로 offset을 계산
SEEK_END - 파일의 마지막을 기준으로 offset을 계산
성공했을 경우 파일의 시작으로부터 멀어진 byte 만큼의 offset을 리턴한다.
실패했을 경우 -1 리턴(errno)
*/
