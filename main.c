#include <stdio.h>
#include "list.h"
#include "pthread_cond.h"
#include "timer_posix.h"
#include "debug.h"

ListEntry *g_list = NULL;

int main()
{
	int iRet = 0;
    char *str1 = "hello";
    char *str2 = "world\n";
    ListEntry *p_node = NULL;

//	printf("debug test\n");
//	DERROR("derror test. iRet:%d\n", iRet);
//    DWARN("derror test. iRet:%d\n", iRet);
//    DDEBUG("derror test. iRet:%d\n", iRet);
//    DINFO("derror test. iRet:%d\n", iRet);

//    printf("cond test\n");
//	pthread_cond_demo();

//    p_node = list_append(&g_list, (void *)str1);
//    printf("node0 addr:%p\n", p_node);

//    p_node = list_append(&g_list, (void *)str2);
//    printf("node0 addr:%p\n", p_node);

//    p_node = list_nth_entry(g_list, 0);
//    //str = list_nth_data(g_list, 0);
//    printf("node0 addr:%p, data:%s\n", p_node, (char *)p_node->data);
//    p_node = list_nth_entry(g_list, 1);
//    printf("node1 addr:%p, data:%s\n", p_node, (char *)p_node->data);

    printf("timer epoll test.\n");
    timer_test();
	ecdh_test();
	return 0;
}

