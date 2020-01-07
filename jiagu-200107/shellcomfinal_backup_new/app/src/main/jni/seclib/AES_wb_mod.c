#include "AES_wb_mod.h"
#include "AES_wb_decrypt.h"
#include <string.h>

#include <android/log.h>
#define LOG_TAG "JNI_info"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

void AES_wb_Decrypt_ecb(unsigned char *M,unsigned int *table,const unsigned char *c,unsigned char *p,int length)
{

	int i = 0;
	int n = length / (512*1024);

//	if(n == 0)
//		n = 1;
//	while((length / (16*n)) != 0)
//	{
//		AES_wb_decrypt(c+i*16,p+i*16,M,table);
//
//		i += n;
//		length = length - 16*n;
//	}

	while((length / 16) != 0)
	{
		AES_wb_decrypt(c+i*16,p+i*16,M,table);

		i += 1;
		length = length - 16;
	}



}
