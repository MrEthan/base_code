#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <sys/prctl.h>
#include <time.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "timer_posix.h"
#include "epoll.h"
#include "debug.h"
#include "base.h"

typedef struct{
    int tfd;
    int value_s;
    int value_ms;
    int interval_s;
    int interval_ms;
    char dev_id[32];
}BaseTimer;


int base_timer_create(void)
{
    int tfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
    if (0 > tfd){
        printf("base_timer_create failed, tfd(%d)\n", tfd);
    }
    return tfd;
}

void base_timer_delete(int tfd)
{
    SAFE_CLOSE(tfd);
    return;
}

int base_timer_start(int eplfd, int tfd, int value_s, int value_ms, int interval_s, int interval_ms,
                    epoll_callback callback, void *data, int len)
{
    struct timespec nw;
    struct itimerspec its;
    EpollEvent epoll_data;
    char *pdata = NULL;
    int ret = 0;

    if ((NULL != data) && (0 != len)){
        pdata = (char *)malloc(len);
        if (NULL == pdata){
            DERROR("malloc err.\n");
            ret = -1;
            goto exit;
        }
    }

    ret = clock_gettime(CLOCK_MONOTONIC, &nw);
    if (ret){
        DERROR("clock_gettime() error\n");
        goto exit;
    }

    DDEBUG("nw.tv_sec:%ld, nw.tv_nsec:%ld\n", nw.tv_sec, nw.tv_nsec);

    its.it_value.tv_sec = nw.tv_sec + value_s;
    its.it_value.tv_nsec = value_ms * 1000000;
    its.it_interval.tv_sec = interval_s;
    its.it_interval.tv_nsec = interval_ms * 1000000;

    ret = timerfd_settime(tfd, TFD_TIMER_ABSTIME, &its, NULL);
    if (ret){
        DERROR("timerfd_settime() error\n");
        goto exit;
    }

    memset(pdata, 0, sizeof(len));
    memcpy(pdata, data, len);

    memset(&epoll_data, 0, sizeof(EpollEvent));
    epoll_data.eplfd = eplfd;
    epoll_data.fd = tfd;
    epoll_data.callback = callback;
    epoll_data.data = pdata; //此处挂上的data由epoll负责释放
    epoll_data.len = len;
    ret = base_epoll_ctl(eplfd, EPOLL_CTL_ADD, tfd, &epoll_data);
    if (ret){
        DERROR("base_epoll_ctl_add, error. eplfd(%d), ret(%d)\n", eplfd, ret);
        ret = -1;
        goto exit;
    }

exit:
    if (ret){
        SAFE_FREE(pdata);
    }
    return ret;
}

void base_timer_stop(int eplfd, int tfd)
{
    int ret = 0;
    struct itimerspec its;

    memset(&its, 0, sizeof(struct itimerspec));
    timerfd_settime(tfd, TFD_TIMER_ABSTIME, &its, NULL);
    /* 停止定时器, 从epoll摘除 */
    ret = base_epoll_ctl(eplfd, EPOLL_CTL_DEL, tfd, NULL);
    DDEBUG("eplfd:%d, tfd:%d, ret:%d\n", eplfd, tfd, ret);
    SAFE_CLOSE(tfd);
    return;
}


void *timer1_callback(void *p_data, int len)
{
    EpollEvent *p_epoll_data = NULL;
    unsigned long long count = 0;
    int ret = 0;
    BaseTimer *p_timer = NULL;

    if(NULL == p_data){
        DERROR("input para err.\n");
    }

    p_epoll_data = (EpollEvent *)p_data;
    p_timer = p_epoll_data->data;

    //防止重复触发，读取fd
    ret = read(p_epoll_data->fd, &count, sizeof(unsigned long long));
    printf("\n");
    DDEBUG("read timerfd, event_fd(%d), ret(%d), count(%llu)\n", p_epoll_data->fd, ret, count);

    //DDEBUG("timer1_callback, data:%s, len:%d\n", (char*)p_epoll_data->data, p_epoll_data->len);

    if (NULL != p_timer){
        DDEBUG("p_timer->interval_s:%d\n", p_timer->interval_s);
        DDEBUG("p_timer->dev_id:%s\n", p_timer->dev_id);
        if ((p_timer->interval_s == 0) && (p_timer->interval_ms == 0)){
            base_timer_stop(p_epoll_data->eplfd, p_epoll_data->fd);
        }
    }

    return NULL;
}

//测试例子
void do_action(int epfd)
{
    int tfd = -1;
    int ret = -1;
    BaseTimer data;

    tfd = base_timer_create();
    if (0 > tfd){
        DERROR("timer creat fail. tfd(%d)\n", tfd);
        return;
    }

    memset(&data, 0, sizeof(data));
    data.tfd = tfd;
    data.value_s = 4;
    data.value_ms = 0;
    data.interval_s = 1;
    data.interval_ms = 0;
    strncpy(data.dev_id, "123456789", sizeof(data.dev_id) - 1);
    ret = base_timer_start(epfd, tfd, 4, 0, 1, 0, timer1_callback, (void *)&data, sizeof(data));
    if (ret){
        DERROR("base_timer_start fail. ret (%d)\n", ret );
        goto exit;
    }

exit:
    if (ret){
        base_timer_delete(tfd);
    }

    return;
}

//测试例子 一次性定时器
void do_action1(int epfd)
{
    int tfd = -1;
    int ret = -1;
    BaseTimer data;

    tfd = base_timer_create();
    if (0 > tfd){
        DERROR("timer creat fail. tfd(%d)\n", tfd);
        return;
    }

    memset(&data, 0, sizeof(data));
    data.tfd = tfd;
    data.value_s = 1;
    data.value_ms = 0;
    data.interval_s = 0;
    data.interval_ms = 0;
    strncpy(data.dev_id, "ABCDEF", sizeof(data.dev_id) - 1);
    ret = base_timer_start(epfd, tfd, 1, 0, 0, 0, timer1_callback, (void *)&data, sizeof(data));
    if (ret){
        DERROR("base_timer_start fail. ret (%d)\n", ret );
        goto exit;
    }

exit:
    if (ret){
        base_timer_delete(tfd);
    }

    return;
}


int timer_test(void)
{
    pthread_t tid;
    int ret = -1;
    int eplfd = -1;

    base_epoll_init();

    eplfd = base_epoll_create();
    DDEBUG("eplfd:%d\n", eplfd);

    ret = pthread_create(&tid, NULL, epoll_thread, &eplfd);
    DDEBUG("ret(%d)\n", ret);
    sleep(1);

    do_action(eplfd);
    do_action1(eplfd);

    while(1)
    {
        sleep(1);
    }

    return ret;
}

