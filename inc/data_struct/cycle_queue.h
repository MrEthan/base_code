//
// Created by wxm_e on 2021/12/18.
// 循环缓冲区实现
//

#ifndef BASE_CODE_CYCLE_QUEUE_H
#define BASE_CODE_CYCLE_QUEUE_H

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct CycleQueue {

    /**
     * 内部数据
     */
    void *pri;

    /**
     * 向缓冲区尾部插入数据
     * @param q 缓冲区对象指针
     * @param data 待插入数据
     * @param data_len 待插入数据长度
     * @return 0成功 -1失败
     */
    int (*write)(struct CycleQueue *thiz, uint8_t *data, uint32_t data_len);

    /**
     * 从缓冲区头部读数据
     * @param q 缓冲区对象指针
     * @param buf 待读取缓冲区
     * @param buf_len 传入缓冲区长度，传出实际读取的长度（取有效数据和缓冲区长度的较小值）
     * @return 0成功 -1失败
     */
    int (*read)(struct CycleQueue *thiz, uint8_t *buf, uint32_t *buf_len);

    /**
     * 获取缓冲区剩余空间长度
     * @param q 缓冲区对象
     * @return 缓冲区剩余空间长度
     */
    uint32_t (*get_free_size)(struct CycleQueue *thiz);

    /**
     * 获取缓冲区有效数据长度
     * @param q 缓冲区对象
     * @return 缓冲区有效数据长度
     */
    uint32_t (*get_data_size)(struct CycleQueue *thiz);
} CycleQueue;

/**
 * 创建环形缓冲区对象
 * @param size 缓冲区大小
 * @return NULL失败 非NULL，创建成功的环形缓冲区对象指针
 */
CycleQueue *cycle_queue_create(uint32_t size);

/**
 * 销毁环形缓冲区
 * @param q 环形缓冲区对象
 */
void cycle_queue_destroy(CycleQueue *thiz);

#ifdef __cplusplus
}
#endif

#endif // BASE_CODE_CYCLE_QUEUE_H
