#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

/* 1. 기본 시그널 핸들러 함수 */
void basic_handler(int signo) {
    if (signo == SIGINT) {
        printf("\n[Handler] SIGINT(Ctrl+C) 수신! 프로그램을 계속 실행합니다.\n");
    }
}

/* 2. sigaction을 위한 고급 핸들러 함수 (SA_SIGINFO 사용) */
void advanced_handler(int signo, siginfo_t *si, void *ucontext) {
    printf("\n[Advanced Handler] 시그널 번호: %d\n", signo);
    printf("시그널을 보낸 프로세스 PID: %d\n", si->si_pid);
    
    if (si->si_code == SI_USER) {
        printf("시그널 발생 원인: 사용자 전송(kill 또는 raise)\n");
    }
    
    // sigqueue로 보낸 데이터가 있다면 출력
    printf("전달된 데이터(Value): %d\n", si->si_value.sival_int);
}

int main() {
    struct sigaction sa;
    sigset_t set, oldset;

    printf("PID: %d - 시그널 실습을 시작합니다.\n", getpid());

    // --- [시그널 등록: signal() 방식] ---
    // SIGINT를 basic_handler에 연결
    if (signal(SIGINT, basic_handler) == SIG_ERR) {
        perror("signal registration fail");
        return -1;
    }

    // SIGTSTP(Ctrl+Z)는 무시하도록 설정
    signal(SIGTSTP, SIG_IGN);
    printf("1. SIGINT는 핸들러 등록됨, SIGTSTP(Ctrl+Z)는 무시(SIG_IGN) 설정됨.\n");


    // --- [시그널 차단: sigprocmask() 방식] ---
    sigemptyset(&set);
    sigaddset(&set, SIGINT); // SIGINT를 차단 목록에 추가
    
    printf("2. 5초간 SIGINT를 차단(Block)합니다. Ctrl+C를 눌러보세요.\n");
    sigprocmask(SIG_BLOCK, &set, &oldset); // 차단 시작
    
    sleep(5);
    
    printf("3. 차단을 해제(Unblock)합니다. 보류되었던 시그널이 전달됩니다.\n");
    sigprocmask(SIG_SETMASK, &oldset, NULL); // 이전 상태로 복구 (차단 해제)


    // --- [시그널 전송: raise() 및 kill()] ---
    printf("4. raise()를 통해 자신에게 SIGINT를 보냅니다.\n");
    raise(SIGINT);


    // --- [시그널 고급 관리: sigaction() 방식] ---
    sa.sa_sigaction = advanced_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO; // 상세 정보를 받기 위한 플래그

    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction fail");
        return -1;
    }

    // --- [데이터와 함께 시그널 전송: sigqueue()] ---
    printf("5. sigqueue()를 통해 데이터를 담아 SIGUSR1을 보냅니다.\n");
    union sigval value;
    value.sival_int = 2024; // 보낼 데이터
    sigqueue(getpid(), SIGUSR1, value);

    printf("6. 프로그램을 종료합니다.\n");
    return 0;
}

/* ============================================================
 * [시그널(Signal) 전체 내용 정리]
 * ============================================================
 * 1. 개념:
 * - 유닉스 계열 운영체제에서 프로세스 간 통신(IPC)의 제한된 형태.
 * - 비동기적으로 이벤트를 알리는 알림 메시지.
 * - 시그널 발생 시 커널은 프로세스의 실행 흐름을 일시 중단하고 핸들러를 실행.
 * - kill -l을 통해 전체 시그널 목록 확인 가능
 *
 * 2. 시그널 전송 방식:
 * - kill(pid, signo): 특정 프로세스(그룹)에 시그널 전송.
 * - raise(signo): 호출한 프로세스 자신에게 시그널 전송.
 * - sigqueue(pid, signo, value): 데이터(페이로드)를 포함하여 시그널 전송.
 * - 키보드 입력: Ctrl+C(SIGINT), Ctrl+\(SIGQUIT), Ctrl+Z(SIGTSTP).
 *
 * 3. 시그널 처리:
 * - SIG_DFL: 기본 동작 수행 (보통 종료).
 * - SIG_IGN: 시그널 무시.
 * - 시그널 핸들러: 사용자가 정의한 함수를 실행.
 * - *가로채거나 무시할 수 없는 시그널: SIGKILL, SIGSTOP.
 *
 * 4. 시그널 차단(Masking):
 * - sigprocmask()를 사용하여 임계 영역(Critical Section) 실행 중 시그널 수신을 보류 가능.
 * - 차단된 시그널은 해제될 때까지 전달되지 않고 대기함.
 *
 * 5. 고급 관리(sigaction):
 * - signal() 함수보다 정교한 제어 가능.
 * - sa_mask를 통해 핸들러 실행 중 다른 시그널 차단 설정 가능.
 * - SA_SIGINFO 플래그 사용 시 siginfo_t 구조체를 통해 시그널 발생 원인, 
 * 보낸 프로세스의 PID, 전달된 데이터 등을 확인 가능.
 * ============================================================
 */
 /*
gpt 보충
프로세스가 시그널을 받았을 때 선택할 수 있는 행동은 크게 3가지입니다.
1. 기본 동작(Default): 운영체제가 정해놓은 대로 행동합니다. (대부분 종료)
2. 무시(Ignore): 시그널이 와도 아무 일 없다는 듯이 계속 자기 할 일을 합니다. (SIG_IGN)
3. 직접 처리(Handler): 내가 미리 만든 함수(핸들러)를 실행합니다. (signal() 또는 sigaction() 사용)
⚠️ 
예외 사항: SIGKILL(강제 종료)과 SIGSTOP(강제 정지)은 절대로 무시하거나 가로챌 수 없습니다. 관리자의 최후의 수단이기 때문입니다.

3. 시그널은 어떻게 보내나요? (전송 방법)
키보드로 전송:
Ctrl + C: SIGINT (프로세스 종료)
Ctrl + Z: SIGTSTP (프로세스 일시 정지)

터미널 명령어로 전송:
kill -l: 사용 가능한 시그널 목록 확인
kill -9 [PID]: 특정 프로세스를 강제 종료(SIGKILL)

코드 내부에서 전송:
kill(pid, signo): 특정 프로세스에게 전송
raise(signo): 나 자신에게 전송

4. 시그널 차단(Masking)은 왜 필요할까요?
프로그램이 아주 중요한 데이터를 저장하고 있는데, 중간에 시그널이 와서 프로그램이 멈춰버리면 데이터가 꼬일 수 있겠죠?
1. 임계 영역(Critical Section): 방해받으면 안 되는 코드 구간.
2. 차단(Block): 시그널을 '삭제'하는 게 아니라, 중요한 일이 끝날 때까지 '잠시 보류'해두는 것입니다. 일이 끝나고 차단을 풀면 밀려있던 시그널이 한꺼번에 전달됩니다.

함수: sigprocmask()를 사용해 차단 목록을 관리합니다.

5. 더 똑똑하게 관리하기 (sigaction)
옛날 방식인 signal() 함수는 단순하지만 기능이 부족했습니다. 그래서 현대 리눅스에서는 sigaction()을 권장합니다.
1. 더 안전함: 시그널 핸들러가 실행되는 동안 또 다른 시그널이 들어오는 것을 정교하게 막을 수 있습니다.
2. 정보 확인: SA_SIGINFO를 쓰면 "누가 나한테 이 시그널을 보냈는지(PID)", "어떤 추가 데이터가 들어왔는지"까지 알 수 있습니다.
데이터 전송: sigqueue()를 쓰면 시그널과 함께 숫자 데이터도 보낼 수 있습니다.
*/
