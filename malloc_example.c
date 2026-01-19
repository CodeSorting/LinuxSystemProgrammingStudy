#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
struct information {
    int index;
    char name[32];
    char phone_number[32];
    char mobile_number[32];
    char address[128];
};
/* addr_book: memory for store address information  */
/* memidx: start index of addr_book array */
/* pidx: start index of person's index */
/* pnum: count of input person */
void input_person_information(struct information *addr_book, int memidx, int pidx, int pnum);
void print_person_information(struct information *addr_book, int pnum);

/*
1) 주소록에는 이름, 연락처, 주소등의 정보가 포함된다.
2) 주소록에 몇명의 사람이 기록되는지 입력을 받는다. (malloc, calloc)
3) 각각의 사람에 대한 정보를 입력을 받는다.
4) 입력완료 후 더 추가할 사람이 없는 지 물어보고 있다면 입력을 받는다. (realloc)
5) 전체 완성된 주소록을 출력하고, 수정할 사람이 있는지 물어본다.
6) 수정할 사람이 있다면 해당사람의 정보를 다시 입력받는다. (memset)
7) 새로 입력받은 정보와 기존주소록 정보를 비교하여 다를경우 주소록에 기록하고 출력한다. (memcmp, memmove)
8) 종료한다.

*/
int main() {
    int nperson;
    int i;
    struct information *addr,newaddr;
    char yn[2];
    
    /* STEP 1 - input address book */
    printf("Input total number of person: ");
    scanf("%d", &nperson);
    if (nperson <= 0) {
        printf("Please input at least one person!\n");
        return 0;
    }
    addr = (struct information *)calloc(nperson, sizeof(struct information));
    if (!addr) {
        perror("calloc error: ");
        return -1;
    }
    input_person_information(addr, 0, 1, nperson);
    
    /* STEP 2 - add address */
    printf("\nanyone else? [y/n] ");
    scanf("%s",yn);
    if (yn[0] == 'y' || yn[1] == 'Y') {
        int addnum;
        printf("Input add number of person: ");
        scanf("%d", &addnum);
        if (addnum > 0) {
            addr = (struct information *)realloc(addr, (nperson + addnum) * sizeof(struct information));
            if (!addr) {
                perror("realloc error: ");
                return -1;
            }
            input_person_information(addr, nperson, nperson+1, addnum);
            nperson += addnum;
        }
    }
    printf("\n< print original address book >\n");
    print_person_information(addr, nperson);

    /* STEP 3 - change address */
    printf("\nanyone change? [y/n] ");
    scanf("%s",yn);
    if (yn[0] == 'y' || yn[0] == 'Y') {
        int cidx;
        printf("Input change index: ");
        scanf("%d", &cidx);
        if (cidx <= 0 || cidx > nperson) {
            printf("There's only %d persons\n", nperson);
            return 0;
        }
        printf("\n< change first person's information >\n");
        memset(&newaddr, 0, sizeof(struct information));
        input_person_information(&newaddr, 0, cidx, 1);
        if (memcmp(&addr[cidx-1], &newaddr, sizeof(struct information))) {
            memmove(&addr[cidx-1], &newaddr, sizeof(struct information));
            printf("\n< print new address book >\n");
            print_person_information(addr, nperson);
        } else
            printf("\nThe information is same!\n");
    }
    printf("finish!\n");
    free(addr);
    return 0;
}

void input_person_information(struct information *addr_book, int memidx, int pidx, int pnum) {
    for (int i = 0; i < pnum; i++) {
        struct information *pinfo = &addr_book[memidx+i];
        printf("\nInput %d person's information.\n", pidx+i);
        pinfo->index = pidx+i;
        printf("Input name: ");
        scanf("%s", pinfo->name);
        printf("Input phone number: ");
        scanf("%s", pinfo->phone_number);
        printf("Input mobile number: ");
        scanf("%s", pinfo->mobile_number);
        printf("Input address: ");
        scanf("%s", pinfo->address);
    }
}

void print_person_information(struct information *addr_book, int pnum) {
    for (int i = 0; i < pnum; i++) {
        struct information *pinfo = &addr_book[i];
        printf("[%d]\n", pinfo->index);
        printf("\tName: %s\n", pinfo->name);
        printf("\tPhone: %s\n", pinfo->phone_number);
        printf("\tMobile: %s\n", pinfo->mobile_number);
        printf("\tAddress: %s\n", pinfo->address);
    }
}
/*
가상 메모리 (Virtual Memory)
리눅스 운영체제는 물리적인 메모리를 가상화하여 프로세스에서 직접 물리 메모리 주소에 접근하지 않고, 커널이 프로세스에게 독립적인 가상의 주소 공간을 제공한다. 실제 시스템에 존재하는 물리 메모리 크기와 관계없이 가상 주소 공간을 프로세스에게 제공한다.
- 32bit 시스템 -> 2^32 크기 (4GB)
- 64bit 시스템 -> 2^64 크기 제공

페이지 (Page)
페이지는 메모리 관리 유닛(MMU)에서 관리할 수 있는 최소 단위이다.
$ getconf PAGESIZE
4096

참고: struct page (http://lxr.linux.no/linux+v3.3.4/include/linux/mm_types.h#L40)

메모리 영역 (Memory Layout)
- 텍스트 세그먼트 (text): 프로세스의 프로그램 코드, 문자열 상수, 상수 변수, 읽기 전용 데이터 등으로 구성
- 초기화된 데이터 세그먼트 (data): 프로그래머가 초기화한 전역 변수와 정적 변수 포함
- 초기화되지 않은 데이터 세그먼트 (bss): 초기화되지 않은 전역 변수 및 정적 변수 포함 (ex. static int a; 로 선언된 변수, int a; 로 선언된 전역 변수)
- 힙 (heap): 동적 메모리 할당 (ex. malloc)
- 스택 (stack): 프로세스의 지역 변수와 함수 반환 데이터 등으로 구성


프로그램에서 메모리를 할당할 때 배열 등을 활용하여 자동 변수나 정적 변수의 형태로 사용되지만, 프로그램 실행 전에 메모리가 얼마만큼 필요한지 정해지지 않은 경우, 즉 컴파일 시점이 아니라 실행 중에 메모리가 할당되는 동적 메모리를 사용할 수 있다.

#include <stdlib.h>
void *malloc(size_t size);
malloc() 함수가 성공하면 size 바이트만큼 메모리를 할당해서 그 시작점을 가리키는 포인터를 리턴한다. (할당된 메모리는 초기화되어 있지 않음. 즉, 0으로 초기화되어 있지 않음에 주의)
실패 시 NULL을 리턴하고 errno(ENOMEM) 설정한다.

#define BUFLEN 1024
char *buf;
buf = (char *)malloc(BUFLEN);
if (!buf)
    perror("malloc error: ");

#include <stdlib.h>
// 동적 메모리 할당 
void *calloc(size_t nr, size_t size);
calloc() 함수는 size 바이트 크기의 원소를 nr 개만큼 메모리를 할당한다. (즉, size * nr = 할당 크기)
malloc() 함수와 달리 calloc()의 경우 할당된 메모리를 0으로 초기화한다. 0 값으로 초기화가 필요한 동적 메모리 할당의 경우, malloc()으로 할당 후 memset() 등을 이용해 0 값으로 초기화하는 것보다 calloc()을 이용하는 것이 더 빠르다.

#define BUFLEN 1024
char *buf;
buf = (char *)calloc(1, BUFLEN);
if (!buf)
    perror("calloc error: ");

// 동적 메모리 크기 변경 
#include <stdlib.h>
void *realloc(void *ptr, size_t size);
realloc() 함수가 성공하면 ptr이 가리키는 메모리 영역을 size 바이트 크기로 새로 변경한다. 새로운 크기의 메모리 영역을 리턴하며 ptr과 동일한 포인터가 아닐 수 있으므로 주의해야 한다.
메모리를 확장하는 경우, 즉 realloc은 ptr 포인터가 가리키는 메모리 주소 공간을 size 크기만큼 확장할 수 없는 경우, size 크기만큼 새로운 메모리 공간을 할당하고 기존 ptr이 가리키는 공간의 내용을 복사한 후 이전 영역을 해제한다.

#include <stdio.h>
#include <stdlib.h>
#define BUFLEN 12
int main()
{
    char *buf;
    buf = (char *)malloc(BUFLEN);
    if (!buf) {
        perror("malloc error: ");
        return -1;
    }
    sprintf(buf, "LinuxSystem");
    buf = (char *)realloc(buf, BUFLEN + 11);
    if (!buf) {
        perror("realloc error: ");
        return -1;
    }
    sprintf(buf + BUFLEN - 1, "Programming");
    printf("final buffer = %s\n", buf);
    free(buf);
    return 0;
}

// 동적 메모리 해제 
#include <stdlib.h>
void free(void *ptr);

malloc(), calloc(), realloc()을 통해 동적으로 할당된 메모리는 사용 후 free()를 통해 시스템에 메모리를 반환해야 한다. 동적으로 할당된 메모리를 반환하지 않은 경우 해당 프로세스 주소 공간에 계속 남아있다. 만약 동적으로 메모리를 할당한 프로세스가 일시적인 프로세스가 아니라 시스템의 백그라운드에서 계속 운용되는 상주 프로세스라면, free()를 통해 메모리를 반환하지 않는 행위가 누적될 경우 점점 더 많은 메모리를 점유하여 OOM(Out Of Memory)과 같은 시스템 문제를 야기할 수 있다. (Memory leak)

OOM (Out Of Memory)
리눅스 운영체제에서 시스템 메모리가 부족할 때 OOM 킬러라는 이름의 매커니즘을 이용하여 하나 이상의 프로세스를 강제 종료함으로써 메모리 부족 상황을 복구하려고 시도한다.

메모리 조작
#include <string.h>
void *memset(void *s, int c, size_t n);
memset()은 s가 가리키는 메모리 주소 공간을 n 바이트만큼 c 바이트로 채운다. 일반적으로 0으로 초기화할 때 많이 사용된다. (ex. memset(buf, 0, sizeof(buf));)

int memcmp(const void *s1, const void *s2, size_t n);
memcmp()는 s1이 가리키는 메모리 영역과 s2가 가리키는 메모리 영역의 처음 n 바이트를 비교한다. 같으면 0 리턴, s1이 s2보다 작다면 음수 리턴, 크다면 양수 리턴한다.

void *memmove(void *dst, const void *src, size_t n);
memmove()는 src가 가리키는 처음 n 바이트를 dst로 복사하고 dst를 리턴한다. dst와 src가 중첩되더라도(dst의 일부가 src 안에 존재) 문제가 없다.

void *memcpy(void *dst, const void *src, size_t n);
memcpy()는 src가 가리키는 처음 n 바이트를 dst에 복사하고 dst를 리턴한다. memmove()와 동일하지만 dst와 src가 중첩되지 않아야 한다. 즉, 중첩된다면 결과를 보장할 수 없다.
*/
