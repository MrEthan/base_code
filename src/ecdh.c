#include "comm/base.h"
#include "uECC.h"
#include "ecdh.h"

#define ECC_KEY_LEN 24

typedef struct {
	unsigned char pub_key[2 * ECC_KEY_LEN];
	unsigned char pri_key[ECC_KEY_LEN];
	unsigned char secret[ECC_KEY_LEN];
}ecdh_type;

ecdh_type g_ecdh_key1;
ecdh_type g_ecdh_key2;

int ecdh_test()
{
	int ret = 0;
    long long time_start = get_time_ms();
    long long time_now = 0;

	memset(&g_ecdh_key1, 0, sizeof(g_ecdh_key1));
	memset(&g_ecdh_key2, 0, sizeof(g_ecdh_key2));

	//alice
	ret = uECC_make_key(g_ecdh_key1.pub_key, g_ecdh_key1.pri_key, uECC_secp192r1());
	if (0 == ret){
		DERROR("make key1 failed, ret:%d\n", ret);
	}

	//bob
	ret = uECC_make_key(g_ecdh_key2.pub_key, g_ecdh_key2.pri_key, uECC_secp192r1());
	if (0 == ret){
		DERROR("make key2 failed, ret:%d\n", ret);
	}

	//alice
	ret = uECC_shared_secret(g_ecdh_key2.pub_key, g_ecdh_key1.pri_key, g_ecdh_key1.secret, uECC_secp192r1());
	if (0 == ret){
		DERROR("make secret1 failed, ret:%d\n", ret);
	}

	//bob
	ret = uECC_shared_secret(g_ecdh_key1.pub_key, g_ecdh_key2.pri_key, g_ecdh_key2.secret, uECC_secp192r1());
	if (0 == ret){
		DERROR("make secret1 failed, ret:%d\n", ret);
	}

    //”√ ±
    time_now = get_time_ms();
    DDEBUG("time_start:%lld, time_now:%lld, spend time:%lld ms\n", time_start, time_now, (time_now - time_start));

	if (0 != memcmp(g_ecdh_key1.secret, g_ecdh_key2.secret, sizeof(g_ecdh_key2.secret))){
		DERROR("key1 != key2\n");
	}else{
	    DINFO("key1 == key2. ecc success.\n");
        dumpBuffer(__FUNCTION__, g_ecdh_key1.secret, sizeof(g_ecdh_key1.secret));
        dumpBuffer(__FUNCTION__, g_ecdh_key2.secret, sizeof(g_ecdh_key2.secret));
	}

//    dumpBuffer("g_ecdh_key1 pub_key", g_ecdh_key1.pub_key, sizeof(g_ecdh_key1.pub_key));
//    dumpBuffer("g_ecdh_key1 pri_key", g_ecdh_key1.pri_key, sizeof(g_ecdh_key1.pri_key));
//    dumpBuffer("g_ecdh_key1 secret", g_ecdh_key1.secret, sizeof(g_ecdh_key1.secret));
//    dumpBuffer("g_ecdh_key2 pub_key", g_ecdh_key2.pub_key, sizeof(g_ecdh_key2.pub_key));
//    dumpBuffer("g_ecdh_key2 pri_key", g_ecdh_key2.pri_key, sizeof(g_ecdh_key2.pri_key));
//    dumpBuffer("g_ecdh_key2 secret", g_ecdh_key2.secret, sizeof(g_ecdh_key2.secret));

	return 0;
}




