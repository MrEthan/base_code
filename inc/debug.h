#ifndef DEBUG_H
#define DEBUG_H
 
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>


#define MAX_MESG_LEN   1024

#define SPIDER_LEVEL_DEBUG 0
#define SPIDER_LEVEL_INFO  1
#define SPIDER_LEVEL_WARN  2
#define SPIDER_LEVEL_ERROR 3
#define SPIDER_LEVEL_CRIT  4 

#define DEBUG_LEVEL 0

static const char * LOG_STR[] = { 
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "CRIT"
};

//可变参数 
//输出日期，时间，日志级别，源码文件，行号，信息
//'\'后面不要加注释
#define SPIDER_LOG(level, format, ...) do{ \
    if (level >= DEBUG_LEVEL) {\
        time_t now = time(NULL);                      \
        char msg[MAX_MESG_LEN];                        \
        char buf[32];                                   \
        sprintf(msg, format, ##__VA_ARGS__);             \
        strftime(buf, sizeof(buf), "%Y%m%d %H:%M:%S", localtime(&now)); \
        fprintf(stdout, "[%s][%s][file:%s:%d] %s\n", buf, LOG_STR[level],__FILE__,__LINE__, msg); \
        fflush (stdout); \
    }\
     if (level == SPIDER_LEVEL_ERROR) {\
        exit(-1); \
    } \
} while(0)

#endif //DEBUG_H