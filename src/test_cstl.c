#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <string.h>

#include "cstl/cstl_def.h"
#include "cstl/citerator.h"
#include "cstl/cstl_list_iterator.h"
#include "cstl/clist.h"
#include "cstl_list_aux.h"
#include "cstl/cvector.h"
#include "cstl/cdeque.h"
#include "cstl_vector_aux.h"
#include "cstl/cstring.h"
#include "cstl/cfunctional.h"
#include "cstl/cslist.h"
#include "cstl_algo_nonmutating.h"
#include "cstl_algo_nonmutating_private.h"

void print_val(const void* input, void* output)
{
    printf("print_val, var:%d\n", *(int *)input);
    return;
}

void cstl_test()
{
    list_iterator_t it_iter;
    list_t* plist = create_list(int);
    int ret = 0;

    list_init(plist);
    list_push_back(plist, 2);
    list_push_back(plist, 1);
    list_push_back(plist, 0);
    list_sort(plist);
    it_iter = list_begin(plist);
    it_iter = iterator_next(it_iter);

    ret = *(int*)iterator_get_pointer(it_iter);
    printf("ret(%d)\r\n", ret);

    it_iter = iterator_next(it_iter);
    ret = *(int*)iterator_get_pointer(it_iter);
    printf("ret(%d)\r\n", ret);

    algo_for_each(list_begin(plist), list_end(plist), print_val);

    list_destroy(plist);
    return;
}
