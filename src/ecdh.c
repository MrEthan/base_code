#include "comm/base.h"
#include "../lib/ecc/uECC.h"
#include "ecdh.h"

typedef struct {
	unsigned char pub_key[2 * uECC_MAX_WORDS];
	unsigned char pri_key[uECC_MAX_WORDS];
	unsigned char secret[uECC_MAX_WORDS];
}ecdh_type;

struct uECC_Curve_t *curves = uECC_secp192r1();

ecdh_type g_ecdh_key1;
ecdh_type g_ecdh_key2;

int ecdh_test()
{
	int ret = 0;

	memset(&g_ecdh_key1, 0, sizeof(g_ecdh_key1));
	memset(&g_ecdh_key2, sizeof(g_ecdh_key2));

	//alice
	ret = uECC_make_key(g_ecdh_key1.pub_key, g_ecdh_key1.pri_key, curves);
	if (0 == ret){
		DERROR("make key1 failed, ret:%d\n", ret);
	}

	//bob
	ret = uECC_make_key(g_ecdh_key2.pub_key, g_ecdh_key2.pri_key, curves);
	if (0 == ret){
		DERROR("make key2 failed, ret:%d\n", ret);
	}

	//alice
	ret = uECC_shared_secret(g_ecdh_key2.pub_key, g_ecdh_key1.pri_key, g_ecdh_key1.secret, curves);
	if (0 == ret){
		DERROR("make secret1 failed, ret:%d\n", ret);
	}

	//bob
	ret = uECC_shared_secret(g_ecdh_key1.pub_key, g_ecdh_key2.pri_key, g_ecdh_key2.secret, curves);
	if (0 == ret){
		DERROR("make secret1 failed, ret:%d\n", ret);
	}

	if (0 != memcmp(g_ecdh_key1.secret, g_ecdh_key2.secret, sizeof(g_ecdh_key2.secret)){
		DERROR("key1 != key2\n");
	}

	return;
}




