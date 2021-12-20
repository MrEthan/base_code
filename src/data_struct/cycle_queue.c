//
// Created by wxm_e on 2021/12/18.
//

#include "data_struct/cycle_queue.h"
#include "base.h"
#include "debug.h"
#include <malloc.h>
#include <string.h>

// 环形缓冲区数据结构,留一个空间作为标识
typedef struct {
    uint32_t head; // 头，从此处开始写 [0, size -1]
    uint32_t tail; // 尾，从此处开始读 [0, size -1]
    uint32_t size; // 缓冲区总大小
    uint8_t *buf;  // 实际使用的缓冲区
} CycleQueuePri;


/**
 * 向缓冲区尾部插入数据
 * @param thiz 缓冲区对象指针
 * @param data 待插入数据
 * @param data_len 待插入数据长度
 * @return 0成功 -1失败
 */
static int cycle_queue_write(CycleQueue *thiz, uint8_t *data, uint32_t data_len)
{
    if (!thiz || !data) {
        DERROR("illegal param\n");
        return -1;
    }
    uint32_t free_size = thiz->get_free_size(thiz);
    if (free_size < data_len) {
        DERROR("lack of space, free:%d data_len:%d\n", free_size, data_len);
        return -1;
    }
    CycleQueuePri *pri = (CycleQueuePri *)thiz->pri;
    // 根据下标是否溢出做不同的处理
    if (pri->head + data_len <= pri->size - 1) {
        // 未溢出，整段拷贝
        memcpy(&pri->buf[pri->head], data, data_len);
    } else {
        // 溢出，分段拷贝
        uint32_t part1 = pri->size - 1 - pri->head;
        memcpy(&pri->buf[pri->head], data, part1);
        memcpy(&pri->buf[0], data + part1, data_len - part1);
    }
    pri->head = (pri->head + data_len) % pri->size;
    return 0;
}

/**
 * 从缓冲区头部读数据
 * @param thiz 缓冲区对象指针
 * @param buf 待读取缓冲区
 * @param buf_len 传入缓冲区长度，传出实际读取的长度（取有效数据和缓冲区长度的较小值）
 * @return 0成功 -1失败
 */
static int cycle_queue_read(CycleQueue *thiz, uint8_t *buf, uint32_t *buf_len)
{
    if (!thiz || !buf || !buf_len || *buf_len <= 0) {
        DERROR("illegal param\n");
        return -1;
    }
    uint32_t data_size = thiz->get_data_size(thiz);

    if (data_size == 0) {
        // 当前缓冲区内无有效数据
        *buf_len = 0;
        return 0;
    }
    uint32_t read_len    = MIN(data_size, *buf_len);
    CycleQueuePri *pri = (CycleQueuePri *)thiz->pri;
    // 根据下标是否溢出做不同的处理
    if (pri->tail + read_len <= pri->size - 1) {
        // 未溢出，整段拷贝
        memcpy(buf, &pri->buf[pri->tail], read_len);
    } else {
        // 溢出，分段拷贝
        uint32_t part1 = pri->size - 1 - pri->tail;
        memcpy(buf, &pri->buf[pri->tail], part1);
        memcpy(buf + part1, &pri->buf[0], read_len - part1);
    }
    pri->tail = (pri->tail + read_len) % pri->size;
    *buf_len   = read_len;
    return 0;
}

/**
 * 获取缓冲区剩余空间长度
 * @param thiz 缓冲区对象
 * @return 缓冲区剩余空间长度
 */
static uint32_t cycle_queue_get_free_size(CycleQueue *thiz)
{
    if (!thiz) {
        DERROR("illegal param\n");
        return -1;
    }
    CycleQueuePri *pri = (CycleQueuePri *)thiz->pri;
    uint32_t free_size;
    if (pri->head == pri->tail) {
        free_size = pri->size -1;
    } else if (pri->head > pri->tail) {
        free_size = pri->size - (pri->head - pri->tail) - 1;
    } else {
        free_size = pri->tail - pri->head - 1;
    }
    return free_size;
}

/**
 * 获取缓冲区有效数据长度
 * @param thiz 缓冲区对象
 * @return 缓冲区有效数据长度
 */
static uint32_t cycle_queue_get_data_size(CycleQueue *thiz)
{
    if (!thiz) {
        DERROR("illegal param\n");
        return -1;
    }
    CycleQueuePri *pri = (CycleQueuePri *)thiz->pri;
    uint32_t data_size;
    if (pri->head >= pri->tail) {
        data_size = pri->head - pri->tail;
    } else {
        data_size = pri->size - pri->tail + pri->head;
    }
    return data_size;
}

/**
 * 创建环形缓冲区对象
 * @param size 缓冲区大小
 * @return NULL失败 非NULL，创建成功的环形缓冲区对象指针
 */
CycleQueue *cycle_queue_create(uint32_t size)
{
    CycleQueue *thiz = NULL;
    uint8_t *buf = NULL;
    if (size > 0) {
        thiz = (CycleQueue *)malloc(sizeof(CycleQueue) + sizeof(CycleQueuePri));
        if (NULL == thiz) {
            DERROR("malloc thiz fail\n");
            return NULL;
        }
        buf = (uint8_t *)malloc(size);
        if (NULL == buf)
        {
            DERROR("malloc buf fail\n");
            free(thiz);
            return NULL;
        }
        memset(thiz, 0, sizeof(CycleQueue) + sizeof(CycleQueuePri));
        memset(buf, 0, size);
        CycleQueuePri *pri = (CycleQueuePri *)((uint8_t *)thiz + sizeof(CycleQueue));
        pri->size = size;
        pri->head = 0;
        pri->tail = 0;
        pri->buf  = buf;
        thiz->pri = pri;
        thiz->write = cycle_queue_write;
        thiz->read = cycle_queue_read;
        thiz->get_free_size = cycle_queue_get_free_size;
        thiz->get_data_size = cycle_queue_get_data_size;
    }

    return thiz;
}

/**
 * 销毁环形缓冲区
 * @param thiz 环形缓冲区对象
 */
void cycle_queue_destroy(CycleQueue *thiz)
{
    if (thiz) {
        free(thiz);
    }
}
