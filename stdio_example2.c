#include <stdio.h>
int main(int argc, char *argv[]) {
    FILE *fp;
    int c;
    if (argc < 2) {
        printf("Usage: %s <file>\n", argv[0]);
        return -1;
    }
    fp = fopen(argv[1], "r+"); //읽기 쓰기 모두 가능
    if (!fp) {
        perror("fopen error: ");
        return -1;
    }
    while ((c = fgetc(fp)) != EOF) {
        //필터링
        if (c == '!' || c == '@' || c == '#' || c == '$' || c == '%' || c == '^' || c == '&' || c == '*' ||
        c == '-' || c == '_') {
            if (ungetc(c, fp) == EOF) { //읽었던 문자 c를 다시 스트림에 밀어 넣는다. 결과적으로 파일 포인터가 방금 위치로 돌아간다.
                perror("ungetc error: ");
                return -1;
            }
            if (fputc(' ', fp) == EOF) { //돌아간 자리에 공백 쓰기
                perror("fputc error: ");
                return -1;
            }
        }
    }
    if (fclose(fp) == EOF) {
        perror("fclose error: ");
        return -1;
    }
    return 0;
}
/*
사용자 버퍼 입출력

사용자 버퍼 입출력은 블록 단위로 동작하는 파일 시스템과 추상 개념을 갖고 있는 애플리케이션 간의 간극을 좁혀준다.
데이터가 쓰여지면 프로그램 주소 공간 내 버퍼에 저장된다.
버퍼가 특정 크기(버퍼 크기)에 도달하면 전체 버퍼는 한 번의 쓰기 연산을 통해 실제로 기록이 된다.
읽기 역시 마찬가지로 버퍼 크기에 맞춰 블록에 정렬된 데이터를 읽는다.
그 결과 데이터가 많더라도 모두 블록 크기에 맞춰 적은 횟수의 시스템 콜만 사용하게 되며 이를 통해 성능 향상을 얻을 수 있다.

표준 C 라이브러리가 제공하는 stdio 라이브러리는 플랫폼 독립적인 사용자 버퍼링 해법을 제공한다.

#include <stdio.h>
FILE * fopen (const char *path, const char *mode);

표준 입출력 함수들은 파일 디스크립터를 직접 다루지 않는 대신 파일 포인터라는 식별자를 사용한다.
파일 포인터는 C 라이브러리 내부에서 파일 디스크립터로 매핑된다.
파일 포인터는 <stdio.h>에 정의된 FILE typedef를 가리키는 포인터이다.
path 인자는 파일 경로를 의미하며, mode 인자는 주어진 파일을 어떻게 열지 기술한다.

- r / r+ : r(읽기) / r+(읽기, 쓰기) 목적으로 파일을 연다.
- w / w+ : w(쓰기) / w+(읽기, 쓰기) 목적으로 파일을 연다. 파일이 이미 존재하면 길이를 0으로 잘라버린다. 파일이 존재하지 않으면 새로 만든다.
- a / a+ : 덧붙이기 상태에서 a(쓰기) / a+(읽기, 쓰기) 목적으로 파일을 연다. 파일이 존재하지 않으면 새로 만든다. 스트림(위치)은 파일 끝 지점에 위치한다. 쓰기는 파일 끝에서부터 진행된다.

성공 시 유효한 FILE 포인터를 반환한다.
실패 시 NULL을 반환하고 errno를 적절한 값으로 설정한다.

#include <stdio.h>
int fclose (FILE *stream);

fclose()는 fopen 등을 통해 열린 파일 포인터(스트림)를 닫는다.
버퍼에 쌓여있지만 아직 스트림에 쓰지 않은 데이터는 먼저 처리한 후 닫는다.
성공하면 0 리턴, 실패 시 EOF 리턴하고 errno 설정.

#include <stdio.h>
FILE * fdopen (int fd, const char *mode);

fdopen() 함수는 이미 열린 파일 디스크립터를 통해 파일 포인터(스트림)를 생성한다.
mode 인자는 fopen()과 동일하며, 원래 파일 디스크립터를 열 때 사용했던 모드와 호환성을 유지해야 한다.
(w/w+ 모드일 경우 이미 파일이 존재하더라도 0으로 초기화되지 않는다.)
스트림은 파일 디스크립터가 가리키는 위치에서 시작한다.
스트림을 닫을 경우(fclose) 파일 디스크립터도 닫힌다(close).
성공 시 유효한 FILE 포인터를 반환한다.
실패 시 NULL을 반환하고 errno를 적절한 값으로 설정한다.

#include <stdio.h>
int fgetc (FILE *stream);

stream에서 다음 문자를 읽고 int 타입으로 변환해서 반환한다.
에러 발생 시 EOF 리턴(errno).

int fputc (int c, FILE *stream);

int 타입으로 변환한 한 문자를 stream에 쓴다.
쓰기 성공하면 c를 반환하고, 에러 발생 시 EOF 리턴(errno).
*/
