#include <stdio.h>
#include <unistd.h> //access(),F_OK
#include <sys/stat.h> //chmod(), S_IRWXU, S_IRWXG

int main() {
    char *filename = "./a.out";
    int mode = F_OK;
    if (access(filename,mode)==0) {
        if (chmod(filename, S_IRWXU | S_IRWXG)!=0) {
            printf("chmod() error\n");
            return -1;
        }
    } else {
        printf("file(%s) access error\n",filename);
        return -1;
    }
    return 0;
}
/*
#include <unistd.h>
int chown(const char *pathname, uid_t owner, gid_t group);
경로 이름으로 지정된 파일의 소유권을 변경합니다.
정상수행시 파일0 리턴, 에러발생시 -1 리턴(errno)

*/
