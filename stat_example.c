#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc,char *argv[]) {
    struct stat sb;
    int ret;
    if (argc<2) {
        printf("Usage: %s <file>\n",argv[0]);
        return -1;
    }
    ret = stat(argv[1],&sb);
    if (ret) {
        perror("stat error: ");
        return -1;
    }
    printf("file(%s) is %ld bytes\n",argv[1],sb.st_size);
    return 0;
}
/*
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int stat(const char *path, struct stat *buf);
리눅스는 파일의 메타데이터 정보를 얻기위해 stat()을 제공합니다.
stat 구조체에 정보를 저장하며 stat 구조체는<bits/stat.h>에 선언되어있고 이는<sys/stat.h>에 포함되어있다.

struct stat {
    dev_t     st_dev;     // 파일을 포함하고 있는 장치 ID 
    ino_t     st_ino;     // inode 번호 
    mode_t    st_mode;    // 권한 (permissions) 
    nlink_t   st_nlink;   // 하드 링크 수 
    uid_t     st_uid;     // user ID 
    gid_t     st_gid;     // group ID 
    dev_t     st_rdev;    // device ID (특수 파일일 경우) 
    off_t     st_size;    // 바이트 단위의 전체 사이즈 
    blksize_t st_blksize; // filesystem I/O 를 위한 블록 사이즈 
    blkcnt_t  st_blocks;  // 할당된 블록의 개수 
    time_t    st_atime;   // 마지막 접근 시간 
    time_t    st_mtime;   // 마지막 편집 시간 
    time_t    st_ctime;   // 마지막 상태 변경 시간 
};
switch (sb.st_mode & S_IFMT) {
    case S_IFBLK:
        printf("block device node\n");
        break;
    case S_IFCHR:
        printf("character device node\n");
        break;
    case S_IFDIR:
        printf("directory\n");
        break;
    case S_IFIFO:
        printf("FIFO\n");
        break;
    case S_IFLNK:
        printf("symbolic link\n");
        break;
    case S_IFREG:
        printf("regular file\n");
        break;
    case S_IFSOCK:
        printf("socket\n");
        break;
    default:
        printf("unknown\n");
        break;
}
*/
