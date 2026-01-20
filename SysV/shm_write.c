#include <stdio.h>
#include <stdlib.h>
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
    
    printf("input data: ");
    if (fgets(str,1024,stdin)==NULL) {
        perror("fgets erro: ");
        exit(1);
    }
    printf("Written data: %s\n",str);
    
    shmdt(str);

    return 0;
}

/*
공유 메모리 개요

공유 메모리란?
IPC의 관점에서 공유 메모리란, 여러 프로세스가 동일한 메모리 공간을 공유함으로써 프로세스 사이에서 해당 공유 메모리를 통해 데이터를 공유할 수 있는 메커니즘이다.

장점
주소 번지에 직접 액세스하므로 속도가 빠르다.

단점
여러 프로세스나 스레드가 공유 메모리에 직접 접근하게 되므로 크리티컬 섹션에서 데이터 오염 등 문제가 발생하기 쉬우므로 락 메커니즘 등으로 보호해 주는 노력 필요

공유 메모리는 구현상 크게 두 가지 방식으로 나누어진다.
전통적인 SysV 형식과 POSIX 표준 형식으로 나누어진다.
두 방식의 차이점은 SysV 방식의 경우 공유 메모리의 IPC ID를 통해 공유 메모리를 접근, 관리하고 POSIX 방식은 파일을 생성하여 공유 메모리와 매핑한 후 공유 메모리도 파일처럼 인식되게 사용하는 것이다. (파일 디스크립터 사용)

참고로 SysV IPC 자원들을 확인할 수 있는 ipcs(IPC Status)와 같은 커맨드로는 POSIX 방식은 보이지 않습니다. POSIX 방식은 /dev/shm 디렉토리 아래에 파일을 생성하고 관리합니다.

SysV IPC들은 IPC Key, IPC ID, 소유권의 속성을 가지고 있다.
IPC Key는 IPC 자원에 접근하기 위해 사용하는 키 역할을 하며 이를 이용해 IPC ID 값을 얻어오는 구조이다.

공유 메모리 SysV 공유 메모리 함수 호출 순서
1단계: IPC Key 획득
Key = ftok (조건...)
2단계: IPC ID 획득(get)
ID = shmget (Key...)
3단계: 공유 메모리 연결(attach)
shmat (ID...)
4단계: 공유 메모리 조작(control)
shmctl (ID...)
5단계: 공유 메모리 해제(detach)
shmdt (ID...)

#include <sys/types.h>
#include <sys/ipc.h>

key_t ftok(const char *pathname, int proj_id);
ftok() 함수는 경로명과 프로젝트 식별자를 System V IPC 키로 변환합니다.
ftok() 함수는 주어진 경로 이름(기존의 액세스 가능한 파일을 참조해야 함)으로 이름이 지정된 파일의 ID와 proj_id의 최하위 8비트(0이 아니어야 함)를 사용하여 key_t 유형 System V IPC 키를 생성합니다.
성공하면 생성된 key_t 값이 반환됩니다.
실패 시 -1이 리턴되며 errno는 stat(2) 시스템 호출과 관련된 오류를 나타냅니다.

key_t shmkey;
if ((shmkey = ftok("/tmp", 'a')) == (key_t) -1) {
    perror("IPC error: ftok");
    exit(1);
}

#include <sys/ipc.h>
#include <sys/shm.h>

int shmget(key_t key, size_t size, int shmflg);
shmget() 함수는 인수 키 값과 연관된 SysV 공유 메모리 세그먼트의 ID를 리턴합니다.
키 값이 IPC_PRIVATE이거나 키에 해당하는 공유 메모리 세그먼트가 없고 shmflg에 IPC_CREAT가 지정된 경우 size 크기의 새 공유 메모리가 생성됩니다.
IPC_PRIVATE 매크로는 공유하지 않는 임시 IPC, 즉 사설 IPC의 의미를 나타냅니다.
shmflg가 IPC_CREAT와 IPC_EXCL을 모두 지정하고 키에 대한 공유 메모리가 이미 존재하면 errno가 EEXIST로 설정되며 shmget()이 실패합니다.

shmflg
- IPC_CREAT: 새로운 세그먼트를 생성, 이 플래그가 없다면 키와 연관된 세그먼트를 찾음
- IPC_EXCL: IPC_CREAT 플래그와 함께 사용, 세그먼트가 이미 존재하면 에러(EEXIST) 발생

위 플래그 외에도 shmflg의 하위 9비트는 소유자, 그룹, 기타에게 부여된 권한을 지정합니다.
이 비트는 open()의 모드 인수와 동일한 형식과 의미를 갖습니다. (실행 권한은 사용하지 않음)
ex) shm_id = shmget(IPC_PRIVATE, 4*sizeof(int), IPC_CREAT | 0666);

shmget으로 획득한 공유 메모리에 접근하기 위해서는 가상 메모리 주소에 매핑해야 합니다.
이 매핑 과정을 연결(attach)해 주는 함수가 shmat이고 연결을 해제하는 함수가 shmdt입니다.

#include <sys/types.h>
#include <sys/shm.h>

void *shmat(int shmid, const void *shmaddr, int shmflg);
int shmdt(const void *shmaddr);

shmat() 함수는 shmid로 식별된 SysV 공유 메모리 세그먼트를 호출 프로세스의 주소 공간에 연결합니다.
shmaddr는 attach 되는 어드레스 주소를 의미하며 일반적으로 NULL을 지정합니다.
shmflg는 동작 옵션을 나타내며
• SHM_RDONLY: 공유 메모리를 읽기 전용으로 첨부
• SHM_RND: shmaddr이 NULL이 아닌 경우, shmaddr은 반드시 페이지 정렬된(page aligned) 주소여야 함. SHM_RND 사용 시 자동으로 메모리 페이지 경계에 맞춰줌
성공 시 첨부된 공유 메모리 세그먼트의 주소가 리턴, 실패 시 (void *)-1이 리턴되고 errno 설정

shmdt() 함수는 shmaddr에 의해 지정된 주소에 있는 공유 메모리 세그먼트를 호출 프로세스의 주소 공간에서 분리합니다. 분리될 세그먼트는 shmat() 호출에 의해 리턴된 값을 인자로 호출합니다.
성공 시 0이 리턴, 실패 시 -1이 리턴되고 errno 설정

#include <sys/ipc.h>
#include <sys/shm.h>

int shmctl(int shmid, int cmd, struct shmid_ds *buf);
shmctl()은 식별자가 shmid로 지정된 SysV 공유 메모리 세그먼트에서 cmd로 지정된 제어 조작을 수행합니다.

cmd
- IPC_STAT: IPC 자원의 정보(생성자, 생성 시간, 접근 권한 등)를 읽어온다.
- IPC_SET: IPC 자원의 정보(권한 등)를 변경한다.
- IPC_RMID: IPC 자원을 제거한다.
- IPC_INFO: IPC 자원의 시스템 설정값을 읽어온다.

struct shmid_ds {
    // <sys/shm.h> 
    struct ipc_perm shm_perm;     Ownership and permissions 
    size_t          shm_segsz;    Size of segment (bytes) 
    time_t          shm_atime;    Last attach time 
    time_t          shm_dtime;    Last detach time 
    time_t          shm_ctime;    Last change time 
    pid_t           shm_cpid;     PID of creator 
    pid_t           shm_lpid;     PID of last shmat(2)/shmdt(2) 
    shmatt_t        shm_nattch;   No. of current attaches 
    ...
};
*/
