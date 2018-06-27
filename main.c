#include "pthread_cond.h"
#include "inc/debug.h"
#include <stdio.h>

extern void cstl_test();

int main()
{
	int iRet = 0;
	printf("debug test\n");

    SPIDER_LOG(SPIDER_LEVEL_DEBUG, "this is debug, iRet(%d).\r\n", iRet);
    SPIDER_LOG(SPIDER_LEVEL_INFO, "this is info, iRet(%d).\r\n", iRet);
    SPIDER_LOG(SPIDER_LEVEL_INFO, "this is warning, iRet(%d).\r\n", iRet);

    printf("cond test\n");
	pthread_cond_demo();
	printf("libcstl test\n");
    cstl_test();

	return 0;
}

