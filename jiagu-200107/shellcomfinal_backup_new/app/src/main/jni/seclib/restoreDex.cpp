#include <iostream>
#include <map>
#include <cstring>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <assert.h>
#include "zlib.h"
#include <android/log.h>
#include "restoreDex.h"
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <sys/mman.h>
#pragma comment(lib,"ZLib.lib")
#if defined(_TEST_DEBUG_ANDROID)
#define LOGTEST(...)   __android_log_print(3,"testsec", __VA_ARGS__)
#else
#define LOGTEST(...)
#endif
//#include "aes.h"
//#include "aes_locl.h"
extern "C"
{
#include "aes.h"
#include "aes_mod.h"
#include "./seclib/AES_wb_mod.h"
	extern void AES_Decrypt_128_CBC(const unsigned char *userk, const unsigned char *iv, const unsigned char *c, unsigned char *p, int length);
	extern void AES_wb_Decrypt_ecb(unsigned char *M,unsigned int *table,const unsigned char *c,unsigned char *p,int length);

}

using namespace std;
//typedef map<string, mapl> MY_MAP;
MY_MAP my_Map;
//typedef uint8_t             u1;
u1 *odexmemoff;
unsigned char key[17];
unsigned char *M;
unsigned int *table;
unsigned int odexoffdex;
char pack_name[200];
unsigned int odexsize;
unsigned char *encdata;
FILETYPE *fileType;
double writebacktme;

unsigned int hex2oct(unsigned char* hex)
{
	unsigned int oct = 0;
	for (int i = 3, tmp = 1; i >= 0; --i) {
		oct += (unsigned int)hex[i] * tmp;
		tmp *= 256;
	}
	return oct;
}

void stroeoatbuf(u1 *todexmemoff, size_t length){
	odexmemoff = todexmemoff;
	odexsize = length;
	odexoffdex = 0;
}


void stroeodexbuf(u1 *todexmemoff, size_t length){
	odexmemoff = todexmemoff;
	odexsize = length;
	unsigned char temp[4];
	unsigned char buf[4];
	memcpy(temp, odexmemoff+8, 4);
	for(int i=0; i<4; i++){
	    buf[3-i] = temp[i];
	}
	odexoffdex = hex2oct(buf);
	LOGTEST("dexoff %x", (odexmemoff + odexoffdex));
	return odexoffdex;
}

void decryptCode( unsigned char *inData, unsigned char *outData, int length)
{

	/*****锟斤拷锟斤拷*****/
	LOGTEST("start decrypt dex by whitebox -------- AES\n");
	AES_wb_Decrypt_ecb(M, table, inData, outData,length);

}
void cppkname(char pk_tname[]){
	memcpy( pack_name, pk_tname, 200);

}
unsigned int getdexoff(char pk_tname[]){
	char odexpath[256];
	sprintf(odexpath,"data/data/%s/files/classes.dex", pk_tname);
	LOGTEST("odexpath :%s", odexpath);
	errno = 0;
	FILE *odex = fopen(odexpath, "rb");
	if(odex == NULL){
		LOGTEST("odex open error");
	}
    //LOGTEST("open err %d", errno);
    errno = 0;
    int i = fseek(odex, 8, 0);
    if(i == 0){
    	LOGTEST("fseek ok");
    }
    //LOGTEST("fseek err %d", errno);
    long foff = ftell(odex);
    LOGTEST("%d",foff);

    unsigned char temp[4];
    unsigned char buf[4];
    errno = 0;
    fread(temp, 1, 4, odex);

    for(int i=0; i<4; i++){
    	buf[3-i] = temp[i];
    }
    LOGTEST("dexoff %d", hex2oct(buf));
    fclose(odex);
    odexoffdex = hex2oct(buf);
    return odexoffdex;
}

JNIEXPORT void JNICALL Java_com_example_forshell_CustomerClassLoader_writeback( JNIEnv* env, jclass thiz, jstring name)
 {
	LOGTEST("in function writeback***************    ");

	MY_MAP::iterator it;
    int cmp = 1;

	char* str;
	str = env->GetStringUTFChars(name,false);

	if(str==NULL){
		LOGTEST("str1: ");
	return NULL;
	}

	it = my_Map.find(str);


	if (it == my_Map.end()){
		LOGTEST("TA END LE ");
		return;
	}
	int sizemethod = it->second.methodsize;
	int encsize = it -> second.encodesum;
	int encoff = it -> second.encodeoff;
	unsigned char *ptemp = (unsigned char *)malloc(encsize * sizeof(unsigned char));
	unsigned char *ptemp1 = (unsigned char *)malloc(encsize * sizeof(unsigned char));
	memset(ptemp, 0, encsize);
	memset(ptemp1, 0, encsize);
	memcpy(ptemp, (unsigned char *)encdata + encoff, encsize);
	decryptCode(ptemp, ptemp1, encsize);

    unsigned int count = 0;
	for (int j = 0; j < sizemethod; ++j) {

		METHODCODE *tmpdata;
		tmpdata = &(it->second.methodcode[j]);

		int tmd = mprotect(odexmemoff, odexsize, PROT_READ | PROT_WRITE | PROT_EXEC);
		memcpy((char *)odexmemoff + odexoffdex + tmpdata->dexoff, ptemp1 + count, tmpdata->dexsize );
        count = count + tmpdata->dexsize;
	}

	free(str);
}

/*
 * dexpath:锟节匡拷dex锟侥硷拷
 * datapath锟斤拷dump锟斤拷锟斤拷锟侥硷拷锟斤拷insns锟斤拷锟斤拷
 * classFile锟斤拷锟斤拷锟斤拷偏锟斤拷
 *
 */


void analysis_class_file( fstream& classFile, unsigned char tkey[]){
	LOGTEST("ans_class_files");
	string  l;
	unsigned int temp = 0;
	unsigned int classSize;

	getline(classFile, l);
	//LOGTEST("line %s   ",l.c_str());
	temp = atoi(l.c_str());
	fileType =(FILETYPE *) malloc(sizeof(unsigned int) + temp * 4);
    classSize = temp;
	for (int i = 0; i < classSize; i++)
	{
		// get the class name
		string line = "";
		getline(classFile, line);

		char *b = (char*)malloc(sizeof(char)*(line.size()));
		for (int k = 0; k != (line.size()); ++k) {
			b[k] = line[k];
		}
		b[(line.size()-1)] = '\0';
		int m = line.size();

		line = "";
		getline(classFile, line);
		temp = 0;
		temp = atoi(line.c_str());
		fileType->classList[i] = (CLASSDATA *)malloc(sizeof(char *) + sizeof(unsigned int) + temp * 4 * sizeof(unsigned int));
		fileType->classList[i]->className = b;
		fileType->classList[i]->methodSize = temp;

		for (int j = 0; j < fileType->classList[i]->methodSize; j++)
		{ 
			line = "";
			getline(classFile, line);
			char *a = (char*)malloc(sizeof(char) * (line.size() +1));
			for (int k = 0; k != line.size(); ++k) {
				a[k] = line[k];
			}
			a[line.size()] = '\0';
			char *token = strtok(a , " ");
			temp = 0;
			temp = atoi(token);
			fileType->classList[i]->codeList[j].dexoff = temp;
           // LOGTEST("dexoff %d", temp);
			token = strtok(NULL, " ");
			temp = 0;
			temp = atoi(token);
			fileType->classList[i]->codeList[j].dexsize = temp;
		}
		unsigned int ensize = 0;
		unsigned int enoff = 0;
		unsigned int beforesize = 0;
		if( fileType->classList[i]->methodSize != 0){
            line = "";
            getline(classFile, line);
            temp = 0;
            temp = atoi(line.c_str());
            beforesize = temp;
           // LOGTEST("beforesize %d", beforesize);


            line = "";
            getline(classFile, line);
		    temp = 0;
		    temp = atoi(line.c_str());
		    ensize = temp;
           // LOGTEST("ensize %d", ensize);


            line = "";
            getline(classFile, line);
		    temp = 0;
		    temp = atoi(line.c_str());
		    enoff = temp;
		   // LOGTEST("enoff %d", enoff);
		}
		if (fileType->classList[i]->methodSize != 0) {
			my_Map[fileType->classList[i]->className].methodsize = fileType->classList[i]->methodSize;
			my_Map[fileType->classList[i]->className].methodcode = fileType->classList[i]->codeList;
			my_Map[fileType->classList[i]->className].encodesum = ensize;
			my_Map[fileType->classList[i]->className].encodeoff = enoff;

		}

	}

    char datapath[256];
    sprintf(datapath, "/data/data/%s/files/codedata.txt",pack_name);
	FILE *encpath = fopen(datapath, "rb");
	if(encpath == NULL ){
		LOGTEST("encpath open file: %d",errno);
	}

	long filesize;
	fseek(encpath, 0, SEEK_END);
	filesize = ftell(encpath);
	encdata = (unsigned char *)malloc(sizeof(unsigned char) * filesize);

	int i = fseek(encpath, 0, SEEK_SET);
    fread(encdata, 1, filesize, encpath);
    mprotect(encdata, filesize, PROT_READ | PROT_WRITE | PROT_EXEC);
    fclose(encpath);




	char openpath[256];
	FILE *fp_M;
	FILE *fp_t;
	M = (unsigned char *) malloc(11 * 128 * 16 * sizeof(unsigned char));
	sprintf(openpath, "/data/data/%s/files/inv_M.m", pack_name);
	fp_M = fopen(openpath, "rb+");
	if(fopen(openpath, "rb+") == NULL)
		LOGTEST("lyy : open error");

	fread(M, sizeof(unsigned char), 11 * 128 * 16, fp_M);
	fclose(fp_M);
	table = (unsigned int *) malloc(10 * 16 * 256 * sizeof(unsigned int));
	sprintf(openpath, "/data/data/%s/files/inv_tables.t", pack_name);
	fp_t = fopen(openpath, "rb");
	fread(table, sizeof(unsigned int), 10 * 16 * 256, fp_t);
	fclose(fp_t);

}
