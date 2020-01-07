#include <stdio.h>
#include <jni.h>
#include <android/log.h>
#include <assert.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#ifdef ANDROID_L_AFTER
#include <sys/statvfs.h>
#endif
#include "godin-hook/godin_elf_hook.h"
//#include "include/native_hook.h"
#include <dlfcn.h>
#include <dirent.h>
#include "godin_io_hook.h"
//#include <cutils/properties.h>
#define TAG "testsec"
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE,TAG,__VA_ARGS__)



void*(*backDlopen)(const char* filename, int flags);


int(*backOpen)(const char *pathname ,int flags, mode_t mode);
int(*backChmod)(const char *pathname , mode_t mode);
int(*backAccess)(const char *pathname , mode_t mode);
int(*backMkdir)(const char *pathname , mode_t mode);
int(*backRemove)(const char *pathname);
int(*backClose)(int fd);
int(*backWrite)(int fd,char *buff,int len);
int(*backRead)(int fd,char *buff,int len);
int(*backExecv)(const char *path, char *const argv[]);
int(*backExecve)(const char *filename, char *const argv[],char *const envp[]);
int(*backChown)(const char *path, uid_t owner, gid_t group);
int(*backRename)(const char *oldpath, const char *newpath);
int(*backStat)(const char *path, struct stat *buf);
int(*backStatvfs)(const char *path, struct statvfs *buf);
int(*backSymlink)(const char *oldpath, const char *newpath);
int(*backExecl)(const char *path, const char *arg, ...);
void *(*backMmap)(void *addr, size_t length, int prot, int flags,int fd, off_t offset);

char *(*backGetenv)(const char *name);
FILE *(*backFopen)(const char *path, const char *mode);

int (*backSprintf)(char *str, const char *format, ...);
DIR *(*backOpendir)(const char *name);
void *(*backDlsym)(void *handle, const char *symbol);

int(*my_open)(const char *pathname, int oflag);
int(*my_close)(int fd);
size_t(*my_pread64)(int fd,char *buff,size_t nbyte,off64_t offset);
size_t(*my_pwrite64)(int fd,char *buff,size_t nbyte, off64_t offset);


/*
unsigned int start_hook(const char *module_path, void *hook_func, const char *symbol_name)
{
	LOGV("hook----starthook_mmap begin");
	GodinHook::GodinELfHook::RETINFO ret = GodinHook::GodinELfHook::registeredElfModule(module_path);
	if((ret == GodinHook::GodinELfHook::EFL_MODULE_ALLREADY_REGISTERED) ||(ret == GodinHook::GodinELfHook::GODIN_OK))
	    {
	        ret = GodinHook::GodinELfHook::hook(module_path,symbol_name,hook_func,(void**)&backMmap);
	    }

   return ret;
}
*/



 unsigned int start_hook_mmap(const char *module_path, void *hook_func, const char *symbol_name)
{
	LOGV("hook----starthook_mmap begin");
	GodinHook::GodinELfHook::RETINFO ret = GodinHook::GodinELfHook::getInstance()->registeredElfModule(module_path,0);
	LOGV("mmap : register ret = %d", ret);
	if((ret == GodinHook::GodinELfHook::EFL_MODULE_ALLREADY_REGISTERED) ||(ret == GodinHook::GodinELfHook::GODIN_OK))
	    {
		ret = GodinHook::GodinELfHook::getInstance()->hook(module_path,symbol_name,hook_func,(void**)&backMmap);
		LOGV("mmap : hook ret = %d", ret);
	    }

    return ret;
}
 unsigned int start_hook_write(const char *module_path, void *hook_func, const char *symbol_name)
{
	//LOGV("hook----starthook_write begin");
	GodinHook::GodinELfHook::RETINFO ret = GodinHook::GodinELfHook::getInstance()->registeredElfModule(module_path,0);
	//LOGV("write : register ret = %d", ret);
	if((ret == GodinHook::GodinELfHook::EFL_MODULE_ALLREADY_REGISTERED) ||(ret == GodinHook::GodinELfHook::GODIN_OK))
	    {
		ret = GodinHook::GodinELfHook::getInstance()->hook(module_path,symbol_name,hook_func,(void**)&backWrite);
		//LOGV("write : hook ret = %d", ret);
	    }

    return ret;
}
 unsigned int start_hook_execv(const char *module_path, void *hook_func, const char *symbol_name)
{
	LOGV("hook----starthook_execv begin");
	GodinHook::GodinELfHook::RETINFO ret = GodinHook::GodinELfHook::getInstance()->registeredElfModule(module_path,0);
	LOGV("execv : register ret = %d", ret);
	if((ret == GodinHook::GodinELfHook::EFL_MODULE_ALLREADY_REGISTERED) ||(ret == GodinHook::GodinELfHook::GODIN_OK))
	    {
		ret = GodinHook::GodinELfHook::getInstance()->hook(module_path,symbol_name,hook_func,(void**)&backExecv);
		LOGV("execv : hook ret = %d", ret);
	    }

    return ret;
}

 unsigned int start_hook_execve(const char *module_path, void *hook_func, const char *symbol_name)
{
	LOGV("hook----starthook_execve begin");
	GodinHook::GodinELfHook::RETINFO ret = GodinHook::GodinELfHook::getInstance()->registeredElfModule(module_path,0);
	LOGV("execve : register ret = %d", ret);
	if((ret == GodinHook::GodinELfHook::EFL_MODULE_ALLREADY_REGISTERED) ||(ret == GodinHook::GodinELfHook::GODIN_OK))
	    {
		ret = GodinHook::GodinELfHook::getInstance()->hook(module_path,symbol_name,hook_func,(void**)&backExecve);
		LOGV("execve : hook ret = %d", ret);
	    }

    return ret;
}

 unsigned int start_hook_execl(const char *module_path, void *hook_func, const char *symbol_name)
{
	LOGV("hook----starthook_execl begin");
	GodinHook::GodinELfHook::RETINFO ret = GodinHook::GodinELfHook::getInstance()->registeredElfModule(module_path,0);
	LOGV("execve : register ret = %d", ret);
	if((ret == GodinHook::GodinELfHook::EFL_MODULE_ALLREADY_REGISTERED) ||(ret == GodinHook::GodinELfHook::GODIN_OK))
	    {
		ret = GodinHook::GodinELfHook::getInstance()->hook(module_path,symbol_name,hook_func,(void**)&backExecl);
		LOGV("execve : hook ret = %d", ret);
	    }

    return ret;
}

 unsigned int start_hook_open(const char *module_path, void *hook_func, const char *symbol_name)
{
	LOGV("hook----starthook_open begin");
	GodinHook::GodinELfHook::RETINFO ret = GodinHook::GodinELfHook::getInstance()->registeredElfModule(module_path,0);
	LOGV("open : register ret = %d", ret);
	if((ret == GodinHook::GodinELfHook::EFL_MODULE_ALLREADY_REGISTERED) ||(ret == GodinHook::GodinELfHook::GODIN_OK))
	    {
		ret = GodinHook::GodinELfHook::getInstance()->hook(module_path,symbol_name,hook_func,(void**)&backOpen);
		LOGV("open : hook ret = %d", ret);
	    }

    return ret;
}

 unsigned int start_hook_close(const char *module_path, void *hook_func, const char *symbol_name)
{
	LOGV("hook----starthook_close begin");
	GodinHook::GodinELfHook::RETINFO ret = GodinHook::GodinELfHook::getInstance()->registeredElfModule(module_path,0);
	LOGV("close : register ret = %d", ret);
	if((ret == GodinHook::GodinELfHook::EFL_MODULE_ALLREADY_REGISTERED) ||(ret == GodinHook::GodinELfHook::GODIN_OK))
	    {
		ret = GodinHook::GodinELfHook::getInstance()->hook(module_path,symbol_name,hook_func,(void**)&backClose);
		LOGV("close : hook ret = %d", ret);
	    }

    return ret;
}

 unsigned int start_hook_read(const char *module_path, void *hook_func, const char *symbol_name)
{
	LOGV("hook----starthook_read begin");
	GodinHook::GodinELfHook::RETINFO ret = GodinHook::GodinELfHook::getInstance()->registeredElfModule(module_path,0);
	LOGV("read : register ret = %d", ret);
	if((ret == GodinHook::GodinELfHook::EFL_MODULE_ALLREADY_REGISTERED) ||(ret == GodinHook::GodinELfHook::GODIN_OK))
	    {
		ret = GodinHook::GodinELfHook::getInstance()->hook(module_path,symbol_name,hook_func,(void**)&backRead);
		LOGV("read : hook ret = %d", ret);
	    }

    return ret;
}

 unsigned int start_hook_database_open(const char *module_path, void *hook_func, const char *symbol_name)
{
	LOGV("hook----starthook_open begin");
	GodinHook::GodinELfHook::RETINFO ret = GodinHook::GodinELfHook::getInstance()->registeredElfModule(module_path,0);
	LOGV("open : register ret = %d", ret);
	if((ret == GodinHook::GodinELfHook::EFL_MODULE_ALLREADY_REGISTERED) ||(ret == GodinHook::GodinELfHook::GODIN_OK))
	    {
		ret = GodinHook::GodinELfHook::getInstance()->hook(module_path,symbol_name,hook_func,(void**)&my_open);
		LOGV("open : hook ret = %d", ret);
	    }

    return ret;
}

 unsigned int start_hook_database_close(const char *module_path, void *hook_func, const char *symbol_name)
{
	LOGV("hook----starthook_close begin");
	GodinHook::GodinELfHook::RETINFO ret = GodinHook::GodinELfHook::getInstance()->registeredElfModule(module_path,0);
	LOGV("close : register ret = %d", ret);
	if((ret == GodinHook::GodinELfHook::EFL_MODULE_ALLREADY_REGISTERED) ||(ret == GodinHook::GodinELfHook::GODIN_OK))
	    {
		ret = GodinHook::GodinELfHook::getInstance()->hook(module_path,symbol_name,hook_func,(void**)&my_close);
		LOGV("close : hook ret = %d", ret);
	    }

    return ret;
}

 unsigned int start_hook_database_read(const char *module_path, void *hook_func, const char *symbol_name)
{
	LOGV("hook----starthook_read begin");
	GodinHook::GodinELfHook::RETINFO ret = GodinHook::GodinELfHook::getInstance()->registeredElfModule(module_path,0);
	LOGV("read : register ret = %d", ret);
	if((ret == GodinHook::GodinELfHook::EFL_MODULE_ALLREADY_REGISTERED) ||(ret == GodinHook::GodinELfHook::GODIN_OK))
	    {
		ret = GodinHook::GodinELfHook::getInstance()->hook(module_path,symbol_name,hook_func,(void**)&my_pread64);
		LOGV("read : hook ret = %d", ret);
	    }

    return ret;
}

 unsigned int start_hook_database_write(const char *module_path, void *hook_func, const char *symbol_name)
{
	LOGV("hook----starthook_write begin");
	GodinHook::GodinELfHook::RETINFO ret = GodinHook::GodinELfHook::getInstance()->registeredElfModule(module_path,0);
	LOGV("write : register ret = %d", ret);
	if((ret == GodinHook::GodinELfHook::EFL_MODULE_ALLREADY_REGISTERED) ||(ret == GodinHook::GodinELfHook::GODIN_OK))
	    {
		ret = GodinHook::GodinELfHook::getInstance()->hook(module_path,symbol_name,hook_func,(void**)&my_pwrite64);
		LOGV("write : hook ret = %d", ret);
	    }

    return ret;
}

 unsigned int start_hook_dlopen(const char *module_path, void *hook_func, const char *symbol_name)
{
	LOGV("hook----starthook_dlopen begin");
	GodinHook::GodinELfHook::RETINFO ret = GodinHook::GodinELfHook::getInstance()->registeredElfModule(module_path,0);
	LOGV("write : register ret = %d", ret);
	if((ret == GodinHook::GodinELfHook::EFL_MODULE_ALLREADY_REGISTERED) ||(ret == GodinHook::GodinELfHook::GODIN_OK))
	    {
		ret = GodinHook::GodinELfHook::getInstance()->hook(module_path,symbol_name,hook_func,(void**)&backDlopen);
		LOGV("write : hook ret = %d", ret);
	    }

    return ret;
}
