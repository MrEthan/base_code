#include <setjmp.h>
#include <stdlib.h>
#include "comm/debug.h"

void setjmp_test()
{
    jmp_buf env;
    int i;

    i = setjmp(env);
    DDEBUG("i = %d\n", i);
    if (i != 0) {
        exit(0);
    }

    longjmp(env, 2);
    DDEBUG("This line does not get printed\n");
}