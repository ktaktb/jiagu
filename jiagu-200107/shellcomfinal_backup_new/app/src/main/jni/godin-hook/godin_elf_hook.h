/*===============================================================
*   Copyright (C) 2016 All rights reserved.
*
*   文件名称：godin_elf_hook.h
*   创 建 者：genglei.cuan@godinsec.com
*   创建日期：2016年05月16日
*   描    述：hook框架对外API
*
*   更新日志：
*
================================================================*/

#ifndef GODIN_ELF_HOOK_H
#define GODIN_ELF_HOOK_H

#include "godin_type.h"
#include <string>
#include <map>
#include "hook_module.h"

using namespace std;

namespace GodinHook {

  class GodinELfHook{


  public :

    enum RETINFO{
      GOIDINERR                                    = -1,
      REGISTERED                                   = 0,
      EFL_MODULE_ALLREADY_REGISTERED               = 1,
      HOOKED                                       = 2,
      NOT_ELF_FILE                                 = 3,
      MEM_ERR                                      = 4,
      ELF_MODULE_INIT_ERR                          = 5,
      GODIN_OK                                     = 6,
      ALLREADY_HOOKED                              = 7,
      EFL_MODULE_NOT_REGISTERRD                    = 8,
      HOOKED_ERR                                   = 9,
      NOT_HOOKED                                   = 10,
      UNHOOKED_ERR                                 = 11,
      NOT_FIND_ELFMODULE                           = 12,
    };
    /**
     * @brief isRegisteredModule
     * 判断ELF文件是否已经注册
     * @param name
     * ELF文件名字
     * @return
     * true,已经注册；
     * flase,没有注册；
     */
     bool isRegisteredModule(string name);

    /**
     * @brief registeredElfModule
     * 注册ELF Module，注册的过程中包括了对该elf模块dynamic的解析
     * @param name
     * ELF Mdule 名字
     * @return
     * 成功返回GODIN_OK，或者EFL_MODULE_ALLREADY_REGISTERED;
     * 失败返回其他 RETINFO
     */
     RETINFO registeredElfModule(string name,pid_t pid=0);

    /**
     * @brief isHookedInModule
     * 该symbol是否已经在该模块中hook
     * @param moduleName
     * elf module 模块
     * @param hookSymbol
     * 方法symbol
     * @return
     * true,已经被hook;
     * false,没有被hook;
     */
     bool isHookedInModule(string moduleName,string hookSymbol);


    /**
     * @brief hook
     * 对指定的ELF Module中的symbol进行hook
     * @param moduleName
     * ELF Module模块HOOKED
     * @param hookSymbol
     * function symbol
     * @param newFunc
     * 替换symbol对应的方法
     * @param backFunc
     * 备份symbol对应的原方法
     * @return
     * RETINFO
     */
     RETINFO hook(string moduleName,string hookSymbol, void *newFunc, void **backFunc);

    /**
     * @brief hookAllRegisteredModule
     * 对所有已经注册的ELF Module中的symbol进行hook
     * @param hookSymbol
     * function symbol
     * @param newFunc
     * 替换symbol对应的方法
     * @param backFunc
     * 备份symbol对应的原方法
     * @return
     */
     RETINFO hookAllRegisteredModule(string hookSymbol, void *newFunc, void **backFunc);

    /**
     * @brief unHook
     * 对指定的ELF Module中的symbol进行unhook
     * @param moduleName
     * ELF Module模块
     * @param hookSymbol
     * function symbol
     * @param backFunc
     * symbol对应的原方法备份
     * @return
     * RETINFO
     */
     RETINFO unHook(string moduleName,string hookSymbol,void *backFunc);

     RETINFO unHookAllRegisteredModule(string hookSymbol,void *backFunc);

   // static int  hookedCount();


  private:

     RETINFO hook(HookModule * module,string hookSymbol, void *newFunc, void **backFunc);
     RETINFO unhook(HookModule * module,string hookSymbol,void *backFunc);

    /**
     * @brief isElfFile
     * 判断指定内存中的内容是否是一个elf文件
     * @param baseAddress
     * 指定的内存地址
     * @return
     * 返回true，是elf文件；
     * 返回false，不是elf文件
     */
     bool isElfFile(size_t baseAddress);

    /**
     * @brief GetTargetElfMachine
     * 获取当前ELF Machine,验证ELF文件时需要用到
     * @return
     * 返回当前ELF Machine
     */
     int GetTargetElfMachine();

    /**
     * @brief getHookModule
     * 返回HookModule
     * @param name
     * 已经注册的 ELF Module 名字
     * @return
     * 返回HookModule *
     */
     HookModule * getHookModule(string name);

  public:
    static GodinELfHook* getInstance(){
      if(instance == NULL)
        instance = new GodinELfHook();
      return instance;
    }
  private:
    GodinELfHook(){}
  private:
    static GodinELfHook* instance;
    typedef map<string,HookModule*> ModuleMap;
    ModuleMap module_map_;
  };

}
#endif // GODIN_ELF_HOOK_H
