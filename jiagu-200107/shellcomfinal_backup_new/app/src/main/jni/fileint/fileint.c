#include "./dexdec/dexdec.h"
#include "./seclib/amac.h"
#include "./seclib/aes_mod.h"
#include "fileint.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <android/log.h>
#include <jni.h>
#define LOG_TAG "JNI_info_aes"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

typedef struct p_paramater
{
	char *binfilename;
	unsigned char *addr;
	int length;

}p_paramater_int;

static void* p_fileint(void *arg);

void file_integrity(const char *binfilename,const unsigned char *addr,int length)
{
	pthread_t p_file_int;
	p_paramater_int *paramater;

	paramater = (p_paramater_int *)malloc(sizeof(p_paramater_int));

	paramater->binfilename = binfilename;
	paramater->addr = addr;
	paramater->length = length;

	pthread_create(&p_file_int,NULL,&p_fileint,paramater);
}

static void* p_fileint(void *arg)
{
	p_paramater_int *paramater;
	FILE *file_bin;
	BIN_FILE_TYPE binfiletype;
	unsigned char key[16];
	unsigned char iv[16] = {0x0};
	unsigned char *pbinfile;
	unsigned char *ptemp;
	unsigned char *ptemp1;
	amac_ctx amac;
	unsigned char result1[16];
	unsigned char result2[16];

	paramater = (p_paramater_int *)arg;
	LOGI("fileint1");
	while(1)
	{
		analysis_bin_file(paramater->binfilename,&binfiletype);
		file_bin = fopen(paramater->binfilename, "r");

		LOGI("fileint2");
		fseek(file_bin,binfiletype.key_start,SEEK_SET);
		fread(key,1,binfiletype.key_size,file_bin);

		pbinfile = (unsigned char *)malloc(binfiletype.dex_size);

		ptemp = (unsigned char *)malloc((binfiletype.dex_size+16)-(binfiletype.dex_size%16));
		ptemp1 = (unsigned char *)malloc((binfiletype.dex_size+16)-(binfiletype.dex_size%16));

		fseek(file_bin,binfiletype.dex_start,SEEK_SET);
		fread(ptemp,1,(binfiletype.dex_size+16)-(binfiletype.dex_size%16),file_bin);

		AES_Decrypt_128_CBC(key,iv,ptemp,ptemp1,(binfiletype.dex_size+16)-(binfiletype.dex_size%16));

		memcpy(pbinfile,ptemp1,binfiletype.dex_size);

		free(ptemp);
		free(ptemp1);

		LOGI("fileint3");
		amac_init(&amac, key, key);

		amac_calc(&amac, pbinfile, binfiletype.dex_size);

		amac_result(&amac, result1);

		amac_init(&amac, key, key);

		amac_calc(&amac, paramater->addr, paramater->length);

		amac_result(&amac, result2);

		if( memcmp(result1, result2, 16) != 0)
			exit(1);

		sleep(600);

	}



	free(paramater);
}
