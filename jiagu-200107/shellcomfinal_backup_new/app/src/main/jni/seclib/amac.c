#include "amac.h"
#include <string.h>

void amac_init(amac_ctx* amac,const unsigned char* key,const unsigned char* key_last)
{
	memset(&(amac->cache)[0],0,16);
//	memcpy(&(amac->key)[0],key,16);
//	memcpy(&(amac->key_last)[0],key_last,16);
	private_AES_set_encrypt_key(key,128,&(amac->key));
	private_AES_set_encrypt_key(key_last,128,&(amac->key_last));

	amac->is_padding = 0;
}
int amac_calc(amac_ctx* amac,const unsigned char *data,unsigned long length)
{
	int i,j;
	unsigned char temp[16];

	if(1 == amac->is_padding)
		return 0;

	i = 0;

	while((length / 16) != 0)
	{
		memcpy(&temp[0],(data+i*16),16);

		for(j = 0;j < 16;j++)
		{
			 temp[j] ^= amac->cache[j];
		}

		AES_encrypt(&temp[0],&temp[0],&(amac->key));

		memcpy(&(amac->cache)[0],temp,16);

		i++;
		length = length - 16;
	}

	if(length != 0)
	{
		memcpy(&temp[0],(data+i*16),length);

		memset(&temp[length],0,(16-length));

		for(j = 0;j < 16;j++)
		{
			 temp[j] ^= amac->cache[j];
		}

		AES_encrypt(&temp[0],&temp[0],&(amac->key));

		memcpy(&(amac->cache[0]),temp,16);

		amac->is_padding = 1;
	}

	return 1;
}

void amac_result(amac_ctx* amac,unsigned char *result)
{
	AES_encrypt(&(amac->cache)[0],&(amac->cache)[0],&(amac->key_last));

	memcpy(result,&(amac->cache)[0],16);
}
