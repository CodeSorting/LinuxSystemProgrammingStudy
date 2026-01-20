#include <stdio.h>
#include <unistd.h>
#include <time.h>

int main() {
    time_t t = time(NULL);
    struct tm *tm1,*tm2;
    tm1 = gmtime(&t);
    tm2 = localtime(&t);
    if (!tm1 || !tm2) {
        perror("gtime, localtime error: ");
        return -1;
    }
    // 두 함수가 내부적으로 동일한 정적 메모리 버퍼를 공유하기 때문에 결과가 동일하다.
    printf("gmtime : %d년 %d월 %d일 %d:%d:%d\n",tm1->tm_year+1900,tm1->tm_mon+1,tm1->tm_mday,tm1->tm_hour,tm1->tm_min,tm1->tm_sec);
    printf("localtime : %d년 %d월 %d일 %d:%d:%d\n",tm2->tm_year+1900,tm2->tm_mon+1,tm2->tm_mday,tm2->tm_hour,tm2->tm_min,tm2->tm_sec);
    
    // gtime_r 및 localtime_r 사용이 좋다. 버퍼를 직접 전달하는 함수다.
    struct tm tim1,tim2;
    gmtime_r(&t,&tim1);
    localtime_r(&t,&tim2);

    printf("gmtime : %d년 %d월 %d일 %d:%d:%d\n",tim1.tm_year+1900,tim1.tm_mon+1,tim1.tm_mday,tim1.tm_hour,tim1.tm_min,tim1.tm_sec);
    printf("localtime : %d년 %d월 %d일 %d:%d:%d\n",tim2.tm_year+1900,tim2.tm_mon+1,tim2.tm_mday,tim2.tm_hour,tim2.tm_min,tim2.tm_sec);
    return 0;
}
/*
프로그래밍을 하다 보면 시간에 대해 중요해질 때가 많습니다.

01 시간 개요
예를 들어 현재 시간을 획득하여 로그와 같은 정보를 출력한다던지,
일정 시간을 기다리는 루틴을 작성한다던지,
혹은 특정 기능이 수행한 시간을 감시한다던지 등
시간에 대한 개념을 활용하여 많은 유용한 프로그램을 개발할 수 있습니다.
리눅스 시스템 프로그래밍에서 시간의 개념을 어떻게 다루고
시간을 어떤 식으로 활용할 수 있는지 알아보는 시간을 갖도록 하겠습니다.

리눅스 시스템의 기원 시간
리눅스 시스템은 1970년 1월 1일 00:00:00 UTC (협정세계시, Universal Time Coordinate)로 정의된
기원(epoch)부터 경과된 절대시간을 초로 표현합니다.

초를 나타내는 대표 용어
밀리초(millisecond)      0.001 (천분의 일초)
마이크로초(microsecond)  0.000,001 (백만분의 일초)
나노초(nanosecond)       0.000,000,001 (십억분의 일초)

리눅스에서 시간을 나타내는 자료 구조
typedef long time_t;
<time.h>에 정의, 기원 시간 이후 경과한 초를 숫자로 표현

마이크로초
#include <sys/time.h>
struct timeval {
    time_t tv_sec;       // 초
    suseconds_t tv_usec; // 마이크로초
};

나노초
#include <time.h>
struct timespec {
    time_t tv_sec; // 초
    long tv_nsec;  // 나노초
};

상세 시간 정보
#include <time.h>
struct tm {
    int tm_sec;     // 초
    int tm_min;     // 분
    int tm_hour;    // 시(0~23)
    int tm_mday;    // 월경과일
    int tm_mon;     // 월(0~11)
    int tm_year;    // 년(1900년 이후 경과값)
    int tm_wday;    // 주경과일(0~6 일요일 이후 경과값)
    int tm_yday;    // 연경과일
    int tm_isdst;   // 섬머타임 여부
#ifdef _BSD_SOURCE
    long tm_gmtoff;  // GMT 시간대 오프셋
    const char *tm_zone; // 시간대 약어
#endif
};

#include <time.h>
현재 시간 획득
time_t time (time_t *t);
time() 함수를 이용하면 기원 시간(1970년 1월 1일 00:00:00) 이후 경과한 현재 시간을
초 단위로 표현하여 리턴한다.
t가 NULL이 아니라면 현재 시간을 t 포인터에 기록한다.
에러 발생 시 -1 리턴 후 errno 설정(EFAULT - 유효하지 않은 t 값)

ex)
#include <stdio.h>
#include <time.h>
int main()
{
    printf("time returned: %ld\n", (long)time(NULL));
    return 0;
}

#include <sys/time.h>
02 현재 시간 획득
int gettimeofday (struct timeval *tv, struct timezone *tz);
성공 시 tv가 가리키는 timeval 구조체에 기원 시간 이후 경과한 현재 시각을 기록하고 0 리턴(tz는 사용하지 않음 NULL)
실패 시 -1 리턴, errno 설정(EFAULT - 유효하지 않은 tv 값)

ex)
#include <stdio.h>
#include <sys/time.h>
int main()
{
    struct timeval tv;
    int ret;
    ret = gettimeofday(&tv, NULL);
    if (ret == -1) {
        perror("gettimeofday error: ");
        return -1;
    }
    printf("seconds = %ld, useconds = %ld\n", (long)tv.tv_sec, (long)tv.tv_usec);
}

#include <time.h>
int stime (time_t *t);
현재 시간 설정
성공 시 시스템 시간을 t 값으로 설정하고 0 리턴,
실패 시 -1 리턴, errno 설정
EFAULT 유효하지 않은 t값
EPERM 권한이 없음(root 관리자가 아님)

#include <sys/time.h>
int settimeofday (const struct timeval *tv, const struct timezone *tz);
성공 시 시스템 시간을 tv 값으로 설정하고 0 리턴, (gettimeofday()와 마찬가지로 tz값 NULL)
실패 시 -1 리턴, errno 설정

#include <time.h>
int clock_settime (clockid_t clock_id, const struct timespec *ts);
성공 시 clock_id로 지정한 시계를 ts로 지정한 시간으로 설정하고 0 리턴,
(설정 가능한 시계는 CLOCK_REALTIME)
실패 시 -1 리턴, errno 설정

_r 계열 함수들의 의미는?
04 시간 유틸리티
재진입 함수(Reentrancy)를 의미
초기 POSIX.1에 기반한 C 함수들은 단일 스레드 프로세스 환경을 가정하고 만들어졌다.
멀티스레드 환경에서 재진입 가능한지 여부를 보장할 수 없다.
(스레드 내에서 재진입이 불가능한 함수 호출 시 인자 데이터 내용을 변경한다든지 하여 결과값의 정합성을 보장하지 못함)
재진입 불가능한 함수 예: asctime(), gmtime(), ctime(), localtime() 등
이 문제를 해결하기 위해 _r이 붙은 별도의 함수 지정
예: asctime_r(), gmtime_r(), ctime_r(), localtime_r()

#include <time.h>
현재 시간 획득(나노초)
int clock_gettime (clockid_t clock_id, struct timespec *ts);
성공 시 0을 리턴하고 clock_id로 지정한 시계의 현재 시간을 ts에 저장한다.
실패 시 -1 리턴, errno 설정
EFAULT 유효하지 않은 ts
EINVAL 유효하지 않은 clock_id

clock_id
CLOCK_REALTIME 시스템에서 사용하는 실시간 시계
CLOCK_MONOTONIC 시스템 시동과 같이 불특정 시점부터 경과한 시간
CLOCK_MONOTONIC_RAW CLOCK_MONOTONIC과 유사, 시간이 뒤틀렸을 때 조정되지 않음
CLOCK_PROCESS_CPUTIME_ID 프로세스에서 지원(사용가능한)되는 고해상도 시계
CLOCK_THREAD_CPUTIME_ID CLOCK_PROCESS_CPUTIME_ID와 유사, 프로세스 스레드별 유일한 시계

#include <time.h>
시간 유틸리티
struct tm * gmtime (const time_t *timep);
struct tm * gmtime_r (const time_t *timep, struct tm *result);
struct tm * localtime (const time_t *timep);
struct tm * localtime_r (const time_t *timep, struct tm *result);
gmtime()은 time_t를 tm 구조체로 변환하여 UTC 시간대로 표현한다.
localtime()은 time_t를 tm 구조체로 변환하여 사용자 시간대로 표현한다.

double difftime (time_t time1, time_t time0);
difftime()은 두 time_t 값의 차이를 double 타입으로 리턴한다.
(double) (time1 - time0) 와 동일

#include <unistd.h>
unsigned int sleep(unsigned int seconds);
int usleep(useconds_t usec);
sleep()은 초 단위 잠들기, usleep()은 마이크로초 단위 잠들기(useconds_t는 unsigned int 타입)

#include <time.h>
int nanosleep(const struct timespec *rqtp, struct timespec *rmtp);
호출 성공 시 rqtp로 명시한 시간 동안 잠들며 0 리턴
(시그널이 방해 시 rqtp로 명시한 시간 전에 리턴될 수 있다. -1 리턴, errno = EINTR
rmtp가 NULL이 아니라면 rqtp 시간만큼 잠들지 못하고 남은 시간을 rmtp에 저장)

retry:
ret = nanosleep (&req, &rem);
if (ret) {
    if (errno == EINTR) {
        req.tv_sec = rem.tv_sec;
        req.tv_nsec = rem.tv_nsec;
        goto retry;
    }
    perror("nanosleep error: ");
}
*/
