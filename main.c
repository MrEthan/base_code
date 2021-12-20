
#include "main.h"
#include "backtrace.h"
#include "comm/base.h"
#include "debug.h"
#include "ecdh.h"
#include "list.h"
#include "openssl/e_os2.h"
#include "pthread_cond.h"
#include "timer_posix.h"
#include <stdio.h>

/* 十六进制转换成ASCII表示 */
void hexbyte(char *out, uint8_t b)
{
    static const char hextable[] = "0123456789abcdef";
    out[0]                       = hextable[b >> 4];
    out[1]                       = hextable[b & 0x0f];
}

extern int TestMqThread(void);
extern void event_fd_test(void);
extern int encrypt_evp_aes_demo(void);
extern void setjmp_test();
extern int pthread_cond_demo(void);

int main(int argc, char **argv)
{
    /* 注册信号捕获函数 */
    backtrace_init(argc, argv);

    /* 测试函数 */
    event_fd_test();
    setjmp_test();
    pthread_cond_demo();

    print_trace_test();

    while (1) {
        pause();
    }
    return 0;
}
