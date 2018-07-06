#include <stdio.h>
#include <pthread.h>
#include "debug.h"
#include "base.h"

int base_mutex_lock(pthread_mutex_t *mutex)
{
    int ret = -1;

    ret = pthread_mutex_lock(mutex);

    return ret;
}

int base_mutex_trylock(pthread_mutex_t *mutex)
{
    int ret = -1;

    ret = pthread_mutex_trylock(mutex);

    return ret;
}

int base_mutex_unlock(pthread_mutex_t *mutex)
{
    int ret = -1;

    ret = pthread_mutex_unlock(mutex);

    return ret;
}


int base_rwlock_rdlock(pthread_rwlock_t *rwlock)
{
    int ret = -1;

    ret = pthread_rwlock_rdlock(rwlock);

    return ret;
}

int base_rwlock_tryrdlock(pthread_rwlock_t *rwlock)
{
    int ret = -1;

    ret = pthread_rwlock_tryrdlock(rwlock);

    return ret;
}


int base_rwlock_wrlock(pthread_rwlock_t *rwlock)
{
    int ret = -1;

    ret = pthread_rwlock_wrlock(rwlock);

    return ret;
}

int base_rwlock_trywrlock(pthread_rwlock_t *rwlock)
{
    int ret = -1;

    ret = pthread_rwlock_trywrlock(rwlock);

    return ret;
}

int base_rwlock_unlock(pthread_rwlock_t *rwlock)
{
    int ret = -1;

    ret = pthread_rwlock_unlock(rwlock);

    return ret;
}


