/*
 * �����쳣��ֹʱ��ӡ�쳣������ö�ջ
 * gcc -g -rdynamic backtrace.c
 *
 * ���г�����ִ���:
 * System error, Stack trace:
 * 0 ./BackTraceTest(SystemErrorHandler+0x77) [0x40095b]
 * 1 /lib64/libc.so.6() [0x3a4fe326b0]
 * 2 ./BackTraceTest(Fun1+0x10) [0x400a10]
 * 3 ./BackTraceTest(Fun+0xe) [0x400a23]
 * 4 ./BackTraceTest(main+0x37) [0x400a5c]
 * 5 /lib64/libc.so.6(__libc_start_main+0xfd) [0x3a4fe1ed5d]
 * 6 ./BackTraceTest() [0x400829]
 * Segmentation fault (core dumped)
 *
 * gdb��ӡ��������Ϣ
 * gdb BackTraceTest
 * (gdb) info line *0x400a10
 * Line 66 of "BackTraceTest.c" starts at address 0x400a0c <Fun1+12> and ends at 0x400a13 <Fun1+19>.
 * (gdb) list *0x400a10
 * 0x400a10 is in Fun1 (BackTraceTest.c:66).
 * warning: Source file is more recent than executable.
 * 61	}
 * 62
 * 63	void Fun1()
 * 64	{
 * 65		char *p=NULL;
 * 66		*p = 'A';
 * 67	}
 * 68
 * 69	void Fun()
 * 70	{
 *
 *
 * addr2line ��λ�����ַ��Ӧ��Դ����λ��
 * addr2line  -e BackTraceTest -i 0x400a10
 * /home/cyf/workspace/BackTraceTest/BackTraceTest.c:66
 *
 */
#include "backtrace.h"
#include "debug.h"
#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int argc;
    char argv[10][128];
} BacktraceParam;

static BacktraceParam g_backtrace_var;

/* p_str - ����ջ��Ϣ ��./a.out(thread_mq_notify+0x80) [0x8051a62]  */
static void print_addr2line(char *p_str)
{
    char cmd[1024]            = {0};
    char addr_buf[2 + 64 + 1] = {0};
    char *p                   = NULL;
    char name[128]            = {0};

    ASSERT(NULL != p_str);

    p = strchr(p_str, '[');
    if (p != NULL) {
        /* ������ '[' ']' */
        strncpy(addr_buf, p + 1, strlen(p) - 2);
        /* ��װ���� */
        strncpy(name, g_backtrace_var.argv[0], strlen(g_backtrace_var.argv[0]));
        sprintf(cmd, "addr2line -e %s -i %s", name, addr_buf);
//        DDEBUG("%s\n", cmd);
        system(cmd);
    }
}

static void backtrace_init_argv(int argc, char **argv)
{
    int i = 0;

    memset(&g_backtrace_var, 0, sizeof(g_backtrace_var));
    for (i = 0; i < argc; i++) {
        strncpy(&g_backtrace_var.argv[i][0], &argv[i][0], sizeof(g_backtrace_var.argv[i]));
    }
}

void print_trace(int signum)
{
    const int len = 1024;
    void *func[len];
    int size;
    int i;
    char **funs;

    signal(signum, SIG_DFL);
    size = backtrace(func, len);
    funs = (char **)backtrace_symbols(func, size);
    fprintf(stderr, "System error, Stack trace:\n");
    for (i = 0; i < size; ++i) {
        fprintf(stderr, "%d %s \n", i, funs[i]);
        print_addr2line(funs[i]);
    }
    free(funs);
}

void backtrace_init(int argc, char **argv)
{
    backtrace_init_argv(argc, argv);
    signal(SIGSEGV, print_trace); // Invaild memory address
    signal(SIGABRT, print_trace); // Abort signal
    signal(SIGBUS, print_trace);
    signal(SIGFPE, print_trace);
    signal(SIGSTKFLT, print_trace);
    signal(SIGXFSZ, print_trace);
    signal(SIGSYS, print_trace);
}

void Fun1()
{
    char *p = NULL;
    *p      = 'A';
}

void Fun()
{
    Fun1();
}

int print_trace_test()
{
    signal(SIGSEGV, print_trace); // Invaild memory address
    signal(SIGABRT, print_trace); // Abort signal
    Fun();
    return 0;
}
