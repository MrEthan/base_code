#include <stdio.h>
#include "comm/base.h"
#include "list.h"
#include "pthread_cond.h"
#include "timer_posix.h"
#include "debug.h"
#include "ecdh.h"
#include "openssl/e_os2.h"

extern int encrypt_evp_aes_demo(void);

void hexbyte(char *out, uint8_t b) {
  static const char hextable[] = "0123456789abcdef";
  out[0] = hextable[b>>4];
  out[1] = hextable[b&0x0f];
}

extern int print_trace_test(void);

int main()
{
    //lstLib_test();
    //timer_test();
    print_trace_test();
	return 0;
}

