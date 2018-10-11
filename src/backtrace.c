/* 用于打印函数调用栈 */

#include <time.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "debug.h"
#include "base.h"
#include <execinfo.h>


/* Obtain a backtrace and print it to @code{stdout}. */
void print_trace (void)
{
    void *array[10];
    int size;
    char **strings;
    int i;
    size = backtrace(array, 10);
    strings = backtrace_symbols(array, size);
    if(NULL == strings)
    {
        perror("backtrace_synbols");
        exit(EXIT_FAILURE);
    }

    printf ("Obtained %zd stack frames.\n", size);

    for (i = 0; i < size; i++)
        printf ("%s\n", strings[i]);

    free (strings);
    strings = NULL;
}

/* A dummy function to make the backtrace more interesting. */
void dummy_function (void)
{
    print_trace ();
}

int print_trace_test (void)
{
    dummy_function ();
    return 0;
}


