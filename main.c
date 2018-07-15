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

/*  */
int main()
{
    char c[100] = {0};
    char d[100];

    hexbyte(c, 100);
    dumpBuffer_char(NULL, c, strlen(c));
	return 0;
}

