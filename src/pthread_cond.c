#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "comm/base.h"

/* 线程间同步 条件变量使用demo */
#define ERR_EXIT(m) \
        do \
        { \
                perror(m); \
                exit(EXIT_FAILURE); \
        } while(0)

#define CONSUMERS_COUNT 2
#define PRODUCERS_COUNT 1

pthread_mutex_t g_mutex;
pthread_cond_t g_cond;

pthread_t g_thread[CONSUMERS_COUNT + PRODUCERS_COUNT];

static int nready = 0;

void *consume(void *arg)
{
    pthread_detach(pthread_self());
    int idx = *(int *)arg;
    
    while (1)
    {
        pthread_mutex_lock(&g_mutex);
        while (nready <= 0)
        {
            pthread_cond_wait(&g_cond, &g_mutex);
        }
        --nready;
        printf("[%d][%lu]consume a product, nready:%d\n", idx, pthread_self(), nready);

        pthread_mutex_unlock(&g_mutex);
        usleep(10);
    }
    return NULL;
}

void *produce(void *arg)
{
    static int var = 2;
    pthread_detach(pthread_self());
    int idx = *(int *)arg;

    while (1)
    {
        pthread_mutex_lock(&g_mutex);
        if (var > 0)
        {
            var--;
            nready += 5;
            printf("[%d][%lu]nready:%d, var:%d, signal..\n", idx, pthread_self(), nready, var);
            pthread_cond_signal(&g_cond);
        }
        pthread_mutex_unlock(&g_mutex);
        sleep(1);
    }
    return NULL;
}

int pthread_cond_demo(void)
{
    printf("pthread_cond_demo.\n");
    int i;

    pthread_mutex_init(&g_mutex, NULL);
    pthread_cond_init(&g_cond, NULL);

    for (i = 0; i < CONSUMERS_COUNT; i++)
        pthread_create(&g_thread[i], NULL, consume, (void *)&i);

    sleep(1);

    for (i = 0; i < PRODUCERS_COUNT; i++)
        pthread_create(&g_thread[CONSUMERS_COUNT + i], NULL, produce, (void *)&i);

//    pthread_cond_destroy(&g_cond);
//    pthread_mutex_destroy(&g_mutex);
    return 0;
}

