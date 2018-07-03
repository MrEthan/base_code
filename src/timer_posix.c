#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <sys/prctl.h>
#include <time.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "timer_posix.h"

#define MX_EVNTS 10

typedef void (*epoll_callback)(void *data, int len);
typedef struct{
    int event_fd;
    epoll_callback callback;
    void *data;
    int len;
}base_epoll_type;


static int g_epoll_fd = -1;



int base_epoll_init(void)
{
    int eplfd = -1;

    eplfd = epoll_create(1);
    if(eplfd <= 0){
        printf("epoll_create1() error, eplfd(%d)\n", eplfd);
        return -1;
    }
    return eplfd;
}

void base_epoll_fini(int epfd)
{
    if (epfd > 0){
        close(epfd);
    }
    return ;
}

int base_epoll_ctl_add(int epfd, base_epoll_type *ep_data)
{
    int ret = -1;
    struct epoll_event ev;

    if ((0 > epfd) || (!ep_data) ||
        (0 > ep_data->event_fd)  ||
        (NULL == ep_data->callback)){
        printf("base_epoll_ctl_add, input para err.\n");
        return -1;
    }

    memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLIN | EPOLLHUP | EPOLLERR | EPOLLOUT;
    ev.data.ptr = ep_data;
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, ep_data->event_fd, &ev);
    if (ret){
        printf("base_epoll_ctl_add, epoll_ctl error. eplfd(%d), event_fd(%d)\n", epfd, ep_data->event_fd);
        ret = -1;
    }
    return ret;
}

int base_epoll_ctl_delete(int epfd, base_epoll_type *ep_data)
{
    int ret = -1;
    struct epoll_event ev;

    if ((0 > epfd) || (!ep_data) || (0 > ep_data->event_fd)){
        printf("base_epoll_ctl_delete, input para err.\n");
        return -1;
    }
    ret = epoll_ctl(epfd, EPOLL_CTL_DEL, ep_data->event_fd, &ev);
    if (ret){
        printf("base_epoll_ctl_delete, epoll_ctl error. eplfd(%d), event_fd(%d)\n", epfd, ep_data->event_fd);
        ret = -1;
    }
    return ret;
}

int base_epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout)
{
    if ((0 > epfd) || (!events)){
        printf("base_epoll_wait, input para err.\n");
        return -1;
    }

    return epoll_wait(epfd, events, maxevents, timeout);
}

int base_timer_stop();



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
    if (0 > tfd){
        return ;
    }
    close(tfd);
    return;
}

int base_timer_start(int eplfd, int tfd, int interval, epoll_callback callback, void *data, int len)
{
    struct timespec nw;
    struct itimerspec its;
    base_epoll_type * epoll_data = NULL;
    unsigned char *pdata = NULL;
    int ret = 0;

    if ((0 > eplfd) || (0 > tfd) || (0 >= interval) || (NULL == callback)){
        printf("base_timer_start, input para err.\n");
        return ret;
    }

    epoll_data = (base_epoll_type *)malloc(sizeof(base_epoll_type));
    if (NULL == epoll_data){
        printf("base_timer_start, malloc err.\n");
        ret = -1;
        goto exit;
    }

    if ((NULL != data) && (0 != len)){
        pdata = (unsigned char *)malloc(len);
        if (NULL == pdata){
            printf("base_timer_start, malloc err.\n");
            ret = -1;
            goto exit;
        }
    }

    ret = clock_gettime(CLOCK_MONOTONIC, &nw);
    if (ret){
        printf("base_timer_start, clock_gettime() error\n");
        goto exit;
    }
    printf("clock_gettime, nw.tv_sec:%ld, nw.tv_nsec:%ld\n", nw.tv_sec, nw.tv_nsec);

    its.it_value.tv_sec = nw.tv_sec + interval;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = interval;
    its.it_interval.tv_nsec = 0;

    ret = timerfd_settime(tfd, TFD_TIMER_ABSTIME, &its, NULL);
    if (ret){
        printf("timerfd_settime() error\n");
        goto exit;
    }

    memset(pdata, 0, sizeof(len));
    memcpy(pdata, data, len);

    memset(epoll_data, 0, sizeof(base_epoll_type));

    epoll_data->event_fd = tfd;
    epoll_data->callback = callback;
    epoll_data->data = pdata;
    epoll_data->len = len;
    ret = base_epoll_ctl_add(eplfd, epoll_data);
    if (ret){
        printf("base_epoll_ctl_add, epoll_ctl() error. eplfd(%d), ret(%d)\n", eplfd, ret);
        ret = -1;
        goto exit;
    }

exit:
    if (ret){
        if (pdata){
            free(pdata);
        }
        if (epoll_data){
            printf("free(ep_data)\n");
            free(epoll_data);
        }
    }
    return ret;
}


void timer1_callback(void *data, int len)
{
    printf("timer1_callback, data:%s\n", (char*)data);
    return;
}

//测试例子
void do_action(void)
{
    int tfd = -1;
    int ret = -1;
    char str[] = "timer1 work";

    tfd = base_timer_create();
    if (0 > tfd){
        printf("do_action, timer creat fail. tfd(%d)\n", tfd);
        return;
    }

    ret = base_timer_start(g_epoll_fd, tfd, 2, timer1_callback, (void *)str, strlen(str)+1);
    if (ret){
        printf("do_action, base_timer_start fail. ret (%d)\n", ret );
        goto exit;
    }

exit:
    if (ret){
        base_timer_delete(tfd);
    }

    return;
}

void *epoll_thread(void *arg)
{
    struct epoll_event evnts[MX_EVNTS];
    int eplfd = *(int *)arg;
    int n = 0;
    int i = 0;
    int ret = -1;
    unsigned long long count = 0;
    base_epoll_type *epoll_data = NULL;

    pthread_detach(pthread_self());

    printf("epoll_thread start...\n");
    prctl(PR_SET_NAME, "epoll_thread\n");

    while(1)
    {
        n = base_epoll_wait(eplfd, evnts, MX_EVNTS, 0);
        if(0 > n){
            printf("base_epoll_wait, error\n");
            break;
        }

        for(i = 0; i < n;i++){
            epoll_data = (base_epoll_type *)(evnts[i].data.ptr);
            if (EPOLLIN & evnts[i].events){
                //考虑此处删除一次性事件 从epoll中去除，删除定器件
                if (epoll_data){                    //防止重复触发，读取fd
                    ret = read(epoll_data->event_fd, &count, sizeof(unsigned long long));
                    printf("read timerfd, event_fd(%d), ret(%d), count(%llu)\n", epoll_data->event_fd, ret, count);
                    epoll_data->callback(epoll_data->data, epoll_data->len);
                }else{
                    printf("base_epoll_proc_thr, epoll_data is null, n(%d).\n", n);
                    continue;
                }
            }else if (EPOLLHUP & evnts[i].events){
                printf("EPOLLHUP\n");
                goto EXIT;
            }else if (EPOLLERR & evnts[i].events){
                printf("EPOLLHUP\n");
                goto EXIT;
            }
        }
        sleep(1);
    }
EXIT:
    printf("exit!!!!!\n");
    close(eplfd);
    return NULL;
}

int timer_test(void)
{
    pthread_t tid;
    int ret = -1;
    int eplfd = -1;

    eplfd = base_epoll_init();
    if (0 > eplfd){
        return -1;
    }

    g_epoll_fd = eplfd;

    ret = pthread_create(&tid, NULL, epoll_thread, &eplfd);
    printf("ret(%d)\n", ret);
    sleep(1);

    do_action();

    while(1)
    {
        sleep(1);
    }

    return ret;
}


