#ifndef UTILS_H
#define UTILS_H

#include "godin_type.h"
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

namespace GodinHook {
  class Utils{
  public :
    /**
     * @brief getModuleBaseAddress
     * 获取elf模块在进程中加载的首地址
     * @param moduleName
     * elf文件绝对路径+名字
     * @param pid
     * 进程ID，默认为当前进程
     * @return
     * 失败返回0；
     * 成功返回模块在进程中加载的首地址
     */
    static  size_t getModuleBaseAddress(const char * moduleName,pid_t pid=0);

    /**
     * @brief unProtectMemory
     * 对指的的内存区域添加写权限
     * @param addr
     * 起始地址
     * @param size
     * 需要修改权限的内存大小(由于对齐的原因，实际修改的大小可能比size要大)
     * @return
     * 成功返回true;
     * 失败返回false;
     */
    static bool unProtectMemory(size_t addr, int size);
    /**
     * @brief unProtectMemory
     * 对指的的内存区域移除写权限
     * @param addr
     * 起始地址
     * @param size
     * 需要修改权限的内存大小(由于对齐的原因，实际修改的大小可能比size要大)
     * @return
     * 成功返回true;
     * 失败返回false;
     */
    static bool protectMemory(size_t addr, int size);

  };
}

#endif // UTILS_H
