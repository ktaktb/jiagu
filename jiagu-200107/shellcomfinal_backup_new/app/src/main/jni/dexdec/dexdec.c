#include "dexdec.h"
#include <stdio.h>
#include <android/log.h>
#include <jni.h>
#define LOG_TAG "JNI_info_aes"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

#if defined(_TEST_DEBUG_ANDROID)
#define LOGTEST(...)   __android_log_print(3,"testsec", __VA_ARGS__)
#else
#define LOGTEST(...)
#endif
#include <errno.h>

void analysis_bin_file(const char *binfilename, BIN_FILE_TYPE *binfiletype)
{
	LOGTEST("analysis_bin_file 0");
	FILE *filebin;
	int dex_file_length;
	unsigned int temp;
	LOGTEST("analysis_bin_file 00");
	errno = 0;
	filebin = fopen(binfilename,"rb");
	if(filebin == NULL){
		LOGTEST("open err");
	}
	LOGTEST("errno %d", errno);
	LOGTEST("%s", binfilename);
	LOGTEST("analysis_bin_file 000");

	int i = fseek(filebin,-0,2);
    errno = 0;
	if(i == 0){
		LOGTEST("seek ok");
	}

	LOGTEST("err %d", errno);
	LOGTEST("analysis_bin_file 0000");
	dex_file_length = ftell(filebin);
	LOGTEST("analysis_bin_file 00000");
	fseek(filebin,0,0);
	LOGTEST("analysis_bin_file 1");
	binfiletype->key_start = 12;
	binfiletype->key_size = 16;
	LOGTEST("analysis_bin_file 2");
	fseek(filebin,(binfiletype->key_start + binfiletype->key_size + 4 + 4),SEEK_CUR);
	fread(&temp, 1, 4, filebin);
	LOGTEST("analysis_bin_file 3");
	binfiletype->sosign_start = binfiletype->key_start + binfiletype->key_size + 4 + 4 + 4;
	binfiletype->sosign_size = temp;
	LOGTEST("analysis_bin_file 4");
	fseek(filebin,(binfiletype->sosign_size + 4 + 4), SEEK_CUR);
	fread(&temp, 1, 4, filebin);
	LOGTEST("analysis_bin_file 5");
	binfiletype->dexsign_start = binfiletype->sosign_start + binfiletype->sosign_size + 4 + 4 + 4;
	binfiletype->dexsign_size = temp;
	LOGTEST("analysis_bin_file 6");
	fseek(filebin, (binfiletype->dexsign_size + 4 + 4), SEEK_CUR);
	fread(&temp, 1, 4, filebin);
	LOGTEST("analysis_bin_file 7");
	binfiletype->dex_start = binfiletype->dexsign_start + binfiletype->dexsign_size + 4 + 4 + 4 + 4;
	binfiletype->unzdex_size = temp;
	LOGTEST("analysis_bin_file 8");

	fseek(filebin, 0, SEEK_CUR);
	fread(&temp, 1, 4, filebin);

	LOGTEST("analysis_bin_file 9");
	binfiletype->dex_size = temp;

	binfiletype->file_size = dex_file_length;
	LOGTEST("analysis_bin_file 10");
	fclose(filebin);
}


/*
void analysis_bin_file(const char *binfilename, BIN_FILE_TYPE *binfiletype)
{
	FILE *filebin;
	int dex_file_length;
	unsigned int temp;

	filebin = fopen(binfilename,"r+");
	fseek(filebin,0L,SEEK_END);
	dex_file_length = ftell(filebin);
	fseek(filebin,0L,0L);

	binfiletype->key_start = 12;
	binfiletype->key_size = 16;

	fseek(filebin,(binfiletype->key_start + binfiletype->key_size + 4 + 4),SEEK_CUR);
	fread(&temp, 1, 4, filebin);

	binfiletype->sosign_start = binfiletype->key_start + binfiletype->key_size + 4 + 4 + 4;
	binfiletype->sosign_size = temp;

	fseek(filebin,(binfiletype->sosign_size + 4 + 4), SEEK_CUR);
	fread(&temp, 1, 4, filebin);

	binfiletype->dexsign_start = binfiletype->sosign_start + binfiletype->sosign_size + 4 + 4 + 4;
	binfiletype->dexsign_size = temp;

	fseek(filebin, (binfiletype->dexsign_size + 4 + 4), SEEK_CUR);
	fread(&temp, 1, 4, filebin);

	binfiletype->dex_start = binfiletype->dexsign_start + binfiletype->dexsign_size + 4 + 4 + 4;
	binfiletype->dex_size = temp;

	binfiletype->file_size = dex_file_length;

	fclose(filebin);
}
*/
