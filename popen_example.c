#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *fp;
    int status;
    char buff[1024];
    fp = popen("pwd","r");
    if (fp==NULL) {
        fprintf(stderr,"popen error: %m\n");
        exit(1);
    }
    printf("Current Path is ");
    while (fgets(buff,sizeof(buff),fp)!=NULL) {
        printf("%s",buff);
    }
    status = pclose(fp);
    if (status==-1) {
        fprintf(stderr,"pclose error: %m\n");
        exit(1);
    }
    return 0;
}
/*
익명파이프(Pipe)
#include <stdio.h>

FILE *popen(const char *command, const char *type);
int pclose(FILE *stream);

popen() 함수는 단방향 파이프를 생성하고 fork 후 쉘을 호출하여 프로세스를 open 합니다.
type 인수는 fopen의 mode 인수와 비슷하나 단방향 이므로 읽기("r") 또는 쓰기("w")만 지정할 수 있습니다.
popen()으로 생성한 파일포인터는 pclose()로 닫아야 합니다. (fclose가아님에유의)
command 인수는 NULL로 종료되는 문자열이며 -c 플래그를 사용하여 /bin/sh에 전달됩니다.
popen()은 정상종료시 파일포인터를 리턴하며, 실패시 NULL 리턴후 errno 설정
pclose()는 정상종료시 command의 exit 상태값을 리턴하며, 실패시-1 리턴후 errno 설정
*/
