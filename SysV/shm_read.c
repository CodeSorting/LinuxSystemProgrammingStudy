#include <stdio.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>

int main() {
    key_t key;
    int shmid;
    char *str;

    key = ftok("shmtest",1234);
    shmid = shmget(key,1024,IPC_CREAT|0666);
    str = (char *)shmat(shmid,(void *)0,0);
    
    printf("read data: %s\n",str);
    
    shmdt(str);
    shmctl(shmid,IPC_RMID,NULL);
    
    return 0;
}
