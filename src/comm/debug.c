#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/syscall.h>
#include <string.h>
#include "debug.h"
#include "backtrace.h"

#define MAX_LEN_ONE_DEBUG_PRINT   1024

#define SPIDER_LEVEL_DEBUG 0
#define SPIDER_LEVEL_INFO  1
#define SPIDER_LEVEL_WARN  2
#define SPIDER_LEVEL_ERROR 3
#define SPIDER_LEVEL_CRIT  4

#define DEBUG_LEVEL 0

static const char * LOG_STR[] = {
    "ERROR",
    "WARN",
    "DEBUG",
    "INFO",
};

void debug_print(int level, const char *func_name, int line_number, const char *format, ...)
{
    char debug_buf[MAX_LEN_ONE_DEBUG_PRINT];
    char time_str[32];
    va_list ap;
    int debug_head_len = 0;

    time_t now = time(NULL);

    va_start(ap, format);
    strftime(time_str, sizeof(time_str), "%y%m%d %H:%M:%S", localtime(&now));
    snprintf(debug_buf, MAX_LEN_ONE_DEBUG_PRINT, "[%s][%s][%ld][%s:%d]",
             time_str, LOG_STR[level], syscall(SYS_gettid), func_name, line_number);
    debug_head_len = strlen(debug_buf);
    vsnprintf(debug_buf + debug_head_len, MAX_LEN_ONE_DEBUG_PRINT - debug_head_len, format, ap);
    va_end(ap);

    printf("%s\n", debug_buf);
    // fflush(stdout);
    return;
}

void dumpBuffer(const char *describe, unsigned char *buf, int len)
{
    int i = 0, count = 0;

    printf("\n--------dumpBuffer begin----------[%ld]\n", syscall(SYS_gettid));
    if (!buf || 0 > len){
        printf("dempBuffer, para err. buf:%p, len:%d\n", buf, len);
        return;
    }
    if (describe){
        printf("%s, len:%d\n", describe, len);
    }else{
        printf("len:%d\n", len);
    }

    for (i = 0; i < len; i++){
        if (16 == count){
            count = 0;
            printf("\n");
        }
        printf("%02x ", buf[i]);
        count++;
    }
    printf("\n--------dumpBuffer end--------\n\n");
    return;
}

void dumpBuffer_char(const char *describe, char *buf, int len)
{
    int i = 0, count = 0;

    printf("\n--------dumpBuffer begin----------[%ld]\n", syscall(SYS_gettid));
    if (!buf || 0 > len){
        printf("dempBuffer, para err. buf:%p, len:%d\n", buf, len);
        return;
    }
    if (describe){
        printf("%s, len:%d\n", describe, len);
    }else{
        printf("len:%d\n", len);
    }

    for (i = 0; i < len; i++){
        if (16 == count){
            count = 0;
            printf("\n");
        }
        printf("%c", buf[i]);
        count++;
    }
    printf("\n--------dumpBuffer end--------\n\n");
    return;
}

void debug_assert_failed(unsigned char* file, unsigned int line)
{
    printf("assert faile, file:%s, line:%d\r\n", file, line);
    print_trace(1);
    while(1){
        ;
    }
}


