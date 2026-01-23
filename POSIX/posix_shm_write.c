#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>

int main() {
    const int SIZE = 1024;
    const char *name = "shmtest";
    int shm_fd; // 공유 메모리 파일 디스크립터
    void *ptr;
    shm_fd = shm_open(name,O_CREAT | O_RDWR, 0666); //8진수 666 권한으로 open
    if (shm_fd==-1) {
        perror("shm_open error: ");
        exit(1);
    }
    // fd로 참조하는 일반 파일이 정확히 SIZE 바이트 크기로 잘리도록 한다.
    if (ftruncate(shm_fd,SIZE)==-1) {
        perror("ftruncate error: ");
        exit(1);
    }
    // mmap(메모리 맵 입출력) 사용, 쓰기용, 다른 프로세스끼리의 메모리 공유 가능
    ptr = mmap(0,SIZE,PROT_WRITE,MAP_SHARED,shm_fd,0);
    if (ptr==MAP_FAILED) {
        perror("mmap error: ");
        exit(1);
    }
    printf("Input data: ");
    if (fgets(ptr,SIZE,stdin)==NULL) {
        perror("fgets erro: ");
        exit(1);
    }
    printf("Written data: %s\n",(char *)ptr);
    munmap(ptr,SIZE);
    
    return 0;
}
/*
POSIX 공유 메모리는 파일 디스크립터를 사용하여 공유 메모리도 파일처럼 인식하도록 사용합니다.
메모리 공간 일부를 파티션 공간(/dev/shm)으로 만들고 여기에 파일을 만들고 페이지 주소를 공유하는 방식입니다.
POSIX 공유 메모리는 컴파일 시 리눅스 리얼타임 라이브러리 링크가 필요합니다. ("-lrt")

[POSIX 공유 메모리 함수 호출 순서]
1단계: 공유 메모리 파일 디스크립터 획득
      shm_fd = shm_open(name)
2단계: 사이즈 지정
      ftruncate(shm_fd, size)
3단계: 공유 메모리 매핑
      mmap(...shm_fd...)
4단계: 공유 메모리 제거
      shm_unlink(name)

#include <sys/mman.h>
#include <sys/stat.h>        // For mode constants 
#include <fcntl.h>           // For O_ constants 

int shm_open(const char name, int oflag, mode_t mode);
int shm_unlink(const char name);

Link with -lrt.
shm_open() 함수는 새로운 POSIX 공유 메모리 객체를 생성하거나 엽니다.
shm_unlink() 함수는 shm_open()에 의해 작성된 객체를 삭제합니다.
shm_open()의 동작은 open()의 동작과 유사합니다. (ex. oflag: O_RDONLY, O_RDWR, etc)
name은 만들거나 열 공유 메모리 객체를 지정합니다.
name은 /dev/shm 경로를 루트('/')로 하는 파일 이름을 의미하며, 만약 name이 "testfile"이라면 
  실제 파일은 "/dev/shm/testfile"을 의미하고 해당 파일에 접근합니다.

#include <unistd.h>
#include <sys/types.h>

[공유 메모리 크기 조절]
int truncate(const char path, off_t length);
int ftruncate(int fd, off_t length);

truncate() 및 ftruncate() 함수는 path로 명명되거나 fd로 참조하는 일반 파일이 정확히 length 바이트 크기로 잘리도록 합니다.
파일의 이전 크기가 length보다 크다면 데이터가 손실됩니다. 
파일의 이전 크기가 작으면 확장되고, 확장된 부분은 널 바이트('\0')로 읽습니다.
성공 시 0 리턴, 실패 시 -1 리턴하며 errno를 설정합니다.
*/
