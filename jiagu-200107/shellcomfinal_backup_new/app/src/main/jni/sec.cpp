#include <string>
#include <errno.h>
#include <iostream>
#include <map>
#include <jni.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <fstream>
#include <string.h>
#include <cstring>
#include <dlfcn.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/system_properties.h>
#include <dirent.h>
#include <pthread.h>
#include <android/log.h>
#include <zlib.h>
#include <zconf.h>
#include <time.h>
#include <sys/mman.h>
#include "./seclib/restoreDex.h"
#include "./seclib/unzip.h"
#include "sec.h"
#include "./godin_io_hook.h"
#include "./godin-hook/hook_module.h"
#include <errno.h>
#include <sys/wait.h>
#include "./include/SubstrateHook.h"
#include "./include/SymbolFinder.h"
#include "./mylinker.h"
#include "./bionic_macros.h"
#if defined(_TEST_DEBUG_ANDROID)
#define LOGTEST(...)   __android_log_print(3,"testsec", __VA_ARGS__)
#else
#define LOGTEST(...)
#endif

using namespace std;


typedef void* ZipArchiveHandle;
class ZipArchiveCache {
 public:
  ZipArchiveCache() {}
  ~ZipArchiveCache();

  bool get_or_open(const char* zip_path, ZipArchiveHandle* handle);
 private:
  DISALLOW_COPY_AND_ASSIGN(ZipArchiveCache);

  map<string, ZipArchiveHandle> cache_;
};



#define PROT_READ  0x1
#define PROT_WRITE 0x2
//#define MAP_FAILED (void*)-1
#define DEX_INTERFACE_CACHE_SIZE    128 
#define FILE_PATH_LEN 200
typedef int (*HashCompareFunc)(const void* tableItem, const void* looseItem);
unsigned char *gDexBuf = NULL;
long gDexLen = 0;
u1 * gODexBuf = NULL;
u1 * goatBuf = NULL;
long gODexLen = 0;
char pkname[200];
unsigned char *oatdex;
unsigned int pid;
int setcount = 0;

unsigned char sokey[256]={0};
int use = 0;

//unsigned char *ttmp;
//uint32_t oatOff = 0;
//int ODEXFD=0;
typedef struct _SecInfo{
  Elf32_Off secOff;
  Elf32_Word secSize;
}SecInfo;
Elf32_Ehdr ehdr;

map<string, int> mSo;
int soget = 0;

JNINativeMethod *dvm_dalvik_system_DexFile;
DexFile* (*dexParse)(const u1*, size_t, int);
void * (*dvmAllocRegion)(size_t size, int prot, const char *name);
AtomicCache* (*dvmAllocAtomicCache)(int numEntries);
void (*dvmInitMutex)(pthread_mutex_t* pMutex);
void (*openDexFileInMemory)(const u4* args, JValue* pResult);
void (*dvmHashTableLock)(HashTable* pHashTable);
void* (*dvmHashTableLookup)(HashTable* pHashTable, u4 itemHash, void* item,
		HashCompareFunc cmpFunc, bool doAdd);
void (*dvmHashTableUnlock)(HashTable* pHashTable);
ClassObject* (*dvmLookupClass)(const char* descriptor, Object* loader,
		bool unprepOkay);
void (*dvmAddInitiatingLoader)(ClassObject* clazz, Object* loader);

static jobject gAppContext = NULL;
char pk_name[200];

static unsigned char s[256]={0};//s-box
set<int> fdSet;
int fdIndex = 0;

int g_is_dex = 0;

int readConfig(int index){
	char path[256] = { 0 };
	sprintf(path, "/data/data/%s/files/config.bin", pk_name);
	FILE *f = fopen(path, "r");
	int i=-1;
	int r=0;
	LOGTEST("libsec.so....");
	while(i<index){
		fscanf(f, "%d", &r);
		LOGTEST("index: 	  %d, i:   %d r:   %d \n", index, i, r);
		i++;
	}
	fclose(f);
	return r;
}

int start_app(){
	char path[256] = { 0 };
	sprintf(path, "/data/data/%s/files/config.bin", pk_name);
	FILE* fd = fopen(path, "r+");
	int a[6];
	fscanf(fd, "%d", a+0);
	fscanf(fd, "%d", a+1);
	fscanf(fd, "%d", a+2);
	fscanf(fd, "%d", a+3);
	fscanf(fd, "%d", a+4);
	fscanf(fd, "%d", a+5);
	
	if(a[0]>=a[1]){
		LOGTEST("error");
		LOGTEST("cur start: %d      all start: %d", a[0], a[1]);
		fclose(fd);
		exit(-1);
	}else{
		a[0]+=1;
	}
	LOGTEST("cur start: %d      all start: %d", a[0], a[1]);
	fseek(fd, 0, SEEK_SET);
	fprintf(fd, "%d\n", a[0]);
	fprintf(fd, "%d\n", a[1]);
	fprintf(fd, "%d\n", a[2]);
	fprintf(fd, "%d\n", a[3]);
	fprintf(fd, "%d\n", a[4]);
	fprintf(fd, "%d", a[5]);
	
	fclose(fd);
}

void rc4_init(unsigned char* pKey, unsigned long ulKeyLen, unsigned char* pSBox)
{
    int i = 0,j = 0;
    char k[256]={0};
    unsigned char tmp = 0;
    for(i = 0; i < 256; i++)
    {
        pSBox[i] = i;
        k[i] = pKey[i % ulKeyLen];
    }
    for(i = 0;i < 256; i++)
    {
        j = (j + pSBox[i] + k[i]) % 256;
        tmp = pSBox[i];
        pSBox[i] = pSBox[j];//exchange pSBox[i] and pSBox[j]
        pSBox[j] = tmp;
    }
}

void rc4_crypt(unsigned char *pSBox, unsigned char *pData,unsigned long ulDataLen,unsigned char *pDataEnc,unsigned long ulcrypt_offset)
{
	int i = 0,j = 0,t = 0;
    unsigned long k = 0;
    unsigned char tmp={0};
    for(k = 0;k < ulcrypt_offset+ulDataLen; k++)
    {
        i = (i + 1) % 256;
        j = (j + pSBox[i]) % 256;
        tmp = pSBox[i];
        pSBox[i] = pSBox[j];//exchange pSBox[i] and pSBox[j]
        pSBox[j] = tmp;
        t= (pSBox[i] + pSBox[j]) % 256;
        if (k>=ulcrypt_offset)
        {//find the correct offset to xor
            pDataEnc[k-ulcrypt_offset]=pData[k-ulcrypt_offset]^pSBox[t];
        }
    }
}

////////////////////////////////////encrypt-so/////////////////////////
typedef struct _funcInfo{
  Elf32_Addr st_value;
  Elf32_Word st_size;
}funcInfo;


void init_getString() __attribute__((constructor));

static void print_debug(const char *msg){
#ifdef DEBUG
	__android_log_print(ANDROID_LOG_INFO, "JNITag", "%s", msg);
#endif
}

static unsigned elfhash(const char *_name)
{
    const unsigned char *name = (const unsigned char *) _name;
    unsigned h = 0, g;

    while(*name) {
        h = (h << 4) + *name++;
        g = h & 0xf0000000;
        h ^= g;
        h ^= g >> 24;
    }
    return h;
}

static unsigned int getLibAddr(){
  unsigned int ret = 0;
  char name[] = "libsec.so";
  char buf[4096], *temp;
  int pid;
  FILE *fp;
  pid = getpid();
  sprintf(buf, "/proc/%d/maps", pid);
  fp = fopen(buf, "r");
  if(fp == NULL)
  {
    puts("open failed");
    goto _error;
  }
  while(fgets(buf, sizeof(buf), fp)){
    if(strstr(buf, name)){
      temp = strtok(buf, "-");
      ret = strtoul(temp, NULL, 16);
      break;
    }
  }
_error:
  fclose(fp);
  return ret;
}

int getTargetFuncInfo(unsigned long base, const char *funcName, funcInfo *info){
	char flag = -1, *dynstr;
	int i;
	Elf32_Ehdr *ehdr;
	Elf32_Phdr *phdr;
	Elf32_Off dyn_vaddr;
	Elf32_Word dyn_size, dyn_strsz;
	Elf32_Dyn *dyn;
    Elf32_Addr dyn_symtab, dyn_strtab, dyn_hash;
	Elf32_Sym *funSym;
	unsigned funHash, nbucket;
	unsigned *bucket, *chain;

    ehdr = (Elf32_Ehdr *)base;
	phdr = (Elf32_Phdr *)(base + ehdr->e_phoff);
//    __android_log_print(ANDROID_LOG_INFO, "JNITag", "phdr =  0x%p, size = 0x%x\n", phdr, ehdr->e_phnum);
	for (i = 0; i < ehdr->e_phnum; ++i) {
//		__android_log_print(ANDROID_LOG_INFO, "JNITag", "phdr =  0x%p\n", phdr);
		if(phdr->p_type ==  PT_DYNAMIC){
			flag = 0;
			print_debug("Find .dynamic segment");
			break;
		}
		phdr ++;
	}
	if(flag)
		goto _error;
	dyn_vaddr = phdr->p_vaddr + base;
	dyn_size = phdr->p_filesz;
//	__android_log_print(ANDROID_LOG_INFO, "JNITag", "dyn_vadd =  0x%x, dyn_size =  0x%x", dyn_vaddr, dyn_size);
	flag = 0;
	for (i = 0; i < dyn_size / sizeof(Elf32_Dyn); ++i) {
		dyn = (Elf32_Dyn *)(dyn_vaddr + i * sizeof(Elf32_Dyn));
		if(dyn->d_tag == DT_SYMTAB){
			dyn_symtab = (dyn->d_un).d_ptr;
			flag += 1;
//			__android_log_print(ANDROID_LOG_INFO, "JNITag", "Find .dynsym section, addr = 0x%x\n", dyn_symtab);
		}
		if(dyn->d_tag == DT_HASH){
			dyn_hash = (dyn->d_un).d_ptr;
			flag += 2;
//			__android_log_print(ANDROID_LOG_INFO, "JNITag", "Find .hash section, addr = 0x%x\n", dyn_hash);
		}
		if(dyn->d_tag == DT_STRTAB){
			dyn_strtab = (dyn->d_un).d_ptr;
			flag += 4;
//			__android_log_print(ANDROID_LOG_INFO, "JNITag", "Find .dynstr section, addr = 0x%x\n", dyn_strtab);
		}
		if(dyn->d_tag == DT_STRSZ){
			dyn_strsz = (dyn->d_un).d_val;
			flag += 8;
//			__android_log_print(ANDROID_LOG_INFO, "JNITag", "Find strsz size = 0x%x\n", dyn_strsz);
		}
	}
	if((flag & 0x0f) != 0x0f){
		print_debug("Find needed .section failed\n");
		goto _error;
	}
	dyn_symtab += base;
	dyn_hash += base;
	dyn_strtab += base;
	dyn_strsz += base;

	funHash = elfhash(funcName);
	funSym = (Elf32_Sym *) dyn_symtab;
	dynstr = (char*) dyn_strtab;
	nbucket = *((int *) dyn_hash);
	bucket = (unsigned int *)(dyn_hash + 8);
	chain = (unsigned int *)(dyn_hash + 4 * (2 + nbucket));

	flag = -1;
//	__android_log_print(ANDROID_LOG_INFO, "JNITag", "hash = 0x%x, nbucket = 0x%x\n", funHash, nbucket);
	for(i = bucket[funHash % nbucket]; i != 0; i = chain[i]){
		__android_log_print(ANDROID_LOG_INFO, "JNITag", "Find index = %d\n", i);
		if(strcmp(dynstr + (funSym + i)->st_name, funcName) == 0){
			flag = 0;
//			__android_log_print(ANDROID_LOG_INFO, "JNITag", "Find %s\n", funcName);
			break;
		}
	}
	if(flag) goto _error;
	info->st_value = (funSym + i)->st_value;
	info->st_size = (funSym + i)->st_size;
//	__android_log_print(ANDROID_LOG_INFO, "JNITag", "st_value = %d, st_size = %d", info->st_value, info->st_size);
	return 0;
_error:
	return -1;
}


void init_getString(){
	static unsigned char key[16] = {0x00, 0x71, 0xc3, 0xb2, 0x69, 0x73, 0x61, 0x6b, 0x65, 0x79, 0x66, 0x6f, 0x72, 0x72, 0x63, 0x34};
	inv_key_map(key);

	rc4_init(key,16,s);
	unsigned char sTemp[256]={0};
	memcpy(sTemp,s,256);
	memcpy(sokey,sTemp,256);

	char target_fun[] = "JNI_OnLoad";
//	char target_fun[7][30] = {"JNI_OnLoad", "protect_ndebug", "protect_jdebug",
//	                          "decrypt", "start_hook_mmap", "start_hook_write",
//	                          "SetUpDexClassLoader", "SetUpARTDexClassLoader",
//	                          "dvmContinueOptimization"};
	funcInfo info;
	int i;
	unsigned int npage, base = getLibAddr();

	for(int i=0;i<16;i++){ LOGTEST("key_stemp: %x", sTemp[i]); }

//	for(i=0; i<9; i++)
//	{
		if(getTargetFuncInfo(base, target_fun, &info) == -1){
		  print_debug("Find function failed");
		  return ;
		}

		npage = info.st_size / PAGE_SIZE + ((info.st_size % PAGE_SIZE == 0) ? 0 : 1);
		if(mprotect((void *) ((base + info.st_value - (info.st_value % PAGE_SIZE))), (npage+1)*PAGE_SIZE, PROT_READ | PROT_EXEC | PROT_WRITE) != 0){
			print_debug("mem privilege change failed");
		}

		char *addr = (char*)(base + info.st_value -1);

		rc4_crypt(sTemp, (unsigned char *)addr, info.st_size, (unsigned char *)addr, 0);

		if(mprotect((void *) ((base + info.st_value - (info.st_value % PAGE_SIZE))), (npage+1)*PAGE_SIZE, PROT_READ | PROT_EXEC) != 0){
			print_debug("mem privilege change failed");
		}
//	}

}

jobject getAppContext(JNIEnv *env) {

	if (gAppContext == NULL) {
		jclass classActivityThread = env->FindClass(
				"android/app/ActivityThread");
		jmethodID getActivityThreadID = env->GetStaticMethodID(
				classActivityThread, "currentActivityThread",
				"()Landroid/app/ActivityThread;");
		jobject objectCurrentActivityThread = env->CallStaticObjectMethod(
				classActivityThread, getActivityThreadID);
		if (objectCurrentActivityThread != NULL) {

			//get context object
			jmethodID getApplicationID = env->GetMethodID(classActivityThread,
					"getApplication", "()Landroid/app/Application;");
			jobject objectCurrentApplication = env->CallObjectMethod(
					objectCurrentActivityThread, getApplicationID);
			jclass classApp = env->FindClass("android/app/Application");
			jmethodID getAppContext = env->GetMethodID(classApp,
					"getApplicationContext", "()Landroid/content/Context;");
			jobject ctx = env->CallObjectMethod(objectCurrentApplication,
					getAppContext);
			gAppContext = env->NewGlobalRef(ctx);
			env->DeleteLocalRef(classActivityThread);
			env->DeleteLocalRef(classApp);

		}
	}
	return gAppContext;
}
/*
 * use System.getProperty("java.vm.version")
 * if the result is over 2.0.0 it is art mode
 */
bool isArtMode(JNIEnv* env) {
	char tempname[32] = "java.vm.version";
	jclass classContext = env->FindClass("java/lang/System");
	//LOGTEST("zhr-isArtMode: find class over");
	jmethodID getPropertyID = env->GetStaticMethodID(classContext,
			"getProperty", "(Ljava/lang/String;)Ljava/lang/String;");
	//LOGTEST("zhr-isArtMode: find method over");
	jstring setname = env->NewStringUTF(tempname);
	//LOGTEST("zhr-isArtMode: setname:%s",tempname);
	jstring objectVersion = (jstring) env->CallStaticObjectMethod(classContext,
			getPropertyID, setname);
	//LOGTEST("zhr-isArtMode: execute method over");
	const char* ver_now = env->GetStringUTFChars(objectVersion, NULL);
	//LOGTEST("zhr-isArtMode:  ver:%s", ver_now);

	if (ver_now != NULL && ver_now[0] == '2')
		return true;
	else
		return false;
}

void getApplicationPackagename(JNIEnv * env, char *pkName) {

	jobject objContext = getAppContext(env);
	jclass classContext = env->FindClass("android/content/Context");
	jmethodID getPMID = env->GetMethodID(classContext, "getPackageManager",
			"()Landroid/content/pm/PackageManager;");

	jobject objectPM = env->CallObjectMethod(objContext, getPMID);

	jmethodID getPackageNameID = env->GetMethodID(classContext,
			"getPackageName", "()Ljava/lang/String;");
	jstring objectPackageName = (jstring) env->CallObjectMethod(objContext,
			getPackageNameID);
	const char* pPkName = env->GetStringUTFChars(objectPackageName, NULL);
	strcpy(pkName, pPkName);
	env->ReleaseStringUTFChars(objectPackageName, (const char *) pPkName);
	env->DeleteLocalRef(classContext);
}

void getApplicationPackageLocation(JNIEnv *env, char *pszPathBuffer) {

	jobject objContext = getAppContext(env);
	jclass classContext = env->FindClass("android/content/Context");
	jmethodID getPMID = env->GetMethodID(classContext, "getPackageManager",
			"()Landroid/content/pm/PackageManager;");

	jobject objectPM = env->CallObjectMethod(objContext, getPMID);

	jmethodID getPackageNameID = env->GetMethodID(classContext,
			"getPackageName", "()Ljava/lang/String;");
	jobject objectPackageName = (jstring) env->CallObjectMethod(objContext,
			getPackageNameID);
	jclass classPM = env->FindClass("android/content/pm/PackageManager");

	jmethodID getAppInfoID = env->GetMethodID(classPM, "getApplicationInfo",
			"(Ljava/lang/String;I)Landroid/content/pm/ApplicationInfo;");

	jobject objectAppInfo = env->CallObjectMethod(objectPM, getAppInfoID,
			objectPackageName, 0x00002000);
	jclass classAppInfo = env->FindClass("android/content/pm/ApplicationInfo");

	jfieldID sourceDirFID = env->GetFieldID(classAppInfo, "sourceDir",
			"Ljava/lang/String;");
	//jfieldID fieldID = env->GetFieldID(classClassLoader, "parent", "Ljava/lang/ClassLoader;");

	jstring srcDirStr = (jstring) env->GetObjectField(objectAppInfo,
			sourceDirFID);
	const char *szInstallPath = env->GetStringUTFChars(srcDirStr, NULL);

	strcpy(pszPathBuffer, (char *) szInstallPath);
	env->ReleaseStringUTFChars(srcDirStr, (const char *) szInstallPath);
	env->DeleteLocalRef(srcDirStr);
	env->DeleteLocalRef(classContext);
	env->DeleteLocalRef(classAppInfo);
	env->DeleteLocalRef(classPM);

	return;
}

bool getFileByteFromApk(JNIEnv *env, const char* targetFile,
		unsigned char **data, int *length) {

	char pszApkPath[FILE_PATH_LEN];
	int err = UNZ_OK;
	LOGI("retrive1 file content from assets....targefile = %s", targetFile);
	getApplicationPackageLocation(env, pszApkPath);
	unzFile uf = unzOpen64(pszApkPath);

	if (!uf) {
		return false;
	}

	err = unzLocateFile(uf, targetFile, 0);
	if (UNZ_OK != err) {
		return false;
	}

	char filename_inzip[256] = { 0 };
	unz_file_info64 file_info;

	err = unzGetCurrentFileInfo64(uf, &file_info, filename_inzip,
			sizeof(filename_inzip), NULL, 0, NULL, 0);

	if (UNZ_OK != err) {
		return false;
	}

	err = unzOpenCurrentFilePassword(uf, NULL);
	if (UNZ_OK != err) {
		return false;
	}

	void *buf = (void *) malloc(file_info.uncompressed_size);
	if (!buf) {
		return false;
	}

	if (unzReadCurrentFile(uf, buf, file_info.uncompressed_size) <= 0) {
		return false;
	}

	unzCloseCurrentFile(uf);
	*length = (int) file_info.uncompressed_size;
	*data = (unsigned char *) buf;
	return true;
}
void checksign(JNIEnv * env, unsigned char ** pbinfile)
{
	    LOGTEST("check dex sign\n");
		FILE *file_bin;
		int dex_file_length;
		char filename[200];
		BIN_FILE_TYPE binfiletype;
		unsigned char key[16];
		unsigned char *dex_from_apk;
		int dex_from_apk_len;
		amac_ctx amac;
		char dexpath[256];
		//char datapath[256];
		char infopath[256];
		unsigned char dex_amac_result[16];
		unsigned char dexsigned[16];

		strncpy(filename, "/data/data/", 12);
		strncat(filename, pk_name, 200 - 13);
		strncat(filename, "/files/class.bin", 17);
		//LOGTEST("zhangyate %s",filename);

		analysis_bin_file(filename, &binfiletype);
		//LOGTEST("zhangyate %d   %d",binfiletype.key_start,binfiletype.key_size);

		file_bin = fopen(filename, "r");

		fseek(file_bin, binfiletype.dexsign_start, SEEK_SET);
		fread(dexsigned, 1, binfiletype.dexsign_size, file_bin);
		LOGTEST("kangtai %s",dexsigned);

		fseek(file_bin, binfiletype.key_start, SEEK_SET);
		fread(key, 1, binfiletype.key_size, file_bin);

		getFileByteFromApk(env, "classes.dex", &dex_from_apk, &dex_from_apk_len);

		amac_init(&amac, key, key);
		amac_calc(&amac, dex_from_apk, dex_from_apk_len);
		amac_result(&amac, dex_amac_result);
		LOGTEST("kangtai %s",dex_amac_result);


		if (memcmp(dex_amac_result, dexsigned, 16) != 0) {
			LOGTEST("dex signed err");
			exit(1);
		}
		free(dex_from_apk);
}


/*
 * hook the mmap, here have two our own logic
 * if mmap the dex, get the real dex, and mmap this(data/data/pkname/classes.dex)
 * if mmap the oat, decrypt the dex part(data/data/pkname/files/classes.dex)
 * --by-zhr
 */
static void *mmap_hook_test(void *addr, size_t length, int prot, int flags,
		int fd, off_t offset) {
	double mmaptime;
	void* pFinal = NULL;
	char* hook = getenv("HOOKFLAG");

	if (hook == NULL) {
		//LOGTEST("hook == NULL");
		pFinal = mmap(addr, length, prot, flags, fd, offset);
		return pFinal;
	}

	LOGTEST("begin in ART-------mmap-hook-test");
	//__android_log_print(ANDROID_LOG_INFO, "JNITagmmap", "Find index = %d\n", 111);
	char tempBuff[256];
	char fdName[256];
	char* hookpkname = getenv("ENVPACN");
	memset(tempBuff, 0, 256);
	memset(fdName, 0, 256);
	sprintf(tempBuff, "/proc/%d/fd/%d", gettid(), fd);
	int readRes = 0;
	readRes = readlink(tempBuff, fdName, 256);
    LOGTEST("fdname %s", fdName);
	char pDexName[256];
	char d2oname[256] = {0};
	memset(pDexName, 0, 256);
	sprintf(pDexName, "/data/data/%s/files/%s", hookpkname, "haha1.dex");
    	sprintf(d2oname, "/data/data/%s/files/%s", hookpkname, "dex2oat.dex");
	//sprintf(d2oname, "/data/data/%s/files/%s", hookpkname, "haha1.dex");
	unsigned int temppid = getpid();
	//sprintf(pDexName, "/data/dalvik-cache/data@app@%s", hookpkname);

	if ((!strcmp(pDexName, fdName))&& temppid != pid) {
		LOGTEST("mmap hook : dexfile is %s", pDexName);
		//pFinal = mmap(addr, length, prot, flags | MAP_ANONYMOUS, fd, offset);
		pFinal = mmap(addr, length, prot, flags, fd, offset);
        FILE *d2afile = fopen(d2oname, "rb");
        unsigned char *d2a = (unsigned char *)malloc(length * sizeof(unsigned char));
        fread(d2a, 1, length, d2afile);
        fclose(d2afile);
		mprotect(pFinal, length, PROT_READ | PROT_WRITE | PROT_EXEC);
        memcpy(pFinal, d2a, length);

		int m = mprotect(pFinal, length, PROT_READ | PROT_WRITE | PROT_EXEC);

		free(d2a);
		return pFinal;

	}



	if((!strcmp(pDexName, fdName))&& temppid == pid ){
		LOGTEST("main process mmap dex");
		pFinal = mmap(addr, length, prot, flags, fd, offset);
        //char *a = (char *)pFinal + 8;
		stroeoatbuf((u1* )pFinal, length);
		unsigned char *tmp = (unsigned char *)pFinal;
		for(int i=0; i<8; ++i){
			LOGTEST("pFnial %x", tmp[i]);			
		}
/*		out = fopen("/tmp/ttt.txt", "wb");
		for(int i=0; i<length; i++){
			FILE *out;
						
						
		}
*/
		return pFinal;
	}
	char pOatName[256];
	memset(pOatName, 0, 256);
	char check[256];
	memset(check, 0, 256);
	sprintf(check, "/data/data/%s/files/%s", hookpkname, "check.sum");
	sprintf(pOatName, "/data/data/%s/files/%s", hookpkname, "classes.dex");
	if (!strcmp(pOatName, fdName) && (length != 0x1000)) {
		LOGTEST("mmap hook : oatfile is %s", pOatName);
		//add the MAP_ANONYMOUS flag in case others get the memory by fd name
		pFinal = mmap(addr, length, prot, flags, fd, offset);
		unsigned char *j = (unsigned char *)pFinal;

		if (!memcmp((char *)pFinal + 1, "ELF", 3)) {
			//uint32_t oatOff = 0;
			unsigned int oatOff = 0;
			if (hook[0] == '6')
				oatOff = 0x1044;//6.0
			else if (hook[0] == 't')
				oatOff = 0x1050;//5.X
			else
				oatOff = 0x103c;

			uint32_t KeyValueStoreSize = 0;
			uint32_t DexNameLen = 0;
			uint32_t DexOff = 0;
			memcpy(&KeyValueStoreSize, (char*) pFinal + oatOff, 4);
			oatOff += 4;
			oatOff += KeyValueStoreSize;
			mprotect(pFinal, length, PROT_READ | PROT_WRITE | PROT_EXEC);
			memcpy(&DexNameLen, (char*) pFinal + oatOff, 4);
			oatOff += 4;			//add the name length part
			oatOff += DexNameLen;	//add the name part

			unsigned char cmpHead2[4];
			FILE *checksum = fopen(check, "rb");
			fread(cmpHead2, 1, 4, checksum);
			fclose(checksum);
            memcpy((char *)pFinal + oatOff, cmpHead2, 4);
			oatOff += 4;			//add the checksum part
			memcpy(&DexOff, (char*) pFinal + oatOff, 4);
			oatOff = 0x1000 + DexOff;

			uint32_t DexSize = 0;
			FILE *file_Dexsize;
			char DexSizeFileName[256];
			sprintf(DexSizeFileName, "/data/data/%s/files/%s", hookpkname,
					"SecSize.config");
			file_Dexsize = fopen(DexSizeFileName, "r");
			fread(&DexSize, 1, sizeof(size_t), file_Dexsize);
			fclose(file_Dexsize);

			char* DexOut = (char*) malloc(DexSize + 1);
			unsigned char key[16];
			unsigned char iv[16] = { 0x0 };
			FILE *file_bin;
			BIN_FILE_TYPE binfiletype;
		    char binFileName[256];
			sprintf(binFileName, "/data/data/%s/files/%s", hookpkname,
					"class.bin");
			analysis_bin_file(binFileName, &binfiletype);
			file_bin = fopen(binFileName, "r");
			fseek(file_bin, binfiletype.key_start, SEEK_SET);
			fread(key, 1, binfiletype.key_size, file_bin);
			fclose(file_bin);
			mprotect(pFinal, length, PROT_READ | PROT_WRITE | PROT_EXEC);

			//LOGTEST("mmap hook : start decrypt oat\n");
		    AES_Encrypt_128_OFB(key, iv, (unsigned char*) pFinal + oatOff,
					(unsigned char*) DexOut, DexSize);
			//LOGTEST("mmap hook : decrypt oat successed\n");

			memcpy((char*) pFinal + oatOff, DexOut, DexSize);

			free(DexOut);

			char *b = (char *)pFinal;

			b = (char *)pFinal + oatOff;
		}

	} else {
		pFinal = mmap(addr, length, prot, flags, fd, offset);
	}
	return pFinal;
}

static void *mmap_hook(void *addr, size_t length, int prot, int flags, int fd,
		off_t offset) {
	//implement the mmap first anyway
	void* pFinal = NULL;

	//LOGTEST("do  actual mmap first anyway!\n");
	char* hookpkname = getenv("ENVPACN");
	char* hook = getenv("HOOKFLAG");
    pFinal = (*backMmap)(addr, length, prot, flags, fd, offset);
	if (hook == NULL) {
		//LOGTEST("not to hook\n");
		//LOGTEST("pFinal2 is %x",(int *)pFinal);
        if(fd == -1){
        	//LOGTEST("fd = NULL ");
        }
		return pFinal;
	}

	//LOGTEST("begin in dalvik-------mmap-hook");

	char tempBuff[256];
	char fdName[256];
	memset(tempBuff, 0, 256);
	memset(fdName, 0, 256);
	sprintf(tempBuff, "/proc/%d/fd/%d", gettid(), fd);
	//LOGTEST("The tempBuff is: %s\n", tempBuff);
	int readRes = 0;
	readRes = readlink(tempBuff, fdName, 256);

	//char pDexName[256];
	// memset(pDexName, 0, 256);
	char pODexName[256];
	memset(pODexName, 0, 256);
	//sprintf(pDexName, "/data/data/%s/%s", hookpkname, "classes.dex");
	sprintf(pODexName, "/data/data/%s/files/%s", hookpkname, "classes.dex");
	//sprintf(pODexName, "/data/dalvik-cache/data@app@%s", hookpkname);
	//LOGTEST("dex name is %s\n", pDexName);
	//LOGTEST("pODexName name is %s\n", pODexName);
	//LOGTEST("fdName name is %s\n", fdName);
	int realfd;
	int fileSize = length;
	char* realDexBuff = NULL;

	if (!strcmp(pODexName, fdName)) {
		LOGTEST("found odex file \n");
        if(fd == -1){
        	LOGTEST("fd = NULL ");
        }

		mprotect(pFinal, length, PROT_READ | PROT_WRITE | PROT_EXEC);

		free(gODexBuf);
		gODexBuf = (u1 *) pFinal;
		stroeodexbuf(gODexBuf, length);

	} else {
        if(fd == -1){
        	LOGTEST("fd = NULL ");
        }

	}

	return pFinal;
}

int sha1_hash(const char *input, char *output){
	SHA1Context sha;
	char buf[45];

	SHA1Reset(&sha);
	SHA1Input(&sha, input, strlen(input));

	if (!SHA1Result(&sha)){
		LOGTEST("SHA1 ERROR: Could not compute message digest");
		return -1;
	}
	else {
		memset(buf,0,sizeof(buf));
		sprintf(buf, "%08X%08X%08X%08X%08X", sha.Message_Digest[0],sha.Message_Digest[1],
		sha.Message_Digest[2],sha.Message_Digest[3],sha.Message_Digest[4]);
		memcpy(output, buf, strlen(buf));
		return 0;
	}
}

static int my_open(const char *pathname, int oflag)
{

	int fd = open(pathname,oflag);
//	LOGTEST("open:fd=%d:pathname=%s,flag= %d",fd,pathname,oflag);

	return fd;
}

static int my_close(int fd)
{
//	LOGTEST("close:fd=%d",fd);
	return close(fd);
}

static ssize_t my_pread64(int fd,char *buff,size_t nbyte,off64_t offset)
{
	unsigned char *buf_from_read;
	buf_from_read = (unsigned char *)malloc(nbyte);
	memset(buf_from_read, 0, nbyte);
    ssize_t res = pread(fd,buf_from_read,nbyte,offset);
//    LOGTEST("pread64_f:fd=%d,buff=%x,nbyte=%d,offset=%x,res=%d",fd,buff,nbyte,offset,res);
    int i=0;

    unsigned char sTemp[256]={0};
    memcpy(sTemp,s,256);
    rc4_crypt(sTemp,buf_from_read,nbyte,(unsigned char *)buff,offset);
	free(buf_from_read);
    return res ;

	//LOGTEST("hello---pread64--fd=%d,buff=%x,nbyte=%d,offset=%x",fd,buff,nbyte,offset);
	//return pread64(fd,buff,nbyte,offset);

}

static ssize_t my_pwrite64(int fd,char *buff,size_t nbyte, off64_t offset)
{
	unsigned char *buf;
	buf = (unsigned char *)malloc(nbyte);
	memset(buf, 0, nbyte);
//    LOGTEST("pwrite64_f:fd=%d,buff=%x,nbyte=%d,offset=%x",fd,buff,nbyte,offset);
    int i=0;
    unsigned char sTemp[256]={0};
    memcpy(sTemp,s,256);
    rc4_crypt(sTemp,(unsigned char *)buff,nbyte,buf,offset);
    ssize_t res=pwrite(fd,buf,nbyte,offset);
	free(buf);
    return res;
//	LOGTEST("hello--pwrite--fd=%d,buff=%x,nbyte=%d,offset=%x",fd,buff,nbyte,offset);
//	return pwrite64(fd,buff,nbyte,offset);
}
int (*old_open)(const char *pathname, int oflag) = NULL;
int fi_open(const char *pathname, int oflag)
{
	LOGTEST("open pathname: %s", pathname);
	char* tmp = NULL;
	char* tmp2 = NULL;
    	int fd = open(pathname, oflag);
    	LOGTEST("[+] hook open successful! pathname = %s, fd= %d", pathname, fd);
    	char path[256] = { '\0' };
	char path2[256] = {'\0'};
    	sprintf(path, "/data/data/%s/files", pk_name);
	sprintf(path2,"/data/user/0/%s/files", pk_name);
    	LOGTEST("[+] hook open path= %s", path);
    	char *pFlag;
	LOGTEST("fi_open1");
	if(strstr(pathname, "myFile")){
		LOGTEST("myFile: %s", pathname);
	}
    	//strcpy(pFlag, strstr(pathname, path));
	tmp = strstr(pathname, path);
	tmp2 = strstr(pathname, path2);
	LOGTEST("fi_open tmp:%s tmp2:%s",tmp, tmp2);
	//strcpy(pFlag, tmp);
	//LOGTEST("fi_open pFlag:%s", pFlag);
    	if(tmp || tmp2)
    	{
    		fdSet.insert(++fdIndex);
//    		LOGTEST("[+] fdSet.size() = %d", fdSet.size());
    		LOGTEST("[+] hook open successful! pathname = %s, fd= %d", pathname, fd);
    	}
    	LOGTEST("[+] hook open successful! pathname=%s, fd= %d", pathname, fd);
    	return fd;
}
static int (*old_close)(int fd) = NULL;
static int fi_close(int fd)
{
	fdSet.erase(fdIndex--);
//	LOGTEST("[+] hook close successful! fd= %d, fdFlag = %d", fd, fdFlag[fd]);
	return close(fd);
}

unsigned int (*old_write)(int fd, char *buff, int len) =NULL;
unsigned int fi_write(int fd, char *buff, int len)
{
	if(0 != fdSet.size())
	{
		unsigned char *buf_for_fi_write;
		buf_for_fi_write = (unsigned char *)malloc(len);
		LOGTEST("[+] hook write successful! fd = %d, len = %d", fd, len);
		//get absolute path of the file
		char tempBuff[256];
		char fdName[256];
		memset(tempBuff, 0, 256);
		memset(fdName, 0, 256);
		sprintf(tempBuff,"/proc/self/fd/%d",fd);
		readlink(tempBuff,fdName,256);
		//open the file and locate the end
		int offset;
//		FILE *fp;
//		fp=fopen(fdName,"rt+");
//		fseek(fp,0,SEEK_END);
//		offset=ftell(fp);

		struct stat st;
		stat(fdName, &st);
		offset = st.st_size;

		//write file from the end
		unsigned char sTemp[256] = {0};
		memcpy(sTemp, s, 256);
		//encrypt
		memset(buf_for_fi_write, 0, len);
		rc4_crypt(sTemp, (unsigned char *)buff, len, (unsigned char *)buf_for_fi_write, offset);
		LOGTEST("[+] encrypt write successful! len = %d", len);
		//xor
//		xor_crypt(sTemp, (unsigned char *)buff, len, (unsigned char *)buf_for_fi_write, offset);
//		LOGTEST("[+] xor encrypt write successful! len = %d", len);
		int res=write(fd, buf_for_fi_write, len);
//		int res=write(fd, buff, len);
		free(buf_for_fi_write);
		return res;
	}
	else{
		LOGTEST("[+] test hook write successful! fd = %d, len = %d", fd, len);
		int res=write(fd, buff, len);
		return res;
	}
}
unsigned int (*old_read)(int fd, char *buff, int len) =NULL;
unsigned int  fi_read(int fd, char *buff, int len)
{
	if(0 != fdSet.size())
	{
		unsigned char *buf_from_fi_read;
		buf_from_fi_read = (unsigned char *)malloc(len);
		//read file
		int offset;
		memset(buf_from_fi_read, 0, len);
		int res= read(fd, buf_from_fi_read, len);
//		int res= read(fd, buff, len);
		LOGTEST("[+] hook read successful!read:fd=%d, len = %d", fd, len);
		//get the read position
		offset=lseek(fd,0l,SEEK_CUR);
		offset=offset-res;

		unsigned char sTemp[256]={0};
		memcpy(sTemp,s,256);
		//decrypt
		rc4_crypt(sTemp, (unsigned char *)buf_from_fi_read, len, (unsigned char *)buff, offset);
		LOGTEST("[+] decrypt read successful! len = %d", len);
		//xor
//		xor_crypt(sTemp, (unsigned char *)buf_from_fi_read, len, (unsigned char *)buff, offset);
//		LOGTEST("[+] xor decrypt read successful! len = %d", len);
		free(buf_from_fi_read);
		return res;
	}
	else{
		LOGTEST("[+] test hook read successful!read:fd=%d, len = %d", fd, len);
		int res= read(fd, buff, len);
		return res;
	}
}

int execv_hook_test(const char *path, char *const argv[]){
	//LOGTEST("execve hook");
	//LOGTEST("execve filename : %s", argv[0]);
	__android_log_print(ANDROID_LOG_INFO, "JNITagdex2oat_execv", "Find index = %d\n", 2);
	int pid;
	if(strcmp("/system/bin/dex2oat",path) == 0){
		return -1;
	}else{
		//if(pid == 0){
			execv(path, argv);
			//exit(EXIT_FAILURE);
		//}
		//int status;
		//wait(&status);	
	}
}

int execl_hook_test(const char *path, const char *arg, ...){
	//LOGTEST("execve hook");
	//LOGTEST("execve filename : %s", argv[0]);
	__android_log_print(ANDROID_LOG_INFO, "JNITagdex2oat_execl", "Find index = %d\n", 2);
	int pid;
	if(strcmp("/system/bin/dex2oat",path) == 0){
		return -1;
	}else{
		//if(pid == 0){
			execl(path, arg);
			//exit(EXIT_FAILURE);
		//}
		//int status;
		//wait(&status);	
	}
}

int execve_hook_test(const char * filename,char * const argv[],char * const envp[]){

	//LOGTEST("execve hook");
	//LOGTEST("execve filename : %s", argv[0]);
	__android_log_print(ANDROID_LOG_INFO, "JNITagdex2oat_execve", "Find index = %d\n", 2);
	int pid;
	if(strcmp("/system/bin/dex2oat",filename) == 0){
		return -1;
	}else{
		//if(pid == 0){
			execve(filename, argv, envp);
			//exit(EXIT_FAILURE);
		//}
		//int status;
		//wait(&status);	
	}
}


static void SetUpDexClassLoader(JNIEnv *env, char* dexPath) {
	LOGTEST("start setclassloader");
	char szFilesDirBuf[256] = { '\0' };
	char szLibPath[256] = { '\0' };
	char *pTmp = NULL;
	jobject ctx = getAppContext(env);

	//call ctx.getClassLoader to get the path class loader
	jclass classContext = env->FindClass("android/content/Context");
	jmethodID getClassLoaderID = env->GetMethodID(classContext,
			"getClassLoader", "()Ljava/lang/ClassLoader;");
	jobject pathClassLoader = env->CallObjectMethod(ctx, getClassLoaderID);
	//get files dir like "/data/data/packagename/files"
	sprintf(szFilesDirBuf, "/data/data/%s/files", pk_name);
	//LOGTEST("the szFilesDirBuf is: %s\n", szFilesDirBuf);
	int setEvnPacName = setenv("ENVPACN", pk_name, 1);
	//create one custom classloader under the boot classloader
//	jclass classDexLoader = env->FindClass("dalvik/system/DexClassLoader");
	//LOGTEST("begin set new classloader");
	jclass classDexLoader = env->FindClass("com/example/forshell/CustomerClassLoader");
	jmethodID initMID =
			env->GetMethodID(classDexLoader, "<init>",
					"(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/ClassLoader;)V");
	jstring inputPath = env->NewStringUTF(dexPath);
	jstring outputPath = env->NewStringUTF(szFilesDirBuf);
	sprintf(szLibPath, "/data/data/%s/%s", pk_name, "files");//set apk sopath
	jstring libpath = env->NewStringUTF(szLibPath);
	//LOGTEST("lib path = %s", szLibPath);
	jclass classClassLoader = env->FindClass("java/lang/ClassLoader");
	jmethodID getparentID = env->GetMethodID(classClassLoader, "getParent",
			"()Ljava/lang/ClassLoader;");
	jobject objectBootLoader = env->CallObjectMethod(pathClassLoader,
			getparentID);
	// set env "HOOKFLAG" to start really hook.
	int setEvnRes = setenv("HOOKFLAG", "true", 1);
	jobject objectCustomLoader = env->NewObject(classDexLoader, initMID,
			inputPath, outputPath, libpath, objectBootLoader);

	jmethodID setId = env->GetMethodID(classDexLoader, "setIs_write_back", "(I)V");
	env->CallVoidMethod(objectCustomLoader, setId, g_is_dex);
	LOGTEST("native dalvik set is_write_back is ok");

	// unset env "HOOKFLAG" to finish really hook.
	int unSetEvnRes = unsetenv("HOOKFLAG");
	//getdexoff(pk_name);
	env->DeleteLocalRef(inputPath);
	env->DeleteLocalRef(outputPath);
	env->DeleteLocalRef(libpath);

	//set the path classloader's parent as the custom classloader
	jfieldID fieldID = env->GetFieldID(classClassLoader, "parent",
			"Ljava/lang/ClassLoader;");

	if (fieldID == NULL) {
		LOGTEST("erro to get the classloader class's parent field");
	}
	env->SetObjectField(pathClassLoader, fieldID, objectCustomLoader);
	env->DeleteLocalRef(classDexLoader);
	env->DeleteLocalRef(classContext);
	env->DeleteLocalRef(classClassLoader);
	//return objectCustomLoader;
}
static void SetUpARTDexClassLoader(JNIEnv *env, char* dexPath) {
	LOGTEST("SetUp ART DexClassLoader");
	char szFilesDirBuf[256] = { '\0' };
	char szLibPath[256] = { '\0' };
	char szLibSoPath[256] = { '\0' };
	char szLibSoPath1[256] = { '\0' };
	char *pTmp = NULL;
	char m_versdk[16] = { '\0' };
	__system_property_get("ro.build.version.sdk", m_versdk);
	//LOGTEST("versdk-get:%s",m_versdk);
  	int versdk = atoi(m_versdk);
	if(versdk >= 24){
	start_hook_execve("/system/lib/libart.so", (void *) execve_hook_test, "execve");
	start_hook_execv("/system/lib/libart.so", (void *) execv_hook_test, "execv");
	//start_hook_execl("/system/lib/libc.so", (void *) execl_hook_test, "execl");
	}


	char m_verrel[16] = { '\0' };
	__system_property_get("ro.build.version.release", m_verrel);
	//LOGTEST("verrel-get:%s",m_verrel);

	jobject ctx = getAppContext(env);

	//call ctx.getClassLoader to get the path class loader
	jclass classContext = env->FindClass("android/content/Context");
	jmethodID getClassLoaderID = env->GetMethodID(classContext,
			"getClassLoader", "()Ljava/lang/ClassLoader;");
	jobject pathClassLoader = env->CallObjectMethod(ctx, getClassLoaderID);

	//get files dir like "/data/data/packagename/files"
	sprintf(szFilesDirBuf, "/data/data/%s/files", pk_name);
	//LOGTEST("the szFilesDirBuf is: %s\n", szFilesDirBuf);
	//create one custom classloader under the boot classloader
	jclass classDexLoader = env->FindClass("com/example/forshell/CustomerClassLoader");
	jmethodID initMID =
			env->GetMethodID(classDexLoader, "<init>",
					"(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/ClassLoader;)V");
	jstring inputPath = env->NewStringUTF(dexPath);
	jstring outputPath = env->NewStringUTF(szFilesDirBuf);
	sprintf(szLibPath, "/data/data/%s/%s", pk_name, "files");// set apk so path
	jstring libpath = env->NewStringUTF(szLibPath);
	//LOGTEST("lib path = %s", szLibPath);
	jclass classClassLoader = env->FindClass("java/lang/ClassLoader");
	jmethodID getparentID = env->GetMethodID(classClassLoader, "getParent",
			"()Ljava/lang/ClassLoader;");
	jobject objectBootLoader = env->CallObjectMethod(pathClassLoader,
			getparentID);

	sprintf(szLibSoPath, "/data/data/%s/%s/libsec.so", pk_name, "lib");
	sprintf(szLibSoPath1, "/data/data/%s/%s/libGodinElfHook.so", pk_name, "lib");

	//LOGTEST("Set HOOKFLAG");
	int setEvnPacName = setenv("ENVPACN", pk_name, 1);

	int setEvnLDPre = setenv("LD_PRELOAD", szLibSoPath, 1);
//	int setEvnLDPre1 = setenv("LD_PRELOAD", szLibSoPath1, 1);
	char* ld_preload = getenv("LD_PRELOAD");
	//LOGTEST("LD_PRELOAD : %s", ld_preload);

	// set env "HOOKFLAG" to start really hook.
	if (m_versdk[0] >= '2')
	{
		if (m_verrel[0] >= '6')
			int setEvnRes = setenv("HOOKFLAG", "6.0true", 1);
		else
			int setEvnRes = setenv("HOOKFLAG", "true", 1);
	}
	else
		int setEvnRes = setenv("HOOKFLAG", "4.4true", 1);
	
	
	jobject objectCustomLoader = env->NewObject(classDexLoader, initMID,
			inputPath, outputPath, libpath, objectBootLoader);
	jmethodID setId = env->GetMethodID(classDexLoader, "setIs_write_back", "(I)V");
    env->CallVoidMethod(objectCustomLoader, setId, g_is_dex);
    LOGTEST("native art set is_write_back is ok");
	//LOGTEST("inputPath:%s",dexPath);
	//LOGTEST("outputPath:%s",szFilesDirBuf);
	//LOGTEST("libpath:%s",szLibPath);

	// unset env "HOOKFLAG" to finish really hook.
	int unSetEvnRes = unsetenv("HOOKFLAG");
	int unSetEvnLDPre = unsetenv("LD_PRELOAD");
	int unsetEvnPacName = unsetenv("ENVPACN");

	env->DeleteLocalRef(inputPath);
	env->DeleteLocalRef(outputPath);
	env->DeleteLocalRef(libpath);

	//set the path classloader's parent as the custom classloader
	jfieldID fieldID = env->GetFieldID(classClassLoader, "parent",
			"Ljava/lang/ClassLoader;");
	if (fieldID == NULL) {
		LOGTEST("erro to get the classloader class's parent field");
	}
	env->SetObjectField(pathClassLoader, fieldID, objectCustomLoader);
	env->DeleteLocalRef(classDexLoader);
	env->DeleteLocalRef(classContext);
	env->DeleteLocalRef(classClassLoader);
	//return objectCustomLoader;
	LOGTEST("End SetUp ART DexClassLoader");
}

int getModWhen(int fd, u4* modTime) {
	struct stat buf;
	int result = fstat(fd, &buf);

	if (result < 0) {
		//LOGTEST("Unable to determine mod time\n");
		return -1;
	}

	*modTime = (u4) buf.st_mtime;
	return 0;
}

unsigned long getSoAddr(const char *name){
  unsigned int ret = 0;
  char buf[4096], *temp;
  int pid;
  FILE *fp;
  pid = getpid();
  sprintf(buf, "/proc/%d/maps", pid);
  fp = fopen(buf, "r");
  if(fp == NULL)
  {
    puts("open failed");
    goto _error;
  }
  while(fgets(buf, sizeof(buf), fp)){
    if(strstr(buf, name)){
      temp = strtok(buf, "-");
      ret = strtoul(temp, NULL, 16);
      break;
    }
  }
_error:
  fclose(fp);
  return ret;
}

unsigned long getApkSoAddr(char *name){
  unsigned int ret = 0;
  char buf[4096], *temp;
  int pid;
  FILE *fp;
  pid = getpid();
  sprintf(buf, "/proc/%d/maps", pid);
  fp = fopen(buf, "r");
  if(fp == NULL)
  {
    puts("open failed");
    goto _error;
  }
  while(fgets(buf, sizeof(buf), fp)){
    if(strstr(buf, name) && !strstr(buf, "system/lib")){
      temp = strtok(buf, "-");
      ret = strtoul(temp, NULL, 16);
      break;
    }
  }
_error:
  fclose(fp);
  return ret;
}

unsigned long getApkSoEnd(char *name){
  unsigned int ret = 0;
  char buf[4096], *temp, *temp2;
  int pid;
  FILE *fp;
  pid = getpid();
  sprintf(buf, "/proc/%d/maps", pid);
  fp = fopen(buf, "r");
  if(fp == NULL)
  {
    puts("open failed");
    goto _error;
  }
  while(fgets(buf, sizeof(buf), fp)){
    if(strstr(buf, name) && !strstr(buf, "system/lib")){
      temp = strtok(buf, "-");
      temp2 = strtok(NULL, " ");
      ret = strtoul(temp2, NULL, 16);
      break;
    }
  }
_error:
  fclose(fp);
  return ret;
}

int findSymbol(const char *name, const char *libn,
                unsigned long *addr) {
    return find_name(getpid(), name, libn, addr);
}

int htoi(char s[])
{
    int i;
    int n = 0;
    if (s[0] == '0' && (s[1]=='x' || s[1]=='X'))
    {
        i = 2;
    }
    else
    {
        i = 0;
    }
    for (; (s[i] >= '0' && s[i] <= '9') || (s[i] >= 'a' && s[i] <= 'z') || (s[i] >='A' && s[i] <= 'Z');++i)
    {
        if (tolower(s[i]) > '9')
        {
            n = 16 * n + (10 + tolower(s[i]) - 'a');
        }
        else
        {
            n = 16 * n + (tolower(s[i]) - '0');
        }
    }
    return n;
}

int decrySo(soinfo *lib, char *soname){

	LOGTEST("dec start");
	unsigned char sTemp[256]={0};
	memcpy(sTemp, s, 256);
		

	//for(int i=0;i<16;i++){ LOGTEST("key_sokey: %x", sokey[i]); }
	SecInfo *sec;
	int i;
	int ret = mprotect((void*)lib->base, lib->size, 7);
	LOGTEST("mprotect");
	unsigned dump_size = lib->size; 
	LOGTEST("so_base:%d", lib->base);
	LOGTEST("so_size:%d", lib->size);
	LOGTEST("so_phnum:%d", lib->phnum);
    	unsigned buf_size = dump_size + 0x10;
	
	
	
	Elf32_Ehdr ehdr;
	memcpy(&ehdr, lib->base, sizeof(Elf32_Ehdr));
	
	LOGTEST("so_shnum:%d", ehdr.e_shnum);
	LOGTEST("so_phnum:%d", ehdr.e_phnum);
	LOGTEST("so_shoff:%d", ehdr.e_shoff);
	LOGTEST("so_shstrndx:%d", ehdr.e_shstrndx);
	LOGTEST("so_type:%d", ehdr.e_type);
	
	char so_path[256] = {0};
	char so_off[8] = {0};
	char so_size[8] = {0};
	sprintf(so_path, "/data/data/%s/files/%s", pk_name, soname);
	
	LOGTEST("so_path: %s", so_path);

	int so_fd = open(so_path, O_RDONLY);
	if(so_fd < 0){
		LOGTEST("open_so failed");
	}
	lseek(so_fd, -16, SEEK_END);
    	read(so_fd, so_off, 8);
    	lseek(so_fd, -8, SEEK_END);
    	read(so_fd, so_size, 8);

	close(so_fd);
	

	unsigned long text_offset = htoi(so_off);
	unsigned long text_size = htoi(so_size);	
	
	
	//unsigned long text_offset = ehdr.e_entry;
	//unsigned long text_size = ehdr.e_shentsize*65535+ehdr.e_shstrndx;	
	unsigned int npage = text_size / PAGE_SIZE + ((text_size % PAGE_SIZE == 0) ? 0 : 1);
	LOGTEST("text_sized: %x", text_size);
	LOGTEST("text_offset: %x", text_offset);
	
	if(mprotect( (void *)(lib->base+text_offset-(text_offset%PAGE_SIZE)), (npage+1)*PAGE_SIZE, PROT_READ | PROT_EXEC | PROT_WRITE) != 0){
		LOGTEST("mprotect1 fail");	
	}
	//mprotect( (void *)(lib->base+text_offset), text_size, 7);
	LOGTEST("mp2");

	unsigned char *buff = new unsigned char [buf_size];
	memcpy(buff, (void*)lib->base, lib->size);
/*
	FILE* origfp = fopen("/sdcard/dump_origso.txt","wb");
	if(soname == "/data/data/sina.mobile.tianqitong/lib/libijkffmpeg.so"){
	if(origfp == NULL){
		LOGTEST("fopenerror: %s", strerror(errno));

	}else{
		fwrite((void*)buff, 1, dump_size, origfp);
		LOGTEST("mp5");
	}
	}
*/


	if(mprotect( (void *)(lib->base+text_offset-(text_offset%PAGE_SIZE)), (npage+1)*PAGE_SIZE, PROT_READ | PROT_EXEC | PROT_WRITE) != 0){
		LOGTEST("mprotect1 fail");	
	}
	
	unsigned char *addr = new unsigned char[text_size];
	memcpy(addr, lib->base+text_offset, text_size);
	
	

        LOGTEST("addr = lib->base + text_offset-1: %x", lib->base);
        LOGTEST("addr = lib->base + text_offset-1: %x", text_offset);
	LOGTEST("addr: %x", addr);
	
/*	
	FILE* enfp = fopen("/sdcard/miwenso.txt", "wb");
	if(enfp == NULL){
		LOGTEST("fopenerror: %s", strerrror(errno));
	}else{ 
		fwrite((void*)addr, 1, text_size, enfp);	
	}
	
*/
	//for(int k=0;k<16;k++){LOGTEST("aaa: %d", addr[k]);}
       	unsigned char* b;

	rc4_crypt(sTemp, (unsigned char *)addr, text_size, (unsigned char *)addr, 0);
/*
	if(strstr(soname,"libpuzzles.so")){	
		for(int k=0;k<16;k++){LOGTEST("aaaa: %x", addr[k]);}
	}
*/
	//for(int k = 0;k<16;k++){LOGTEST("bbb: %d", b[k]);}
	if(mprotect( (void *)(lib->base+text_offset-(text_offset%PAGE_SIZE)), (npage+1)*PAGE_SIZE, PROT_READ | PROT_EXEC | PROT_WRITE) != 0){
		LOGTEST("mprotect1 fail");	
	}

	
		
	memcpy(lib->base+text_offset, addr,text_size);	
	
	//memcpy(addr, b, text_size);	
	//for(int k=0;k<16;k++){LOGTEST("aaaaa: %d", addr[k]);}
	
/*	
	FILE* jfp = fopen("/sdcard/jiemiso.txt", "wb");
	if(jfp == NULL){
		LOGTEST("fopenerror: %s", strerror(errno));
	}else{
		fwrite((void*)addr, 1, text_size, jfp);	
	}
*/
	LOGTEST("mp3");
	if(mprotect( (void *)(lib->base+text_offset-(text_offset%PAGE_SIZE)), (npage+1)*PAGE_SIZE, PROT_READ | PROT_EXEC | PROT_WRITE) != 0){
		LOGTEST("mprotect1 fail");	
	}
	//mprotect( (void *)(lib->base+text_offset), text_size, 7);
	LOGTEST("mp4");
	

	LOGTEST("dec end");

	return 0;
}

void mapinfo(){
	LOGTEST("start maps info");
		  char buf[4096], *temp;
  	int pid;
  	FILE *fp;
  	pid = getpid();
  	sprintf(buf, "/proc/%d/maps", pid);
	
	  fp = fopen(buf, "r");
  	if(fp == NULL)
  	{
    	puts("open failed");
    	goto _error;
  	}
	LOGTEST("maps");	
	while(fgets(buf, sizeof(buf), fp)){
		if(!strstr(buf, "system/lib")&&strstr(buf, "data/data/")){
    			LOGTEST("maps info :%s", buf);
		}
 	}
	_error:
  		fclose(fp);	

	LOGTEST("end maps info");
	
}

void findrightso(char* soname, unsigned long *base, unsigned long *end){
				
	char buf[4096], *temp1, *temp2, *privilege;
  	int pid;
	long size = 0;
	unsigned long head, tail;
  	FILE *fp;
  	pid = getpid();
  	sprintf(buf, "/proc/%d/maps", pid);
	
	  fp = fopen(buf, "r");
  	if(fp == NULL)
  	{
    	puts("open failed");
    	goto _error;
  	}
	LOGTEST("maps");	
	while(fgets(buf, sizeof(buf), fp)){
    	if(strstr(buf, soname) && !strstr(buf, "system/lib")){
      		LOGTEST("lib:%s",buf);
		//i = 1;
		temp1 = strtok(buf, "-");
		temp2 = strtok(NULL, " ");
		privilege = strtok(NULL, " ");
		head = strtoul(temp1, NULL, 16);
		tail = strtoul(temp2, NULL, 16);
		if( ((tail-head)>size)&&strstr(privilege, "x") ){
			LOGTEST("privilege: %s", privilege);
			size = tail-head;
			*base = head;
			*end = tail;		
		}
				
	}
     	

    		
 	}
	
	_error:
  		fclose(fp);	
	LOGTEST("base %d  end %d", *base, *end);
	LOGTEST("so size: %x", size);
	
}


int initmap(char *soname){
	
	int i = 0;
/*
	  char buf[4096], *temp;
  	int pid;
  	FILE *fp;
  	pid = getpid();
  	sprintf(buf, "/proc/%d/maps", pid);
	
	  fp = fopen(buf, "r");
  	if(fp == NULL)
  	{
    	puts("open failed");
    	goto _error;
  	}
	LOGTEST("maps");	
	while(fgets(buf, sizeof(buf), fp)){
    	if(strstr(buf, "opus")){
      		LOGTEST("libopus:%s",buf);
		i = 1;
     	 	//break;
    		}else if(strstr(buf, "libRDO")){
		LOGTEST("already load:%s",buf);
	}else if(strstr(buf, "libOuc")){
		LOGTEST("already load:%s",buf);
	}
 	}
	_error:
  		fclose(fp);	

*/

//	unsigned long addr;
//	unsigned long end;
/*	
	pid = fork();
	
	if(pid == 0){
		for(i=0;;i++){
			LOGTEST("wait");
			addr = getApkSoAddr("libopus");
			end = getApkSoEnd("libopus");
			if(addr&&end){
				LOGTEST("libopus addrs:%x end:%x", addr, end);
				break;
			}
		}
	}
*/

        unsigned long addr, end;
	findrightso(soname, &addr, &end);
	LOGTEST("init addr:%x end:%x",addr,end);
	
	string str = soname;
	if( ( mSo[str] == 0)&&(addr != 0) ){
		LOGTEST("init strstr:%s", soname);
		mSo[str] = 1;
		LOGTEST("lib map base:%x",addr);
		unsigned long end = getApkSoEnd(soname);
		unsigned char sTemp[256]={0};
		memcpy(sTemp, s, 256);
		void *base = addr;
		char so_path[256] = {0};
		char so_off[8] = {0};
		char so_size[8] = {0};
		sprintf(so_path, "/data/data/%s/files/%s", pk_name, soname);
	
		LOGTEST("so_path: %s", so_path);

		int so_fd = open(so_path, O_RDONLY);
		if(so_fd < 0){
			LOGTEST("open_so failed");
		}
		lseek(so_fd, -16, SEEK_END);
    		read(so_fd, so_off, 8);
    		lseek(so_fd, -8, SEEK_END);
    		read(so_fd, so_size, 8);
		close(so_fd);
	
		unsigned long text_offset = htoi(so_off);
		unsigned long text_size = htoi(so_size);	
		unsigned int npage = text_size / PAGE_SIZE + ((text_size % PAGE_SIZE == 0) ? 0 : 1);
		LOGTEST("init text_sized: %x", text_size);
		LOGTEST("init text_offset: %x", text_offset);
	
		if(mprotect( (void *)(base+text_offset-(text_offset%PAGE_SIZE)), (npage+1)*PAGE_SIZE, PROT_READ | PROT_EXEC | PROT_WRITE) != 0){
			LOGTEST("mprotect1 fail");	
		}
	
		unsigned char *tmpbuf = new unsigned char[text_size];
		memcpy(tmpbuf, base+text_offset, text_size);	
		for(int j =0;j<8;j++){
			LOGTEST("libtest orig insns:%x", tmpbuf[j]);
		}
		rc4_crypt(sTemp, (unsigned char *)tmpbuf, text_size, (unsigned char *)tmpbuf, 0);
		
		for(int i=0;i < 8;i++){
			LOGTEST("libopus insns:%x", tmpbuf[i]);
		}

		if(mprotect( (void *)(base+text_offset-(text_offset%PAGE_SIZE)), (npage+1)*PAGE_SIZE, PROT_READ | PROT_EXEC | PROT_WRITE) != 0){
			LOGTEST("mprotect1 fail");	
		}
		memcpy(base+text_offset, tmpbuf,text_size);

		char path[256] = {0};
		sprintf(path,"/data/data/%s/files/%s", pk_name, "opus_text.txt");
		FILE *libfp = fopen(path, "wb");
		if(libfp){
			LOGTEST("dump opus");
			fwrite((void*)base, 1, (end-addr), libfp);		
		}else{
			LOGTEST("fopenerror: %s", strerror(errno));
		}

		
		LOGTEST("init end");
			
	
	}


/*	
	if(mprotect( (void *)addr, 1024*32, PROT_READ | PROT_EXEC | PROT_WRITE) != 0){
		LOGTEST("mprotect1 fail");	
	}

	void * ins = addr;
	
	char * insns = (char *)ins;
	LOGTEST("insns: %x", insns);
	
	

	for(int i=0;i<64;i++){
		LOGTEST("opus insns: %x", insns[i]);
	}
*/

/*	
	unsigned long base = getApkSoAddr("libopus");
	unsigned long end = getApkSoEnd("libopus");
	
	LOGTEST("end ins");
	LOGTEST("libopus base:%x", base);
	char path[256] = {0};
	sprintf(path,"/data/data/%s/files/%s", pk_name, "opus.txt");
	FILE *libfp = fopen(path, "wb");

	if(libfp){
		LOGTEST("dump opus");
		fwrite((void*)base, 1, (end-base), libfp);		
	}else{
		LOGTEST("fopenerror: %s", strerror(errno));
	}
*/
	return i;
}


soinfo* (*orig_findLibrary)(android_namespace_t* ns, const char* name, int rtld_flags, const android_dlextinfo* extinfo, soinfo* needed_by) =NULL;

soinfo* (*orig_findLibrary5_1)(const char* name, int dlflags, const android_dlextinfo* extinfo) = NULL;



soinfo *new_findLibrary(android_namespace_t* ns, const char* name, int rtld_flags, const android_dlextinfo* extinfo, soinfo* needed_by){
	LOGTEST("hook findlibrary");

	soinfo *lib = NULL;
	int sof = 0;
	//initmap(sof);
//	mapinfo();
	LOGTEST("maps info current: %s", name);
	
	int flag=0;
	//char *apkname = getenv("ENVPACN");
	LOGTEST("apkname: %s", pk_name);
	char so_name_path[256] = {0}; 
	sprintf(so_name_path, "/data/data/%s/files/%s", pk_name, "soname.txt");
	LOGTEST("sprintf");
	FILE* so_name_fp= fopen(so_name_path, "rb");
	if(so_name_fp == NULL){LOGTEST("fopenerror: %s", strerror(errno));}
	LOGTEST("so_name_path: %s",so_name_path);
	string so_name_str[256];
	char s[100];
	int i = 0;
	while(fscanf(so_name_fp, "%s", s) != EOF){
		so_name_str[i] = s;
		//LOGTEST("so_name:: %s", s);
		i++;
	}
	LOGTEST("so_name_get");
	fclose(so_name_fp);

	if(soget ==0){
	for(int t =0;t<i;t++){
		mSo[so_name_str[t]] = 0;
		soget++;
	}	
	}
	char c[256] = {0};
	char so[256] = {0};
	for(int k=0;k<i;k++){
		strcpy(c, so_name_str[k].c_str());
		LOGTEST("c: %s",c);
	if(name != NULL){
		if(strstr(name, c)){
			flag = 1;
			mSo[c] = 1;
			LOGTEST("strstr: %s", c);
			//sprintf(name, "/data/data/%s/files/%s", pk_name, c);
			strcpy(so,c);

		} 
	} 
	}
	LOGTEST("soso: %s", so);

//       flag = 0;
//	if(strstr(name, "libtest.so")){
//		sprintf(name, "/data/data/%s/files/%s", "com.example.administrator.mytest", "libtest.so");//
//	}
		
	if(!flag){
														
		lib = orig_findLibrary(ns, name, rtld_flags, extinfo, needed_by);
	}else{	
//		sprintf(name, "/data/data/%s/files/%s", pk_name, so);
		LOGTEST("appso name: %s", name);
		lib = orig_findLibrary(ns, name, rtld_flags, extinfo, needed_by);
		decrySo(lib,so);
		
		LOGTEST("so_base::%d", lib->base);
		LOGTEST("so_szie::%d", lib->size);
	}

	LOGTEST("second");
//	mapinfo();



	for(int s=0;s<i;s++){
		unsigned long addr = 0;
		char* soname = new char[256];
		strcpy(soname, so_name_str[s].c_str());
		addr = getApkSoAddr(soname);
		if(addr){
			initmap(soname);
		}
		
	}
	


	return lib;
}




soinfo *new_findLibrary5_1(const char* name, int dlflags, const android_dlextinfo* extinfo){
	LOGTEST("hook findlibrary");

	soinfo *lib = NULL;
	int sof = 0;
	//initmap(sof);
		
	//mapinfo();
	LOGTEST("maps info current: %s", name);
	int flag=0;
	//char *apkname = getenv("ENVPACN");
	LOGTEST("apkname: %s", pk_name);
	char so_name_path[256] = {0}; 
	sprintf(so_name_path, "/data/data/%s/files/%s", pk_name, "soname.txt");
	LOGTEST("sprintf");
	FILE* so_name_fp= fopen(so_name_path, "rb");
	if(so_name_fp == NULL){LOGTEST("fopenerror: %s", strerror(errno));}
	LOGTEST("so_name_path: %s",so_name_path);
	string so_name_str[256];
	char s[100];
	int i = 0;
	while(fscanf(so_name_fp, "%s", s) != EOF){
		so_name_str[i] = s;
		//LOGTEST("so_name:: %s", s);
		i++;
	}
	LOGTEST("so_name_get");
	fclose(so_name_fp);

	if(soget ==0){
	for(int t =0;t<i;t++){
		mSo[so_name_str[t]] = 0;
		soget++;
	}	
	}
	char c[256] = {0};
	char so[256] = {0};
	for(int k=0;k<i;k++){
		strcpy(c, so_name_str[k].c_str());
		LOGTEST("c: %s",c);
	if(name != NULL){
		if(strstr(name, c)){
			flag = 1;
			mSo[c] = 1;
			LOGTEST("strstr: %s %d", c, mSo[c]);
			
			strcpy(so,c);
		} 
	} 
	}
	LOGTEST("soso: %s", so);

	if(!flag){
														
		lib = orig_findLibrary5_1(name, dlflags, extinfo);
	}else{	
		LOGTEST("appso name: %s", name);
		lib = orig_findLibrary5_1(name, dlflags, extinfo);
		decrySo(lib,so);
		
		LOGTEST("so_base::%d", lib->base);
		LOGTEST("so_szie::%d", lib->size);
	}
	//mapinfo();
	for(int s=0;s<i;s++){
		unsigned long addr = 0;
		char* soname = new char[256];
		strcpy(soname, so_name_str[s].c_str());
		addr = getApkSoAddr(soname);
		if(addr&&mSo[so_name_str[s]]==0){
			LOGTEST("libjni: %d",mSo["libjni"]);
			initmap(soname);
		}
		
	}

	return lib;
}

soinfo* (*orig_loadLibrary)(int fd, off64_t file_offset, LoadTaskList& load_tasks, const char* name, int rtld_flags, const android_dlextinfo* extinfo) = NULL;

soinfo* new_loadLibrary(int fd, off64_t file_offset, LoadTaskList& load_tasks, const char* name, int rtld_flags, const android_dlextinfo* extinfo){
	LOGTEST("hook loadlibrary");

	soinfo *lib = NULL;
	//mapinfo();
	LOGTEST("maps info current: %s", name);

	int flag=0;
	//char *apkname = getenv("ENVPACN");
	LOGTEST("apkname: %s", pk_name);
	char so_name_path[256] = {0}; 
	sprintf(so_name_path, "/data/data/%s/files/%s", pk_name, "soname.txt");
	LOGTEST("sprintf");
	FILE* so_name_fp= fopen(so_name_path, "rb");
	if(so_name_fp == NULL){LOGTEST("fopenerror: %s", strerror(errno));}
	//LOGTEST("so_name_path: %s",so_name_path);
	string so_name_str[256];
	char s[100];
	int i = 0;

	while(fscanf(so_name_fp, "%s", s) != EOF){
		so_name_str[i] = s;
		LOGTEST("so_name:: %s", s);
		i++;
	}
	LOGTEST("so_name_get");
	fclose(so_name_fp);

	
	char c[256] = {0};
	char so[256] = {0};
	for(int k=0;k<i;k++){
		strcpy(c, so_name_str[k].c_str());
		//LOGTEST("c: %s",c);
	if(name != NULL){
		if(strstr(name, c)){
			flag = 1;
			LOGTEST("strstr: %s", c);
			LOGTEST("name: %s", name);
			strcpy(so,c);
		} 
	} 
	}
	LOGTEST("soso: %s", so);
	
	//flag = 0;
/*	
	if(strstr(name, "libtest.so")){
		sprintf(name, "/data/data/%s/files/%s", pk_name, "libtest.so");
	}	

*/
	if(!flag){
		LOGTEST("unhook");
														
		lib = orig_loadLibrary(fd, file_offset, load_tasks, name, rtld_flags, extinfo);
	}else{	
		LOGTEST("appso name: %s", name);
		lib = orig_loadLibrary(fd, file_offset, load_tasks, name, rtld_flags, extinfo);
		decrySo(lib,so);

		LOGTEST("so_base::%d", lib->base);
		LOGTEST("so_szie::%d", lib->size);
	}
	
	//mapinfo();
	return lib;
}

soinfo* (*orig_find_library4)(const char* name) = NULL;
soinfo* new_find_library4(const char* name){
	LOGTEST("hook findlibrary4");

	soinfo *lib = NULL;
	//mapinfo();
	LOGTEST("maps info current: %s", name);
	LOGTEST("so-v7a: %s", name);

	int flag=0;
	//char *apkname = getenv("ENVPACN");
	LOGTEST("apkname: %s", pk_name);
	char so_name_path[256] = {0}; 
	sprintf(so_name_path, "/data/data/%s/files/%s", pk_name, "soname.txt");
	LOGTEST("sprintf");
	FILE* so_name_fp= fopen(so_name_path, "rb");
	if(so_name_fp == NULL){LOGTEST("fopenerror: %s", strerror(errno));}
	LOGTEST("so_name_path: %s",so_name_path);
	string so_name_str[256];
	char s[100];
	int i = 0;
	while(fscanf(so_name_fp, "%s", s) != EOF){
		so_name_str[i] = s;
		LOGTEST("so_name:: %s", s);
		i++;
	}
	LOGTEST("so_name_get");
	fclose(so_name_fp);

	
	char c[256] = {0};
	char so[256] = {0};
	for(int k=0;k<i;k++){
		strcpy(c, so_name_str[k].c_str());
		LOGTEST("c: %s",c);
	if(name != NULL){
		if(strstr(name, c)){
			flag = 1;
			LOGTEST("strstr: %s", c);
			strcpy(so,c);
		} 
	} 
	}
	LOGTEST("soso: %s", so);


	flag = 0;
	
	if(strstr(name, "libtest.so")){
		sprintf(name, "/data/data/%s/files/%s", pk_name, "libtest.so");
	}


	if(!flag){
														
		lib = orig_find_library4(name);
	}else{	
		LOGTEST("appso name: %s", name);
		lib = orig_find_library4(name);
		decrySo(lib,so);
		
		LOGTEST("so_base::%d", lib->base);
		LOGTEST("so_szie::%d", lib->size);
	}
	//mapinfo();
	
	return lib;
}

static bool (*orig_findlibraries)(android_namespace_t* ns, soinfo* start_with, const char* const library_names[], size_t library_names_count, soinfo* soinfos[], std::vector<soinfo*>* ld_preloads, size_t ld_preloads_count, int rtld_flags, const android_dlextinfo* extinfo, bool add_as_children) = NULL;
static bool new_findlibraries(android_namespace_t* ns, soinfo* start_with, const char* const library_names[], size_t library_names_count, soinfo* soinfos[], std::vector<soinfo*>* ld_preloads, size_t ld_preloads_count, int rtld_flags, const android_dlextinfo* extinfo, bool add_as_children){
	for(size_t i=0; i<library_names_count; i++){
		LOGTEST("libraries name :%s", library_names[i]);
	}	
			
	return orig_findlibraries(ns, start_with, library_names, library_names_count, soinfos, ld_preloads, ld_preloads_count, rtld_flags, extinfo, add_as_children);

}
void hookfindlibraries(){
    	void *symbol = NULL;
	if (findSymbol("__dl__ZL14find_librariesP19android_namespace_tP6soinfoPKPKcjPS2_PNSt3__16vectorIS2_NS8_9allocatorIS2_EEEEjiPK17android_dlextinfob", "linker",(unsigned long *) &symbol) == 0) {
        	MSHookFunction(symbol, (void *) new_findlibraries, (void **) &orig_findlibraries);
    	}
}

static bool (*old_load_library)(android_namespace_t* ns, LoadTask* task, LoadTaskList* load_tasks, int rtld_flags, const std::string& realpath) = NULL;
static bool new_load_library(android_namespace_t* ns, LoadTask* task, LoadTaskList* load_tasks, int rtld_flags, const std::string& realpath){
	string strtmp = realpath;
	const char* p = strtmp.c_str(); 
	LOGTEST("load_library: %s", p);
	mapinfo();
	
	return old_load_library(ns, task, load_tasks, rtld_flags, realpath);
}
void hookloadlibrary(){
    	void *symbol = NULL;
	if (findSymbol("__dl__ZL12load_libraryP19android_namespace_tP8LoadTaskPNSt3__16vectorIS2_NS3_9allocatorIS2_EEEEiRKNS3_12basic_stringIcNS3_11char_traitsIcEENS5_IcEEEE", "linker",(unsigned long *) &symbol) == 0) {
        	MSHookFunction(symbol, (void *) new_load_library, (void **) &old_load_library);
    	}	
}

static int (*oldopen_library_inzip)(ZipArchiveCache* zip_archive_cache, const char* const input_path, off64_t* file_offset, std::string* realpath) = NULL;
static int myopen_library_inzip(ZipArchiveCache* zip_archive_cache, const char* const input_path, off64_t* file_offset, std::string* realpath){
	LOGTEST("open_library");
	return oldopen_library_inzip;
}
void hookopenlibrary(){
    	void *symbol = NULL;
	if (findSymbol("__dl__ZL23open_library_in_zipfileP15ZipArchiveCachePKcPxPNSt3__112basic_stringIcNS4_11char_traitsIcEENS4_9allocatorIcEEEE", "linker",(unsigned long *) &symbol) == 0) {
        	MSHookFunction(symbol, (void *) myopen_library_inzip, (void **) &oldopen_library_inzip);
    	}		
}

static soinfo* (*old_soinfo_alloc)(android_namespace_t* ns,  const char* name, struct stat* file_stat, off64_t file_offset, uint32_t rtld_flags) = NULL;
static soinfo* mysoinfo_alloc(android_namespace_t* ns,  const char* name, struct stat* file_stat, off64_t file_offset, uint32_t rtld_flags){
	LOGTEST("soinfo_alloc: %s", name);
/*	const char *tmp;
	if (name == NULL) {
		
		sprintf(tmp, "/data/data/%s/files/%s", pk_name, "libtest.so");
	}
*/
	return old_soinfo_alloc(ns, name, file_stat, file_offset, rtld_flags);

}
void hooksoinfo_alloc(){
    	void *symbol = NULL;
	if (findSymbol("__dl__ZL12soinfo_allocP19android_namespace_tPKcP4statxj", "linker",(unsigned long *) &symbol) == 0) {
        	MSHookFunction(symbol, (void *) mysoinfo_alloc, (void **) &old_soinfo_alloc);
    	}		
}

static void (*oldsoinfo_free)(soinfo si) = NULL;
static void mysoinfo_free(soinfo si){
	LOGTEST("soinfo_free: %s", si);
	return oldsoinfo_free(si);
}
void hooksoinfo_free(){
    	void *symbol = NULL;
	if (findSymbol("__dl__ZL11soinfo_freeP6soinfo", "linker",(unsigned long *) &symbol) == 0) {
        	MSHookFunction(symbol, (void *) mysoinfo_free, (void **) &oldsoinfo_free);
    	}		
}

static bool (*oldreadpath_fd)(int fd, std::string path) = NULL;
static bool myreadpath_fd(int fd, std::string path){
	const char* str = path.c_str();
	bool flag = oldreadpath_fd(fd, path);
	LOGTEST("readpath :path :%s   :%d", str, flag);
	return flag;
}
void hookreadpath_fd(){
    	void *symbol = NULL;
	if (findSymbol("__dl__ZL11realpath_fdiPNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEEE", "linker",(unsigned long *) &symbol) == 0) {
        	MSHookFunction(symbol, (void *) myreadpath_fd, (void **) &oldreadpath_fd);
    	}	


}
bool (*oldis_accessiable)(const std::string file) = NULL;
bool myis_accessiable(const std::string file){
	bool flag = oldis_accessiable(file);
	LOGTEST("is_accessiable: %d", flag);
	return flag;
}
void hookis_access(){
    	void *symbol = NULL;
	if (findSymbol("__dl__ZN19android_namespace_t13is_accessibleERKNSt3__112basic_stringIcNS0_11char_traitsIcEENS0_9allocatorIcEEEE", "linker",(unsigned long *) &symbol) == 0) {
        	MSHookFunction(symbol, (void *) myis_accessiable, (void **) &oldis_accessiable);
    	}	
}

void hook_findLibrary(){
    	void *symbol = NULL;
	char m_versdk[16] = {'\0'};
    	__system_property_get("ro.build.version.sdk", m_versdk);
	//LOGTEST("versdk-get:%s",m_versdk);
  	int versdk = atoi(m_versdk);
	unsigned long base = getSoAddr("linker");
	unsigned char linker_tag = 0;
	int fp_linker = open("/system/bin/linker", O_RDONLY);
	lseek(fp_linker, 0x84, SEEK_SET);
	read(fp_linker, &linker_tag, 1);
	LOGTEST("linker_tag: %x",linker_tag);
//	if(linker_tag == 0x60){
//		LOGTEST("good");
//	}
	close(fp_linker);

	if(versdk>=24){
		if (findSymbol("__dl__ZL12find_libraryP19android_namespace_tPKciPK17android_dlextinfoP6soinfo", "linker",(unsigned long *) &symbol) == 0) {
        		MSHookFunction(symbol, (void *) new_findLibrary, (void **) &orig_findLibrary);
    		}
	}else if(versdk == 23){
		if (findSymbol("__dl__ZL12load_libraryixR10LinkedListI8LoadTask18TypeBasedAllocatorI15LinkedListEntryIS0_EEEPKciPK17android_dlextinfo", "linker",(unsigned long *) &symbol) == 0) {
        		MSHookFunction(symbol, (void *) new_loadLibrary, (void **) &orig_loadLibrary);
    		}
	}else if(versdk <23&& versdk>19){
		if (findSymbol("__dl__ZL12find_libraryPKciPK17android_dlextinfo", "linker",(unsigned long *) &symbol) == 0) {
        		MSHookFunction(symbol, (void *) new_findLibrary5_1, (void **) &orig_findLibrary5_1);
    		}
	}else if(versdk <20){
		//start_hook_dlopen("/system/lib/libart.so", (void *)new_dlopen, "dlopen");
		//if (findSymbol("__dl_dlopen", "linker",(unsigned long *) &symbol) == 0) {
        	//	MSHookFunction(symbol, (void *) new_dlopen, (void **) &old_dlopen);
    		//}

		//char linker_tag[32] = {0};
		//unsigned long base = getSoAddr("linker");
		//FILE* fp_linker = fopen("/system/bin/linker", "rb");
		//fread(linker_tag, 1, 8, fp_linker);
		//fclose(fp_linker);
		//LOGTEST("linker4.4.4tag: %s", linker_tag);
		if(linker_tag == 0xe4){		
			unsigned long sub22B0_func = (base + 0x22b0) | 0x00000001;
			MSHookFunction((void *)sub22B0_func, (void *) new_find_library4, (void **) &orig_find_library4);
		}else if(linker_tag == 0x0c){
			unsigned long sub228c_func = (base + 0x228c) | 0x00000001;
			MSHookFunction((void *)sub228c_func, (void *) new_find_library4, (void **) &orig_find_library4);		
		}else if(linker_tag == 0xc4){
			unsigned long sub22ac_func = (base + 0x22ac) | 0x00000001;
			MSHookFunction((void *)sub22ac_func, (void *) new_find_library4, (void **) &orig_find_library4);			
		}else if(linker_tag == 0x50){
			unsigned long sub22b4_func = (base + 0x22b4) | 0x00000001;
			MSHookFunction((void *)sub22b4_func, (void *) new_find_library4, (void **) &orig_find_library4);			
		}


	}
}



void getLibInfo(){
/*
	char so_name_path[256] = {0}; 
	sprintf(so_name_path, "/data/data/%s/files/%s", pk_name, "soname.txt");
	LOGTEST("sprintf");
	FILE* so_name_fp= fopen(so_name_path, "rb");
	if(so_name_fp == NULL){LOGTEST("fopenerror: %s", strerror(errno));}
	LOGTEST("so_name_path: %s",so_name_path);
	string so_name_str[256];
	char s[100];
	int i = 0;
	while(fscanf(so_name_fp, "%s", s) != EOF){
		so_name_str[i] = s;
		LOGTEST("so_name:: %s", s);
		i++;
	}
	LOGTEST("so_name_get");
	fclose(so_name_fp);
*/


	char lib_bin_path[256] = {0}; 
	sprintf(lib_bin_path, "/data/data/%s/files/%s", pk_name, "lib.bin");
	FILE* file_bin = fopen(lib_bin_path, "rb");
	char soname_len[256] = {0};
	fgets(soname_len, 256, file_bin);
	int soname_size = atoi(soname_len);
	char so_name_path[256] = {0};
	sprintf(so_name_path, "/data/data/%s/files/%s", pk_name, "soname.txt");
	FILE* soname_info  =fopen(so_name_path, "wb");
	unsigned char *cont = new unsigned char[soname_size];
	fread(cont, 1, soname_size, file_bin);
	LOGTEST("soname_info: %s", soname_info);
	fwrite(cont, 1, soname_size, soname_info);
	fclose(soname_info);
	
	
	FILE* so_name_fp= fopen(so_name_path, "rb");
	if(so_name_fp == NULL){LOGTEST("fopenerror: %s", strerror(errno));}
	LOGTEST("so_name_path: %s",so_name_path);
	string so_name_str[256];
	char s[100];
	int i = 0;
	while(fscanf(so_name_fp, "%s", s) != EOF){
		so_name_str[i] = s;
		LOGTEST("so_name:: %s", s);
		i++;
	}
	LOGTEST("so_name_get");
	fclose(so_name_fp);

	
	LOGTEST("soname len: %d", soname_size);
	fseek(file_bin, 1, SEEK_CUR);
	
	for(int t=0; t<i; t++){
		char so_size[256] = {0};
		fgets(so_size, 256, file_bin);
		int so_len = atoi(so_size);
		LOGTEST("bin so len: %d", so_len);
		unsigned char *cont = new unsigned char[so_len];
		LOGTEST("bin so 1");
		fread(cont, 1, so_len, file_bin);
		LOGTEST("bin so 2");
		char sp[256] = {0};
		string strtmp = so_name_str[t];
		strcpy(sp, strtmp.c_str());
		LOGTEST("bin so: %s", sp);
		char lib[256] = {0};
		char lib2[256] = {0};
		char libdir[256] = {0};
//		sprintf(lib, "/data/data/%s/lib/%s", pk_name, sp);
		sprintf(lib2, "/data/data/%s/files/%s", pk_name, sp);
		sprintf(libdir, "/data/data/%s/lib/", pk_name);
		LOGTEST("bin lib: %s", lib);
//		chmod(libdir, S_IRWXU | S_IRWXG | S_IRWXO);
		//FILE* so = fopen(lib, "wb");
		FILE* so2 = fopen(lib2, "wb");
		//fclose(so);
		if(so2){
			LOGTEST("so2 ok");
		}
		fwrite(cont, 1, so_len, so2);
		LOGTEST("bin so 3");
		fclose(so2);
		so2 = NULL;
		fseek(file_bin, 1, SEEK_CUR);
		LOGTEST("bin so 4");		
	}	
	fclose(file_bin);
}

int myopen(const char *name, int flag){
	int fd = 0;
	fd = open(name, flag);
	LOGTEST("name %s  fd %d", name, fd);
	return fd;
}



/*************************************************************************
 * this function will run while the so is called
 * init the write mmap hook here because we should hook the newed process
 * --by-zhr
 **************************************************************************/
__attribute__((constructor)) void initHookBeforeJNIOnload() {
	LOGTEST("initHookBeforeJNIOnload");
	__android_log_print(ANDROID_LOG_INFO, "JNITaginit", "Find index = %d\n", 111);
	

	//start_hook_open("/system/lib/libc.so",(void *)myopen,  "open");
	//hookis_access();
	//hookreadpath_fd();
	//hooksoinfo_free();
	//hooksoinfo_alloc();
	//hookopenlibrary();
	//hookloadlibrary();
	//hookfindlibraries();
	
	
	start_hook_mmap("/system/lib/libart.so", (void *) mmap_hook_test, "mmap");
}
void ans_codeinfo(){
	FILE *file_bin;
	char filename[200];
	BIN_FILE_TYPE *binfiletype = (BIN_FILE_TYPE *)malloc(11*sizeof(int));
	unsigned char key[16];
	strncpy(filename, "/data/data/", 12);
	strncat(filename, pk_name, 200 - 13);
	strncat(filename, "/files/class.bin", 17);
	LOGTEST("zhangyate %s",filename);

	char infopath[256];

	strncpy(infopath, "/data/data/", 12);
	strncat(infopath, pk_name, 200 - 13);
	strncat(infopath, "/files/codeinfo.txt", 20);

    fstream pinfopath;
    pinfopath.open(infopath,ios::in|ios::binary);
	if (!pinfopath.is_open())
	{
		LOGTEST("exit***************    ");
		exit(1);
	}
	analysis_bin_file(filename, binfiletype);
	//LOGTEST("zhangyate %d   %d",binfiletype[0].key_start,binfiletype[0].key_size);

	file_bin = fopen(filename, "r");

	fseek(file_bin, binfiletype[0].key_start, SEEK_SET);
	fread(key, 1, binfiletype[0].key_size, file_bin);
	cppkname(pk_name);
	analysis_class_file(pinfopath,key);
	pinfopath.close();
	fclose(file_bin);
	free(binfiletype);
}
int getdexfromfiles(unsigned char ** pbinfile, long* dexLength){
	LOGTEST("get dex\n");

		long length;
		int dex_file_length;
		size_t numRead;
		char filename[200];
		BIN_FILE_TYPE *binfiletype = (BIN_FILE_TYPE *)malloc(11*sizeof(int));
		unsigned char key[16];
		unsigned char iv[16] = { 0x0 };
		unsigned char *ptemp;
		unsigned char *ptemp1;
		int i;
		unsigned char *dex_from_apk;
		int dex_from_apk_len;
		amac_ctx amac;
		char dexpath[256];

		char infopath[256];
		unsigned char dex_amac_result[16];
		unsigned char dexsigned[16];
		char openpath[256];

		unsigned char *M;
		unsigned int *table;
		FILE *fp_M;
		FILE *fp_t;
		strncpy(filename, "/data/data/", 12);
		strncat(filename, pk_name, 200 - 13);
		strncat(filename, "/files/class.bin", 17);
		//LOGTEST("zhangyate %s",filename);
		strncpy(dexpath, "/data/data/", 12);
		strncat(dexpath, pk_name, 200 - 13);
		strncat(dexpath, "/files/haha.dex", 16);
		//LOGTEST("zhangyate %s",dexpath);
	    FILE *pdexpath = fopen(dexpath, "rb");
	    if(pdexpath == NULL){
	    	LOGTEST("pdexpath   exit***************    ");
	    	exit(1);
	    }
		analysis_bin_file(filename, binfiletype);
		//LOGTEST("zhangyate %d   %d",binfiletype[0].key_start,binfiletype[0].key_size);
		*pbinfile = (unsigned char *) malloc(binfiletype[0].unzdex_size);
		fread(*pbinfile, 1, binfiletype[0].unzdex_size, pdexpath);
		*dexLength = binfiletype[0].unzdex_size;
		fclose(pdexpath);
		free(binfiletype);
	    LOGTEST("get dex ok");
		return true;
}
int unziphaha(){
	char dexPath[256] = { 0 };
	sprintf(dexPath, "/data/data/%s/files/haha.dex", pk_name);
	char dexPath1[256] = { 0 };
	sprintf(dexPath1, "/data/data/%s/files/haha1.dex", pk_name);
	unzFile ufdex;
	int err = UNZ_OK;
	unz_file_info64 dex_info;
	char dex_inzip[256] = { 0 };
	BIN_FILE_TYPE binfiletype;
	char filename[200];
	strncpy(filename, "/data/data/", 12);
	strncat(filename, pk_name, 200 - 13);
	strncat(filename, "/files/class.bin", 17);
	//LOGTEST("zhangyate %s",filename);
	analysis_bin_file(filename, &binfiletype);
	ufdex = unzOpen64(dexPath);
	if (!ufdex) {
		LOGTEST("1");
		//return false;
	}
	err = unzGetCurrentFileInfo64(ufdex, &dex_info, dex_inzip,sizeof(dex_inzip), NULL, 0, NULL, 0);
	if (UNZ_OK != err) {
		//LOGTEST("1");
	    //return false;
	}
	err = unzOpenCurrentFilePassword(ufdex,NULL);
	if (UNZ_OK != err) {
		//LOGTEST("1");
		//return false;
	}

	void *buf = (void *) malloc(binfiletype.unzdex_size);
	if (!buf) {
		LOGTEST("shenmegui");
	}
	unzReadCurrentFile(ufdex, buf, binfiletype.unzdex_size);
	unzCloseCurrentFile(ufdex);
    unsigned char cmpHead2[4];
    memcpy(cmpHead2, (char *)buf + 8, 4);
    for(int i = 0; i < 4; ++i){
    	//LOGTEST("cmpHead2 %x", cmpHead2[i]);
    }

	char checksum[256] = { 0 };
	sprintf(checksum, "/data/data/%s/files/check.sum", pk_name);
    FILE *oatchecksum = fopen(checksum,"wb");
    fwrite(cmpHead2, 1, 4, oatchecksum);
    fclose(oatchecksum);

	//remove(dexPath);
	int tof =  access(dexPath1, 0);
    if(tof == -1){
    FILE *unzipdex = fopen(dexPath1, "wb");
    if(unzipdex == NULL){
    	LOGTEST("unzipdex open err");
    	LOGTEST("errno %d", errno);
    }
    errno = 0;
    fwrite(buf, 1, binfiletype.unzdex_size, unzipdex);
    LOGTEST("errno %d", errno);
    fclose(unzipdex);
    }
    free(buf);
    char dex2oatname[256] = {0};
	sprintf(dex2oatname, "/data/data/%s/files/dex2oat.dex", pk_name);
	int tof1 =  access(dex2oatname, 0);
	if(tof == -1){
	    FILE *dex2oatfile = fopen(dex2oatname, "wb");
        unsigned char *buf1 = (unsigned char *)malloc(binfiletype.unzdex_size * sizeof(unsigned char));
        memset(buf1, '0', binfiletype.unzdex_size);
        unsigned char dexheader[8] = { 0x64, 0x65, 0x78, 0x0A, 0x30, 0x33, 0x35, 0x00 };
        memcpy(buf1, dexheader, 8);
	    fwrite(buf1, 1, binfiletype.unzdex_size, dex2oatfile);
	    fclose(dex2oatfile);
	    free(buf1);
	}
	return 0;
}
jint JNI_OnLoad(JavaVM* vm, void* reserved) {
	JNIEnv* env = NULL;
	jclass cls = NULL;
	jint result = JNI_ERR;
	char *p = NULL;
	bool checkMode = 0;
	if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
		return JNI_ERR;
	}
	getApplicationPackagename(env, pk_name);
	strncpy(pkname, pk_name, 200);

	start_app();
	
	g_is_dex = readConfig(2);
	int is_so = readConfig(3);
	int is_debug = readConfig(4);
	int is_data = readConfig(5);

	LOGTEST("is_dex: %d", g_is_dex);
	LOGTEST("is_so: %d", is_so);
	LOGTEST("is_debug: %d", is_debug);
	LOGTEST("is_data: %d", is_data);
    if(is_so==1){
        LOGTEST(" is_so running...... ");
        hook_findLibrary();
    }

    if(is_debug==1){
    LOGTEST(" is_debug running...... ");
        protect_ndebug(pkname);
    	protect_jdebug();
    }


	LOGTEST(" ========== Mode check ========== ");
	checkMode = isArtMode(env);
	getLibInfo();
	if(is_data==1){
	    	start_hook_read("/system/lib/libjavacore.so",(void *)fi_read,  "read");
            start_hook_write("/system/lib/libjavacore.so",(void *)fi_write, "write");
            start_hook_open("/system/lib/libjavacore.so",(void *)fi_open,  "open");
            start_hook_close("/system/lib/libjavacore.so",(void *)fi_close, "close");
	}
//	LOGTEST("hook_fi");
//	start_hook_read("/system/lib/libjavacore.so",(void *)fi_read,  "read");
//	start_hook_write("/system/lib/libjavacore.so",(void *)fi_write, "write");
//	start_hook_open("/system/lib/libjavacore.so",(void *)fi_open,  "open");
//	start_hook_close("/system/lib/libjavacore.so",(void *)fi_close, "close");
//	hookfi();
	if (checkMode) {
		LOGTEST("this is in ART mode");
		LOGTEST("keytrans");
		/**********************************************************
		 * decrypt the whole dex
		 * only leave the 0x28 bytes in the head in case the check
		 * --by-zhr
		 **********************************************************/
		pid = getpid();
		unsigned char *pbinfile = NULL;
		checksign(env, &pbinfile);
		char filename[256] = { 0 };
		char newfilename[256] = { 0 };
		char flagname[256] = { 0 };
		FILE *flagfile;
		int artdexlength;
		sprintf(flagname, "/data/data/%s/files/dexflag.flag", pk_name);
		sprintf(newfilename, "/data/data/%s/files/haha1.dex", pk_name);
		chmod(newfilename, S_IRWXU | S_IRWXG | S_IRWXO);
		getLibInfo();
		unziphaha();
		ans_codeinfo();
		SetUpARTDexClassLoader(env, newfilename);

	} else {	// this is in dalvik mode
		LOGTEST("this is in dlavik mode");
		unsigned char *pbinfile = NULL;
		long dexLength = 0;
		getdexfromfiles(&pbinfile, &dexLength);
		checksign(env, &pbinfile);
		LOGTEST("wtf");
		gDexLen = dexLength;
		getLibInfo();
        unziphaha();
		char dexPath[256] = { 0 };
		sprintf(dexPath, "/data/data/%s/files/haha1.dex", pk_name);
		chmod(dexPath, S_IRWXU | S_IRWXG | S_IRWXO);
		free(pbinfile);
		ans_codeinfo();
		LOGTEST("ans_codeinfo FINISH");
		start_hook_mmap("/system/lib/libdvm.so", (void *) mmap_hook, "mmap");
		SetUpDexClassLoader(env, dexPath);

	}

	const char *module_Path = "/system/lib/libsqlite.so";

	if(is_data==1){
	    start_hook_database_open(module_Path,(void *)my_open, "open");
        start_hook_database_close(module_Path,(void *)my_close, "close");
        start_hook_database_read(module_Path,(void *)my_pread64, "pread64");
        start_hook_database_write(module_Path,(void *)my_pwrite64, "pwrite64");
	}
//	start_hook_database_open(module_Path,(void *)my_open, "open");
//	start_hook_database_close(module_Path,(void *)my_close, "close");
//	start_hook_database_read(module_Path,(void *)my_pread64, "pread64");
//	start_hook_database_write(module_Path,(void *)my_pwrite64, "pwrite64");

	return JNI_VERSION_1_4;
}

