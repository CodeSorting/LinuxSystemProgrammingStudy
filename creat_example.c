#include <stdio.h>
#include <fcntl.h> //mode_t
#include <errno.h> //errno
#include <string.h> //strerror

int main() {
    int fd;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    char *filename = "/tmp/file";
    fd = creat(filename,mode);
    if (fd<0) {
        printf("creat error: %s\n",strerror(errno));
        return -1;
    }
    return 0;
}
/*
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
int creat(const char *pathname, mode_t mode); 
파일 생성용이다. 이미 존재할 경우 초기화 시킨다.
S_IRWXU : 00700 모드로 파일소유자에게 읽기, 쓰기, 쓰기실행권한을준다.
S_IRUSR : 00400 으로 사용자에게 읽기권한을준다.
S_IWUSR : 00200 으로 사용자에게 쓰기권한을준다.
S_IXUSR : 00100 으로 사용자에게 실행권한을준다.
S_IRWXG : 00070 으로 그룹에게 읽기, 쓰기, 실행권한을준다.
S_IRGRP : 00040 으로 그룹에게 읽기권한을준다.
S_IWGRP : 00020 으로 그룹에게 쓰기권한을준다.
S_IXGRP : 00010 으로 그룹에게 실행권한을준다.
S_IRWXO : 00007 으로 기타 사용자에게 읽기, 쓰기, 실행권한을준다.
S_IROTH : 00004 으로 기타 사용자에게 읽기권한을준다.
S_IWOTH : 00002 으로 기타 사용자에게 쓰기권한을준다.
S_IXOTH : 00001 으로 기타 사용자에게 실행권한을준다.
*/
