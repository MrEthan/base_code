#include "pthread_cond.h"
#include "inc/debug.h"
#include <stdio.h>
#include "list.h"

ListEntry *g_list = NULL;

int main()
{
	int iRet = 0;
    char *str1 = "hello";
    char *str2 = "world\n";
    ListEntry *p_node = NULL;

	printf("debug test\n");

    SPIDER_LOG(SPIDER_LEVEL_DEBUG, "this is debug, iRet(%d).\r\n", iRet);
    SPIDER_LOG(SPIDER_LEVEL_INFO, "this is info, iRet(%d).\r\n", iRet);
    SPIDER_LOG(SPIDER_LEVEL_INFO, "this is warning, iRet(%d).\r\n", iRet);

    printf("cond test\n");
	pthread_cond_demo();

    SPIDER_LOG(SPIDER_LEVEL_INFO, "list test.\r\n");
    p_node = list_append(&g_list, (void *)str1);
    printf("node0 addr:%p\n", p_node);

    p_node = list_append(&g_list, (void *)str2);
    printf("node0 addr:%p\n", p_node);

    p_node = list_nth_entry(g_list, 0);
    //str = list_nth_data(g_list, 0);
    printf("node0 addr:%p, data:%s\n", p_node, (char *)p_node->data);
    p_node = list_nth_entry(g_list, 1);
    printf("node1 addr:%p, data:%s\n", p_node, (char *)p_node->data);

	return 0;
}

