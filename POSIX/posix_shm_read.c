#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>

int main() {
    const int SIZE = 1024;
    const char* name = "shmtest";
    int shm_fd; // 공유 메모리 파일 디스크립터
    void *ptr;
    shm_fd = shm_open(name,O_RDONLY,0666); //8진수 666 권한으로 open
    if (shm_fd==-1) {
        perror("shm_open error: ");
        exit(1);
    }
    // mmap(메모리 맵 입출력) 사용, 쓰기용, 다른 프로세스끼리의 메모리 공유 가능
    ptr = mmap(0,SIZE,PROT_READ,MAP_SHARED,shm_fd,0);
    if (ptr==MAP_FAILED) {
        perror("mmap error: ");
        exit(1);
    }
    printf("data: %s",(char *)ptr);
    
    munmap(ptr,SIZE);
    shm_unlink(name);

    return 0;
}
