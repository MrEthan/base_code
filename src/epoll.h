#ifndef BASE_EPOLL_H
#define BASE_EPOLL_H

#include "hash-table.h"

typedef void *(*epoll_callback)(void *data, int len);
/* EPOLL�¼����ƿ� */
typedef struct{
    int eplfd;
    int fd; //hash key
    epoll_callback callback;
    void *data; //���ϵ�data��epoll�ӹܣ�epollģ�鸺���ͷ�
    int len;
}EpollEvent;

typedef struct{
    int epoll_fd;
    HashTable *p_event_hash; //data:BaseEpollEventType
}BsaeEpoll;


int base_epoll_create(void);
void base_epoll_close(int epfd);
int base_epoll_ctl(int epfd, int op, int event_fd, EpollEvent *p_epoll_type);
int base_epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
void base_epoll_fini(void);
void base_epoll_init(void);
void *epoll_thread(void *arg);


#endif

