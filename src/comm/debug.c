#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/syscall.h>
#include <string.h>
#include "debug.h"

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

void debug_print(int level, const char *format, ...)
{
    char debug_buf[MAX_LEN_ONE_DEBUG_PRINT];
    char time_str[32];
    va_list ap;
    int debug_head_len = 0;

    time_t now = time(NULL);

    va_start(ap, format);
    strftime(time_str, sizeof(time_str), "%y%m%d-%H:%M:%S", localtime(&now));
    snprintf(debug_buf, MAX_LEN_ONE_DEBUG_PRINT, "[%s][%s][%ld][%s]",
             time_str, LOG_STR[level], syscall(SYS_gettid), __FUNCTION__);
    debug_head_len = strlen(debug_buf);
    vsnprintf(debug_buf + debug_head_len, MAX_LEN_ONE_DEBUG_PRINT - debug_head_len, format, ap);
    va_end(ap);

    printf("%s", debug_buf);

    return;
}


