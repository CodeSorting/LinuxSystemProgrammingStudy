#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
// 동일한 파일포인터를 참조하는 동일한 역할을 하는 스레드 2개가 동시에 실행되지만 파일락으로 인하여
// 첫번째 스레드가 unlock이된 이후에야 두번째 스레드가 lock을 잡고 작업을 수행한다.
void *mod_file_thread(void *fpp) {
    FILE *fp = (FILE *)fpp;
    char buf[1024];
    printf("<thread start>\n");

    flockfile(fp);
    printf("lock!, data file contents:\n");
    while (fgets(buf,sizeof(buf),fp)) {
        printf("%s",buf);
    }
    printf("now add data \"hello world!\\n\"\n");
    sprintf(buf,"hello world!\n");
    if (fputs(buf,fp)==EOF) {
        perror("fputs error: ");
        pthread_exit(NULL);
    }
    sleep(3);
    printf("finish, unlock!\n");
    funlockfile(fp);

    pthread_exit(NULL);
}

int main(int argc,char *argv[]) {
    FILE *fp;
    pthread_t th1,th2;
    if (argc<2) {
        printf("Usage: %s <data-file>\n",argv[0]);
        return -1;
    }
    fp = fopen(argv[1],"r+");
    if (!fp) {
        perror("fopen error: ");
        return -1;
    }
    
    pthread_create(&th1,NULL,mod_file_thread,(void *)fp);
    pthread_create(&th2,NULL,mod_file_thread,(void *)fp);
    pthread_join(th1,NULL);
    pthread_join(th2,NULL);
}
/*
#include <stdio.h>
void flockfile (FILE *stream);
void funlockfile (FILE *stream);

flockfile() 함수는 stream의 락이 해제될 때까지 기다린 다음 락 카운터를 올리고 락을 얻은 다음 스레드가 stream을 소유하도록 만든 후에 반환한다.
funlockfile() 함수는 stream과 연관된 락 카운터를 하나 줄인다.
만일 락 카운터가 0이 되면 현재 스레드는 stream의 소유권을 포기해서 다른 스레드가 락을 얻을 수 있도록 한다.
*/
