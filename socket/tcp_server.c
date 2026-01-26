#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main() {
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t clnt_addr_size;
    char message[] = "Hello, Socket Programming!";

    // 1. 소켓 생성 (IPv4, TCP)
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);

    // 2. 주소 설정
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 내 IP 주소 자동 할당
    serv_addr.sin_port = htons(9000);              // 포트 번호 9000 (리틀 엔디언을 빅 엔디언으로 변환)

    // 3. 소켓에 주소 할당 (Bind)
    bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    // 4. 연결 요청 대기 상태로 전환 (Listen)
    listen(serv_sock, 5);

    // 5. 연결 요청 수락 (Accept)
    clnt_addr_size = sizeof(clnt_addr);
    clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);

    // 6. 데이터 전송
    write(clnt_sock, message, sizeof(message));

    // 7. 소켓 닫기
    close(clnt_sock);
    close(serv_sock);
    return 0;
}
