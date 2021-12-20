#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <sys/prctl.h>
#include <time.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/eventfd.h>

#include "comm/base.h"
#include "epoll.h"

void *event_callback(void *data, int len)
{
    int ret = 0;
    int event_fd = -1;
    long long event = 0;

    assert(data != NULL);

    event_fd = *(int *)data;

    ret = read(event_fd, &event, sizeof(event));
    if (ret > 0){
        printf("read:%lld \n", event);
    }else{
        DERROR("read err ret:%d\n", ret);
    }

    return NULL;
}

void * event_read_routine(void *arg)
{
    int ret = 0;
    int ep_fd = -1;
    int efd = -1;
    EpollEvent event;
    struct epoll_event evnts[MX_EVNTS];
    int n = 0;
    int i = 0;
    EpollEvent *epoll_data = NULL;

    pthread_detach(pthread_self());

    DDEBUG("event_read_routine start...\n");
    prctl(PR_SET_NAME, "event_read_thr\n");

    assert(NULL != arg);

    efd = *(int *)arg;

    /* 创建epoll, 等待event */
    ep_fd = base_epoll_create();
    if (ep_fd < 0){
        return NULL;
    }

    memset(&event, 0, sizeof(event));
    event.eplfd = ep_fd;
    event.fd = efd;
    event.callback = event_callback;
    event.data = NULL;
    event.len = 0;
    ret = base_epoll_ctl(ep_fd, EPOLL_CTL_ADD, efd, &event);
    if (ret < 0){
        goto EXIT;
    }

    while(1){
        n = base_epoll_wait(ep_fd, evnts, MX_EVNTS, 0);
        if(0 > n){
           DERROR("base_epoll_wait, error\n");
           break;
        }

        for(i = 0; i < n;i++){
            epoll_data = (EpollEvent *)(evnts[i].data.ptr);
            if (EPOLLIN & evnts[i].events){
                if (epoll_data){
                    epoll_data->callback((void *)&epoll_data->fd, 0);
                }else{
                    DWARN("base_epoll_proc_thr, epoll_data is null, n(%d).\n", n);
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
    }

EXIT:
    base_epoll_close(ep_fd);
    return NULL;
}

/**
* 创建eventfd, 启动线程epollwait event事件
*/
void event_fd_test(void)
{
    int efd = -1;
    int ret = 0;
    int i = 0;
    pthread_t pid;
    long long event = 0;

    efd = eventfd(0, 0);
    if (efd < 0){
        DERROR("event create err \n");
        return ;
    }

    ret = pthread_create(&pid, NULL, event_read_routine, (void *)&efd);
    if (ret < 0){
        DERROR("thr create err \n");
        return ;
    }

    for (i = 0; i < 5; i++){
        event = i;
        ret = write(efd, &event, sizeof(event));
        printf("write:%lld, len:%d \n", event, ret);
        sleep(1);
    }

    return;
}

