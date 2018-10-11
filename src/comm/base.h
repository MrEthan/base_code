#ifndef BASE_H
#define BASE_H

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>
#include "debug.h"

#define SAFE_FREE(p)  \
do{                   \
    if (NULL != (p)){ \
        free(p);      \
        p = NULL;     \
    }                 \
}while(0)


#define SAFE_CLOSE(fd) \
do{                    \
    if(fd > 0){        \
        close(fd);      \
        fd = -1;        \
    }                   \
}while(0)

#ifndef container_of
#define container_of(ptr, type, member) ({          \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})
#endif


int base_mutex_lock(pthread_mutex_t *mutex);
int base_mutex_trylock(pthread_mutex_t *mutex);
int base_mutex_unlock(pthread_mutex_t *mutex);

int base_rwlock_rdlock(pthread_rwlock_t *rwlock);
int base_rwlock_tryrdlock(pthread_rwlock_t *rwlock);
int base_rwlock_wrlock(pthread_rwlock_t *rwlock);
int base_rwlock_trywrlock(pthread_rwlock_t *rwlock);
int base_rwlock_unlock(pthread_rwlock_t *rwlock);

long long get_time_ms(void);

#endif //BASE_H
