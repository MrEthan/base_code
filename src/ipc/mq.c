/* Link with -lrt */
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <pthread.h>
#include <signal.h>
#include "base.h"

#define USE_SIG_NOTIFY

#define MQ_FILE "/mq_test"
#define MQ_MSG_NUM_MAX  10
#define MQ_MSG_BUF_MAX  1024

void *thread_productor(void *arg)
{
    int iRet = 0;
    mqd_t mq_fd = -1;
    struct mq_attr attr;
    ssize_t iSendLen = 0;
    char acSendBuf[MQ_MSG_BUF_MAX + 1];

    prctl(PR_SET_NAME, "productor_thr");
    pthread_detach(pthread_self());

    /* 填充待发送缓冲区 */
    memset(acSendBuf, 0, sizeof(acSendBuf));
    iSendLen = strlen("mqtest 99");
    strncpy(acSendBuf, "mqtest 99", iSendLen);

    memset(&attr, 0, sizeof(struct mq_attr));
    attr.mq_maxmsg = MQ_MSG_NUM_MAX;
    attr.mq_msgsize = MQ_MSG_BUF_MAX;

    mq_unlink(MQ_FILE);
    mq_fd = mq_open(MQ_FILE,
                    O_RDWR | O_CREAT | O_NONBLOCK, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP,
                    &attr);
    if (mq_fd < 0){
        DERROR("mq open err fd:%d %s \n", mq_fd, strerror(errno));
        return NULL;
    }

    while(1){
        iRet = mq_send(mq_fd, acSendBuf, iSendLen, 99);
        if (iRet != 0 && errno != EAGAIN){
            DERROR("mq_send err iRet:%d errno:%d %s \n", iRet, errno, strerror(errno));
            goto EXIT;
        }

        sleep(1);
    }

EXIT:
    SAFE_CLOSE(mq_fd);
    return NULL;
}

void *thread_customer(void *arg)
{
    ssize_t iRecvLen = 0;
    char acRecvBuf[MQ_MSG_BUF_MAX + 1] = {0};
    mqd_t mq_fd = -1;
    unsigned int prio = 0;

    prctl(PR_SET_NAME, "customer_thr");
    pthread_detach(pthread_self());

    sleep(1);

    mq_fd = mq_open(MQ_FILE, O_RDONLY); //阻塞模式读
    if (mq_fd < 0){
        DERROR("mq open err mq_fd:%d, %s \n", strerror(errno));
        goto EXIT;
    }
    DDEBUG("recv mq_fd:%d \n", mq_fd);

    while(1){
        memset(acRecvBuf, 0, sizeof(acRecvBuf));
        iRecvLen = mq_receive(mq_fd, acRecvBuf, sizeof(acRecvBuf), &prio);
        if (iRecvLen > 0){
            DDEBUG("iRecvLen:%d prio:%d RecvBuf:%s\n", iRecvLen, prio, acRecvBuf);
        }else{
            DERROR("mq_receive, %s \n", strerror(errno));
        }
    }

EXIT:
    SAFE_CLOSE(mq_fd);
    return NULL;
}

/* 使用mq_notify函数，注册信号的方式处理mq消息队列 */
void *thread_mq_notify(void *arg)
{
    sigset_t *set;
    int signo = -1;
    struct sigevent sev;
    mqd_t mq_fd = -1;
    struct mq_attr attr;
    char *buff = NULL;
    int recv_num = 0;
    unsigned int prio = 0;

    prctl(PR_SET_NAME, "mq_notify");
    pthread_detach(pthread_self());

    DDEBUG("thr start\n");

    set = (sigset_t *)arg;

    sleep(1);

    mq_fd = mq_open(MQ_FILE, O_RDONLY | O_NONBLOCK); //非阻塞模式读取
    if (mq_fd < 0){
        DERROR("mq open err mq_fd:%d, %s \n", strerror(errno));
        goto EXIT;
    }

    /* 获取消息队列的长度 */
    memset(&attr, 0, sizeof(struct mq_attr));
    mq_getattr(mq_fd, &attr);
    buff = calloc(attr.mq_msgsize, sizeof(char));
    if(NULL == buff){
        perror("calloc error");
        goto EXIT;
    }

    /* 注册mq_notify信号, 当接收队列由空变为非空时, 产生异步信号SIGUSR1 */
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGUSR1;

    while(1){
        if (0 != mq_notify(mq_fd, &sev)){ //sigwait后必须重新注册mq_notify
            DERROR("mq_notify reg err, %s\n", strerror(errno));
            goto EXIT;
        }
        /* 使用非阻塞模式循环读取, 直到返回EAGAIN这种错误 */
        while(0 <= (recv_num = mq_receive(mq_fd, buff, attr.mq_msgsize, &prio))){
            printf("prio:%d recv num:%d, %s\n", prio, recv_num, buff);
        }
        if (EAGAIN == errno){ //读取空队列错误码
            //DDEBUG("mq recv by sigwait, %s\n", strerror(errno));
        }

        /* 等待信号sigusr1 */
        if (0 != sigwait(set, &signo)){
            DERROR("sigwait err, %s\n", strerror(errno));
            goto EXIT;
        }
        DDEBUG("recv signo:%d\n", signo);
    }

EXIT:
    return NULL;
}

/**
* 1.创建线程A和B 线程A(生产者)通过mq给线程B(消费者)发送消息
*/
int TestMqThread(void)
{
    int ret = 0;
    sigset_t set;
    pthread_t thread_id;

    /* BLOCK信号SIGUSR1, 便于sigwait可以等到此信号 */
    errno = 0;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    if (0 != pthread_sigmask(SIG_BLOCK, &set, NULL)){
        DERROR("pthread_sigmask err, %s \n", strerror(errno));
        return -1;
    }

    ret = pthread_create(&thread_id, NULL, thread_productor, NULL);
    if (ret){
        DERROR("thread_productor create err ret:%d %s \n", ret, strerror(errno));
        return -1;
    }

#ifdef USE_SIG_NOTIFY
    /* 信号通知mq_receive 信号必须在根父线程中注册，子线程继承父线程信号 */
    ret = pthread_create(&thread_id, NULL, thread_mq_notify, (void *)&set);
    if (ret){
        DERROR("thread_mq_notify create err ret:%d %s \n", ret, strerror(errno));
        return -1;
    }
#else
    /* 阻塞轮询mq_receive */
    ret = pthread_create(&thread_id, NULL, thread_customer, NULL);
    if (ret){
        DERROR("thread_customer create err ret:%d %s \n", ret, strerror(errno));
        return -1;
    }
#endif

    return 0;
}

/**
* 创建进程A和B A(生产者)通过mq给B(消费者)发送消息
*/
int TestMqProcess(void)
{
    /* 创建进程用于接收数据 */

    return 0;
}


