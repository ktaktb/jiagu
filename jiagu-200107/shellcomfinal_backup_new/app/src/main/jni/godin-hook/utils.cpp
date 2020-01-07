/*===============================================================
*   Copyright (C) 2016 All rights reserved.
*
*   文件名称：utils.cpp
*   创 建 者：genglei.cuan@godinsec.com
*   创建日期：2016年05月16日
*   描    述：内存操作工具类
*
*   更新日志：
*
================================================================*/

#include "utils.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>

size_t GodinHook::Utils::getModuleBaseAddress(const char *moduleName, pid_t pid=0)
{
  char maps[24] = {0};
  FILE* fd = NULL;
  if(pid>0){
     sprintf(maps,"/proc/%d/maps",pid);
     fd = fopen(maps, "r");
  }else if(0 == pid)
  fd = fopen("/proc/self/maps", "r");

  char buf[2048] = {0};
  size_t baseAddress = 0;
  size_t offsets = 0;
  if(NULL == fd)
    return 0;
  while(fgets(buf,2048,fd)!=NULL){
    /// 根据/proc/<pid>/maps格式可知，每行末尾为加载的文件的名字
    if(strstr(buf,moduleName)==NULL)
      continue;
    else{
       const char *sep = "\t \r\n";
       char *buff = NULL;
       char *paddr = strtok_r(buf, sep, &buff);
       char *pflags = strtok_r(NULL, sep, &buff);
       char *offset =strtok_r(NULL, sep, &buff);       // offsets
       char *pdev = strtok_r(NULL, sep, &buff);        // dev number.
       char *node = strtok_r(NULL, sep, &buff);        // node
       char *pfilename = strtok_r(NULL, sep, &buff);   //module name
       /// 再次判断
       if(strcmp(moduleName,pfilename)!=0)
         return 0;
       if(NULL != offset){
         #if defined(__LP64__)
         offsets = strtoll(offset,NULL,16);
         if(0 == offsets)
            baseAddress = strtoull(strtok_r(paddr,"-",&buff),NULL,16);
         #else
         offsets = strtoul(offset,NULL,16);
         if(0 == offsets)
            baseAddress = strtoul(strtok_r(paddr,"-",&buff),NULL,16);
         #endif
         /*printf("----->addr: %s\n",paddr);
         if(0 == offsets){
           printf("----->name: %s\n",pfilename);
           printf("----->addr: %s\n",paddr);
         }
         */
       }
    }
  }
  fclose(fd);
  return baseAddress;

}


bool GodinHook::Utils::unProtectMemory(size_t addr, int size)
{
   /// 获得当前系统的内存页大小
    int pageSize = sysconf(_SC_PAGESIZE);

    /// 计算所在内存页中的偏移
    int align = addr % pageSize;

    int ret  = mprotect((void*)(addr-align),pageSize,PROT_READ|PROT_WRITE);//PROT_WRITE
    if(-1 == ret){
        perror("mprotect22");
        return false;
      }
    return true;
}

bool GodinHook::Utils::protectMemory(size_t addr, int size)
{
  /// 获得当前系统的内存页大小
   int pageSize = sysconf(_SC_PAGESIZE);

   /// 计算所在内存页的偏移
   int align = addr % pageSize;

   //int ret  = mprotect((void*)(addr-align),(size_t)(size+align),PROT_READ);//PROT_WRITE
   int ret  = mprotect((void*)(addr-align),pageSize,PROT_READ);//PROT_WRITE
   if(-1 == ret){
       perror("mprotect33");
       return false;
     }
   return true;
}

