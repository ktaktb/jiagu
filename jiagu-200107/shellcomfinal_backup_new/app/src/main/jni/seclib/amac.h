#ifndef _AMAC_H_
#define _AMAC_H_

#include "aes.h"

typedef struct amac_ctx
{
	unsigned char cache[16];
	AES_KEY key;
	AES_KEY key_last;
//	unsigned char key[16];
//	unsigned char key_last[16];
	int is_padding;

}amac_ctx;

void amac_init(amac_ctx* amac,const unsigned char* key,const unsigned char* key_last);
int amac_calc(amac_ctx* amac,const unsigned char *data,unsigned long length);
void amac_result(amac_ctx* amac,unsigned char *result);

#endif
