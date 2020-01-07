#include "aes.h"
#include "aes_mod.h"
#include <stdlib.h>
#include <android/log.h>
#include <jni.h>
#define LOG_TAG "JNI_info_aes"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)


void AES_Encrypt_128_ECB(const unsigned char *userk,const unsigned char *p,unsigned char *c,int length)
{
	AES_KEY aes_key;
	int i;
	unsigned char temp[16];

	i = 0;

	private_AES_set_encrypt_key(userk,128,&aes_key);

	while((length / 16) != 0)
	{
		AES_encrypt(p+i*16,c+i*16,&aes_key);

		i++;
		length = length - 16;
	}

	if(length != 0)
	{
		memcpy(&temp[0],(p+i*16),length);

		memset(&temp[length],0,(16-length));

		AES_encrypt(&temp[0],c+i*16,&aes_key);

	}

}
void AES_Decrypt_128_ECB(const unsigned char *userk,const unsigned char *c,unsigned char *p,int length)
{
	AES_KEY aes_key;
	int i;
	unsigned char temp[16];

	i = 0;

	private_AES_set_decrypt_key(userk,128,&aes_key);

	while((length / 16) != 0)
	{
		AES_decrypt(c+i*16,p+i*16,&aes_key);

		i++;
		length = length - 16;
	}

	if(length != 0)
	{
		memcpy(&temp[0],(c+i*16),length);

		memset(&temp[length],0,(16-length));

		AES_decrypt(&temp[0],p+i*16,&aes_key);

	}

}

void AES_Encrypt_128_CBC(const unsigned char *userk,const unsigned char *iv,const unsigned char *p,unsigned char *c,int length)
{
	AES_KEY aes_key;
	int i,j;
	unsigned char temp[16];

	i = 0;

	private_AES_set_encrypt_key(userk,128,&aes_key);
	while((length / 16) != 0)
	{
		if( i != 0)
		{
			memcpy(temp,p+i*16,16);
			for(j = 0;j < 16;j++)
			{
				temp[j] ^= *(c+(i-1)*16+j);
			}
		}
		else
		{
			memcpy(&temp[0],p,16);
			for(j = 0;j < 16;j++)
			{
				temp[j] ^= *(iv+j);
			}
		}
		AES_encrypt(temp,c+i*16,&aes_key);

		i++;
		length = length - 16;
	}

	if(length != 0)
	{
		memcpy(&temp[0],(p+i*16),length);

		memset(&temp[length],0,(16-length));

		if(i != 0)
		{
			for(j = 0;j < 16;j++)
			{
				temp[j] ^= *(c+(i-1)*16+j);
			}
		}
		else
		{
			for(j = 0;j < 16;j++)
			{
				temp[j] ^= *(iv+j);
			}
		}
		AES_encrypt(&temp[0],c+i*16,&aes_key);

	}
}
void AES_Decrypt_128_CBC(const unsigned char *userk,const unsigned char *iv,const unsigned char *c,unsigned char *p,int length)
{
	AES_KEY aes_key;
	int i,j;
	unsigned char temp[16];

	i = 0;

	private_AES_set_decrypt_key(userk,128,&aes_key);

	while((length / 16) != 0)
	{
		memcpy(&temp[0],c+i*16,16);
		AES_decrypt(temp,p+i*16,&aes_key);

		if(i != 0)
		{
			for(j = 0;j < 16;j++)
			{
				(*(p+i*16+j)) ^= (*(c+(i-1)*16+j));
			}
		}
		else
		{
			for(j = 0;j < 16;j++)
			{
				(*(p+j)) ^= *(iv+j);
			}
		}

		i++;
		length = length - 16;
	}
	if(length != 0)
	{
		memcpy(&temp[0],(c+i*16),length);

		memset(&temp[length],0,(16-length));

		AES_decrypt(temp,p+i*16,&aes_key);

		if( i != 0)
		{
			for(j = 0;j < 16;j++)
			{
				*(p+i*16+j) ^= *(c+(i-1)*16+j);
			}
		}
		else
		{
			for(j = 0;j < 16;j++)
			{
				*(p+i*16+j) ^= *(iv+j);
			}
		}

	}
}

/*
 *AES OFB加解密的实现
 *AES userk为密钥，iv为初始向量，in为输入，out为输出，len是输入in的长度
 *AES OFB by zhr
 */
void AES_Encrypt_128_OFB(const unsigned char *userk,const unsigned char *iv,const unsigned char *in,unsigned char *out,int len)
{
	AES_KEY aes_key;
	int n=0;
	unsigned char temp[16] = {0};
	memcpy(temp, iv, 16);

	private_AES_set_encrypt_key(userk,128,&aes_key);

	while(len>=16){
		AES_encrypt(temp, temp, &aes_key);
		for(; n<16; ++n)
			*(size_t*)(out + n) = *(size_t*)(in + n) ^ *(size_t*)(temp + n);
		len -= 16;
		out += 16;
		in += 16;
		n = 0;
	}
	if(len){
		AES_encrypt(temp, temp, &aes_key);
		while(len--){
			out[n] = in[n] ^ temp[n];
			++n;
		}
	}
	return;
}
