#ifndef MQ_H
#define MQ_H

/*
mqd_t MqOpen(const char *name, int oflag, mode_t mode, struct mq_attr *attr);
mqd_t MqClose(mqd_t mqdes);
*/


mqd_t mq_open(const char *name, int oflag, mode_t mode, struct mq_attr *attr);
mqd_t mq_setattr(mqd_t mqdes, struct mq_attr *newattr, struct mq_attr *oldattr);
int mq_send(mqd_t mqdes, const char *ptr, size_t len, unsigned int prio); 
ssize_t mq_receive(mqd_t mqdes, char *ptr, size_t len, unsigned int *prio); 
mqd_t mq_close(mqd_t mqdes);
mqd_t mq_unlink(const char *name); 


#endif

