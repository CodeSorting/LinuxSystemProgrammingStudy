#include <stdio.h>

#define FILENAME "./data.file"

int main() {
    FILE *readfile, *writefile;
    struct address_info { //이름, 전화번호, 주소
        unsigned int index;
        char name[32];
        char phone_number[16];
        char address[100];
    };
    struct address_info kim, who; //kim 만들기
    kim.index = 1;
    sprintf(kim.name, "Kim");
    sprintf(kim.phone_number, "010-1234-5678");
    sprintf(kim.address, "Seoul");
    
    writefile = fopen(FILENAME, "wb"); //쓰기 바이너리 모드 : 배열이니 빈 공간(NULL)이 발생한다. cat으로 출력할 때 일부부분이 깨져서 보이는게 당연함.
    if (!writefile){
        perror("fopen error : ");
        return -1;
    }
    if (!fwrite(&kim, sizeof(struct address_info), 1, writefile)) { //구조체 만큼 옮김.
        perror("fwriteerror: ");
        return -1;
    }
    if (fclose(writefile)) { //닫기
        perror("fcloseerror: ");
        return -1;
    }
    readfile= fopen(FILENAME, "rb"); //읽기 바이너리 모드
    if (!readfile) {
        perror("fopenerror: ");
        return -1;
    }
    if (!fread(&who, sizeof(struct address_info), 1, readfile)) { //구조체만큼 읽기
        perror("freaderror: ");
        return -1;
    }
    if (fclose(readfile)) { //닫기
        perror("fcloseerror: ");
        return -1;
    }
    printf("index: %d\n", who.index); //결과값 출력
    printf("name: %s\n", who.name);
    printf("phone number: %s\n", who.phone_number);
    printf("address: %s\n", who.address);
    return 0;
}
/*
#include <stdio.h>
char * fgets (char *str, int size, FILE *stream);

fgets()는 스트림에서 한 문자열을 읽는다.
stream에서 size보다 한 문자가 적은 내용을 읽어서 str에 저장하고, 버퍼 마지막에 문자열의 끝을 나타내는 null 문자(\0)를 저장한다.
EOF나 개행 문자를 만나면 읽기를 중단한다.
성공하면 str를 반환하고 실패하면 NULL을 리턴.

int fputs (const char *str, FILE *stream);

fputs()는 스트림에 문자열을 기록한다.
str이 가리키는 NULL로 끝나는 문자열 전부를 stream이 가리키는 스트림에 기록한다.
성공하면 쓴 크기를 반환하고, 실패하면 EOF를 반환한다.

#include <stdio.h>
size_t fread (void *buf, size_t size, size_t nr, FILE *stream);

fread()는 stream에서 크기가 size 바이트인 요소를 nr개 읽어서 buf가 가리키는 버퍼에 저장한다.
즉 원하는 총 읽는 바이트 수는 size * nr 바이트가 된다.
주의할 점은 읽은 바이트 수가 리턴되는 것이 아니라 읽어들인 요소의 수(nr)가 리턴된다.
즉 nr보다 적은 값을 반환하여 실패나 EOF를 알려준다.

size_t fwrite (void *buf, size_t size, size_t nr, FILE *stream);

fwrite()는 buf가 가리키는 데이터에서 size 크기의 요소 nr개를 stream에 쓴다.
즉 쓰기 원하는 총 바이트 수는 size * nr 바이트가 된다.
fread와 마찬가지로 쓴 요소의 수 nr가 리턴된다.
nr보다 작은 리턴 값은 실패를 나타낸다.
*/
