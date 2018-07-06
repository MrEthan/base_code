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

#include "list.h"
#include "hash-table.h"
#include "compare-int.h"
#include "hash-int.h"
#include "comm/base.h"
#include "epoll.h"






#define MX_EVNTS 10

static ListEntry *g_p_epoll_list = NULL; //BsaeEpoll
pthread_rwlock_t g_epoll_list_lock;

#if 0
BsaeEpoll *base_epoll_find_epoll_node(int epoll_fd)
{
    BsaeEpoll *p_epoll = NULL;

    list_for_each(BsaeEpoll, &g_epoll_list, p_epoll){
        if (p_epoll->epoll_fd = epoll_fd){
            return p_epoll;
        }
    }
    return NULL;
}
#endif

static void base_epoll_free_event_hash(HashTable *p_event_hash)
{
    hash_table_free(p_event_hash);
}

/* �ͷ�epoll�¼����ƿ�͸�����Դ */
void base_epoll_free_event_block(void *p_data)
{
    EpollEvent *p_evt_block = NULL;

    if (NULL != p_data){
        p_evt_block = p_data;
        SAFE_FREE(p_evt_block->data); //epoll�ͷŽӹܵĸ�����Դ
        SAFE_FREE(p_evt_block);
    }
    return ;
}

static int base_epoll_list_equal(void* value1, void* value2)
{
    BsaeEpoll *p_epoll = NULL;

    assert(value1 != NULL);
    assert(value2 != NULL);

    p_epoll = (BsaeEpoll *)value1;

    return (p_epoll->epoll_fd == *(int *)value2);
}

int base_epoll_create(void)
{
    int eplfd = -1;
    int ret = 0;
    BsaeEpoll *p_epoll = NULL;
    HashTable *p_hash = NULL;

    eplfd = epoll_create(1);
    if(eplfd <= 0){
        printf("epoll_create1() error, eplfd(%d)\n", eplfd);
        return -1;
    }

    /* ����event hash */
    p_hash = hash_table_new(int_hash, int_equal);
    if (NULL == p_hash){
       DERROR("epoll create, new hash failed.\n");
       ret = -1;
       goto EXIT;
    }
    /* ע��hash���� */
    hash_table_register_free_functions(p_hash, NULL, base_epoll_free_event_block);

    /* ����epoll����ڵ� */
    p_epoll = malloc(sizeof(BsaeEpoll));
    if (NULL == p_epoll){
        DERROR("create list node, malloc err.\n");
        ret = -1;
        goto EXIT;
    }
    memset(p_epoll, 0, sizeof(BsaeEpoll));
    p_epoll->epoll_fd = eplfd;
    p_epoll->p_event_hash = p_hash;

    base_rwlock_wrlock(&g_epoll_list_lock);
    list_append(&g_p_epoll_list, (void *)p_epoll);
    base_rwlock_unlock(&g_epoll_list_lock);

EXIT:
    if (-1 == ret){
        SAFE_FREE(p_epoll);
        SAFE_CLOSE(eplfd);
    }
    return eplfd;
}

void base_epoll_close(int epfd)
{
    BsaeEpoll *p_epoll = NULL;
    ListEntry *p_node = NULL;

    if (-1 == epfd){
        return;
    }

    base_rwlock_wrlock(&g_epoll_list_lock);

    p_node = list_find_data(g_p_epoll_list, base_epoll_list_equal, &epfd);
    if (NULL == p_node){
        base_rwlock_unlock(&g_epoll_list_lock);
        goto EXIT;
    }
    p_epoll = p_node->data;
    /* ժ�� */
    list_remove_entry(&g_p_epoll_list, (void *)p_node);
    /* ժ�����Ѿ������������õ������Դ, ���Խ��� */
    base_rwlock_unlock(&g_epoll_list_lock);

    /* �ͷŸ�����Դ */
    base_epoll_free_event_hash(p_epoll->p_event_hash);

    /* �ͷű�����ڵ� */
    SAFE_FREE(p_epoll);

EXIT:
    SAFE_CLOSE(epfd);
    return ;
}

int base_epoll_ctl(int epfd, int op, int event_fd, EpollEvent *p_epoll_type)
{
    int ret = 0;
    BsaeEpoll *p_epoll = NULL;
    EpollEvent *p_evt_blk = NULL;
    ListEntry *p_node = NULL;
    struct epoll_event ev;

    assert(epfd > 0);
    assert(event_fd > 0);


    base_rwlock_wrlock(&g_epoll_list_lock);

    /* ����epoll fd����epoll ����ڵ� */
    p_node = list_find_data(g_p_epoll_list, base_epoll_list_equal, &epfd);
    if (NULL == p_node){
        DERROR("epoll do not existed. epfd:%d\n", epfd);
        goto EXIT;
    }
    p_epoll = p_node->data;

    switch (op){
        case EPOLL_CTL_ADD:
            p_evt_blk = (EpollEvent *)malloc(sizeof(EpollEvent));
            if (NULL == p_evt_blk){
                DERROR("epoll ctl add, malloc err.");
                ret = -1;
                goto EXIT;
            }

            memset(&ev, 0, sizeof(ev));
            memset(p_evt_blk, 0, sizeof(EpollEvent));
            memcpy(p_evt_blk, p_epoll_type, sizeof(EpollEvent));
            ev.events = EPOLLIN | EPOLLHUP | EPOLLERR | EPOLLOUT;
            ev.data.ptr = p_evt_blk;
            ret = epoll_ctl(epfd, op, event_fd, &ev);
            if (0 != ret){
                DERROR("epoll ctl add, failed. ret :%d, err:%s\n", ret, strerror(errno));
                ret = -1;
                goto EXIT;
            }

            /* �õ�epoll ����ڵ�, ���epoll���ƿ� */
            if (1 != hash_table_insert(p_epoll->p_event_hash, (HashTableKey)&event_fd, (HashTableValue)p_evt_blk)){
                DERROR("epoll ctl add, insert hash failed. epfd:%d, event_fd:%d\n", epfd, event_fd);
                goto EXIT;
            }
            break;

        case EPOLL_CTL_DEL:
            ret = epoll_ctl(epfd, op, event_fd, &ev);
            if (0 != ret){
                DERROR("epoll ctl del, failed. ret :%d, err:%s\n", ret, strerror(errno));
            }

            /* ɾ��epoll���ƿ� */
            if (1 != hash_table_remove(p_epoll->p_event_hash, (HashTableKey)&event_fd)){
                DERROR("epoll ctl del, hash table remove failed. ret:%d\n", ret);
            }
            break;

        case EPOLL_CTL_MOD:
            /* ɾ��ԭepoll���ƿ� */
            if (1 != hash_table_remove(p_epoll->p_event_hash, (HashTableKey)&event_fd)){
                DERROR("epoll ctl mod, hash table remove failed. ret:%d\n", ret);
            }

            /*�����µ�epoll���ƿ�*/
            p_evt_blk = (EpollEvent *)malloc(sizeof(EpollEvent));
            if (NULL == p_evt_blk){
                DERROR("epoll ctl mod, malloc new event block err.");
                ret = -1;
                goto EXIT;
            }

            /* ��ʼ�����ƿ�����, ��ӵ�epoll */
            memset(&ev, 0, sizeof(ev));
            memset(p_evt_blk, 0, sizeof(EpollEvent));
            memcpy(p_evt_blk, p_epoll_type, sizeof(EpollEvent));
            ev.events = EPOLLIN | EPOLLHUP | EPOLLERR | EPOLLOUT;
            ev.data.ptr = p_evt_blk;
            ret = epoll_ctl(epfd, op, event_fd, &ev);
            if (0 != ret){
                DERROR("epoll ctl add, failed. ret :%d, err:%s\n", ret, strerror(errno));
                ret = -1;
                goto EXIT;
            }

            /* ���µ�epoll���ƿ���ӵ�hash�� */
            if (1 != hash_table_insert(p_epoll->p_event_hash, (HashTableKey)&event_fd, (HashTableValue)p_evt_blk)){
                DERROR("epoll ctl add, insert hash failed. epfd:%d, event_fd:%d\n", epfd, event_fd);
                goto EXIT;
            }
            break;

        default:
            DERROR("epoll op err.\n");
            break;
    }

EXIT:
    base_rwlock_unlock(&g_epoll_list_lock);
    if (-1 == ret){
        SAFE_FREE(p_evt_blk);
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

void base_epoll_init(void)
{
    pthread_rwlock_init(&g_epoll_list_lock, NULL);
    return;
}

void base_epoll_fini(void)
{
    pthread_rwlock_destroy(&g_epoll_list_lock);
    return;
}

void *epoll_thread(void *arg)
{
    struct epoll_event evnts[MX_EVNTS];
    int eplfd = *(int *)arg;
    int n = 0;
    int i = 0;
    EpollEvent *epoll_data = NULL;

    pthread_detach(pthread_self());

    printf("epoll_thread start...\n");
    prctl(PR_SET_NAME, "epoll_thread\n");

    DDEBUG("eplfd:%d\n", eplfd);

    while(1)
    {
        n = base_epoll_wait(eplfd, evnts, MX_EVNTS, 0);
        if(0 > n){
            printf("base_epoll_wait, error\n");
            break;
        }

        for(i = 0; i < n;i++){
            epoll_data = (EpollEvent *)(evnts[i].data.ptr);
            if (EPOLLIN & evnts[i].events){
                if (epoll_data){
                    epoll_data->callback(epoll_data, sizeof(epoll_data));
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

