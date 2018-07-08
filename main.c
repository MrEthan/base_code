#include <stdio.h>
#include "comm/base.h"
#include "list.h"
#include "pthread_cond.h"
#include "timer_posix.h"
#include "debug.h"
#include "ecdh.h"

int main()
{
    DDEBUG("ecdh test.\n");
	ecdh_test();

	return 0;
}

