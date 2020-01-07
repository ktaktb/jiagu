#include <string.h>
#include <jni.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <string.h>
#include <dlfcn.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/system_properties.h>
#include <dirent.h>
#include <pthread.h>
#include <unistd.h>
#include <android/log.h>
#include <zlib.h>
#include <zconf.h>
#include <sys/mman.h>


#ifdef ANDROID_L_AFTER
#include <sys/statvfs.h>
#endif
#include "godin-hook/godin_elf_hook.h"
//#include "include/native_hook.h"



unsigned int start_hook_mmap(const char *, void *, const char *);
unsigned int start_hook_open(const char *, void *, const char *);
unsigned int start_hook_close(const char *, void *, const char *);
unsigned int start_hook_read(const char *, void *, const char *);
unsigned int start_hook_write(const char *, void *, const char *);
unsigned int start_hook_execv(const char *module_path, void *hook_func, const char *symbol_name);
unsigned int start_hook_execve(const char *, void *, const char *);
unsigned int start_hook_execl(const char *module_path, void *hook_func, const char *symbol_name);
unsigned int start_hook_dlopen(const char *, void *, const char *);

extern void *(*backMmap)(void *addr, size_t length, int prot, int flags,int fd, off_t offset);
extern int(*backExecv)(const char *path, char *const argv[]);
extern int(*backExecve)(const char * filename,char * const argv[],char * const envp[]);
extern int(*backExecl)(const char *path, const char *arg, ...);
extern void*(*backDlopen)(const char* filename, int flags);

unsigned int start_hook_database_open(const char *, void *, const char *);
unsigned int start_hook_database_close(const char *, void *, const char *);
unsigned int start_hook_database_read(const char *, void *, const char *);
unsigned int start_hook_database_write(const char *, void *, const char *);
unsigned int start_hook_dlopen(const char *, void *, const char *);


