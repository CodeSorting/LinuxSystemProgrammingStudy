#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main() {
    int sock;
    struct sockaddr_in serv_addr;
    char buffer[1024];

    // 1. 소켓 생성
    sock = socket(PF_INET, SOCK_STREAM, 0);

    // 2. 연결할 서버 주소 설정
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // 로컬 호스트 주소
    serv_addr.sin_port = htons(9000);

    // 3. 서버에 연결 요청 (Connect)
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        printf("Connection failed\n");
        return -1;
    }

    // 4. 데이터 수신
    read(sock, buffer, sizeof(buffer)-1);
    printf("Message from server: %s\n", buffer);

    // 5. 소켓 닫기
    close(sock);
    return 0;
}
