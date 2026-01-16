#include <stdio.h>
#include <unistd.h>

int main() {
    char *pathname = "./a.out";
    int mode = R_OK | W_OK;
    if (access(pathname,mode)==0) {
        printf("읽고 쓰기 가능!\n");
    } else {
        printf("권한이 없거나 존재하지 않음.\n");
    }
    return 0;
}
/*
#include <unistd.h>
int access(const char *pathname, int mode);
access ()는 호출 프로세스가 파일경로 이름에 액세스할 수 있는지 확인합니다.
모드는 수행할 접근성 검사지정
F_OK: 파일 존재여부
R_OK: 파일 존재여부, 읽기권한
W_OK: 파일 존재여부, 쓰기권한
X_OK: 파일 존재여부, 실행권한
정상수행시 파일 0 리턴, 에러발생시 -1 리턴(errno)
*/
