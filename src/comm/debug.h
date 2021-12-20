#ifndef DEBUG_H
#define DEBUG_H

typedef enum{
    EN_ERROR,
    EN_WARN,
    EN_DEBUG,
    EN_INFO,
}enDebugLevel;

#define ASSERT(expr) ((expr) ? (void)0 : debug_assert_failed((unsigned char *)__FILE__, __LINE__));

#define DERROR(format, args...) debug_print(EN_ERROR, __FUNCTION__, __LINE__, format, ##args)
#define DWARN(format, args...) debug_print(EN_WARN, __FUNCTION__, __LINE__, format, ##args)
#define DDEBUG(format, args...) debug_print(EN_DEBUG, __FUNCTION__, __LINE__, format, ##args)
#define DINFO(format, args...) debug_print(EN_INFO, __FUNCTION__, __LINE__, format, ##args)

void debug_print(int level, const char *func_name, int line_number, const char *format, ...);
void dumpBuffer(const char *describe, unsigned char *buf, int len);
void dumpBuffer_char(const char *describe, char *buf, int len);
void debug_assert_failed(unsigned char* file, unsigned int line);

#if 0
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
#endif

#endif //DEBUG_H
