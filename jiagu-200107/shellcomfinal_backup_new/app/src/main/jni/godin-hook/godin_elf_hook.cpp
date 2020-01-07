/*===============================================================
*   Copyright (C) 2016 All rights reserved.
*
*   鏂囦欢鍚嶇О锛歡odin_elf_hook.cpp
*   鍒� 寤� 鑰咃細genglei.cuan@godinsec.com
*   鍒涘缓鏃ユ湡锛�2016骞�05鏈�16鏃�
*   鎻�    杩帮細hook妗嗘灦瀵瑰API
*
*   鏇存柊鏃ュ織锛�
*
================================================================*/
#include "godin_elf_hook.h"
#include <string.h>
#include "utils.h"





//GodinHook::GodinELfHook::ModuleMap GodinHook::GodinELfHook::module_map_ = map<string,HookModule*>();
GodinHook::GodinELfHook*  GodinHook::GodinELfHook::instance = NULL;

bool GodinHook::GodinELfHook::isRegisteredModule(string name)
{
  ModuleMap::iterator it = module_map_.find(name);
  if(it == module_map_.end())
    return false;
  else
    return true;
}

GodinHook::GodinELfHook::RETINFO GodinHook::GodinELfHook::registeredElfModule(string name,pid_t pid)
{

  /// 鍒ゆ柇鏄惁宸茬粡娉ㄥ唽
  if(isRegisteredModule(name))
    return EFL_MODULE_ALLREADY_REGISTERED;

  /// 璁＄畻moudle鍦ㄨ繘绋嬩腑鍔犺浇鐨勫熀鍦板潃
  size_t baseAddr = Utils::getModuleBaseAddress(name.c_str(),pid);
  if(0 == baseAddr)
    return NOT_FIND_ELFMODULE;

  /// 鍒ゆ柇baseAddr澶勬槸鍚︽槸涓�涓悎娉曠殑ELF鏂囦欢
  if(!isElfFile(baseAddr))
    return NOT_ELF_FILE;

  /// 鍒涘缓HookModule
  HookModule * module = new HookModule(name,baseAddr);
  if(NULL == module)
    return MEM_ERR;

  /// 鍒濆鍖朚odule
  if(!module->initModule())
    return ELF_MODULE_INIT_ERR;
  //LOGV("6666666666-----");
  /// 淇敼module 鐘舵��
  module->setIsRegistered(true);
 //LOGV("8888888888");
  // 娉ㄥ唽Module
  module_map_.insert(map<string,HookModule*>::value_type(name,module));
//  module_map_[name]=module;
 // LOGV("77777777-----");

  return GODIN_OK;

}

bool GodinHook::GodinELfHook::isHookedInModule(string moduleName, string hookSymbol)
{
  if(!isRegisteredModule(moduleName))
      return false;
  HookModule * module = getHookModule(moduleName);
  return module->isSymbolAllreadyHooked(hookSymbol);

}

GodinHook::GodinELfHook::RETINFO GodinHook::GodinELfHook::hook(string moduleName, string hookSymbol,void *newFunc, void **backFunc)
{
  /// 妯″潡鏄惁宸茬粡琚敞鍐�
  if(!isRegisteredModule(moduleName))
    return EFL_MODULE_NOT_REGISTERRD;

  /// symbol鏄惁宸茬粡琚玥ook
  if(isHookedInModule(moduleName,hookSymbol))
    return ALLREADY_HOOKED;

  /// 寮�濮媓ook
  HookModule * module = getHookModule(moduleName);
  if(module->hook(hookSymbol.c_str(),newFunc,backFunc)){
    module->addHookedSymbol(hookSymbol);
    return HOOKED;
  }else
    return HOOKED_ERR;
}
GodinHook::GodinELfHook::RETINFO GodinHook::GodinELfHook::hook(GodinHook::HookModule *module, string hookSymbol, void *newFunc, void **backFunc)
{
  /// symbol鏄惁宸茬粡琚玥ook
  if(module->isSymbolAllreadyHooked(hookSymbol))
    return ALLREADY_HOOKED;
  /// 寮�濮媓ook
  if(module->hook(hookSymbol.c_str(),newFunc,backFunc)){
    module->addHookedSymbol(hookSymbol);
    return HOOKED;
  }else
    return HOOKED_ERR;
}

GodinHook::GodinELfHook::RETINFO GodinHook::GodinELfHook::unhook(GodinHook::HookModule *module, string hookSymbol, void *backFunc)
{
  /// symbol鏄惁宸茬粡琚玥ook
  if(!module->isSymbolAllreadyHooked(hookSymbol))
    return NOT_HOOKED;
  /// 寮�濮媢nhook
  if(module->unhook(hookSymbol.c_str(),backFunc)){
    return GODIN_OK;
  }else
    return UNHOOKED_ERR;
}

GodinHook::GodinELfHook::RETINFO GodinHook::GodinELfHook::hookAllRegisteredModule(string hookSymbol, void *newFunc, void **backFunc)
{
 // LOGV("---hook all----");
  ModuleMap::iterator it;
  for(it = module_map_.begin();it != module_map_.end();it++){
    HookModule * module = it->second;
    RETINFO ret = hook(module,hookSymbol,newFunc,backFunc);
    if(HOOKED_ERR == ret)
      return ret;
  }
  return GODIN_OK;
}

GodinHook::GodinELfHook::RETINFO GodinHook::GodinELfHook::unHook(string moduleName, string hookSymbol,void *backFunc)
{

  /// 妯″潡鏄惁宸茬粡琚敞鍐�
  if(!isRegisteredModule(moduleName))
    return EFL_MODULE_NOT_REGISTERRD;
  /// symbol鏄惁宸茬粡琚玥ook
  if(!isHookedInModule(moduleName,hookSymbol))
    return NOT_HOOKED;
  /// 杩涜unhook
  /// 鍏奸【璧勬簮鍥炴敹
  HookModule * module = getHookModule(moduleName);
  if(!module->unhook(hookSymbol.c_str(),backFunc))
    return UNHOOKED_ERR;
  module->removeHookedSymbol(hookSymbol);
  delete  module;
  module = NULL;
  module_map_.erase(moduleName);
  return GODIN_OK;
}

GodinHook::GodinELfHook::RETINFO GodinHook::GodinELfHook::unHookAllRegisteredModule(string hookSymbol, void *backFunc)
{
  ModuleMap::iterator it = module_map_.begin();
  while(it != module_map_.end()){
    HookModule * module = it->second;
    RETINFO ret = unhook(module,hookSymbol,backFunc);
    if(UNHOOKED_ERR == ret)
      return ret;
    else if(GODIN_OK == ret){
      module->removeHookedSymbol(hookSymbol);
      delete  module;
      module = NULL;
      module_map_.erase(it++);
    }else if(NOT_HOOKED == ret)
      it++;
  }
  return GODIN_OK;
}



/// 鍙傝�� AOSP/bionic/linker/linker_phdr.cpp::VerifyElfHeader()
bool GodinHook::GodinELfHook::isElfFile(size_t baseAddress)
{
  void * base_addr = (void*)baseAddress;
  ELFW(Ehdr) *ehdr = reinterpret_cast<ELFW(Ehdr) *>(base_addr);

  /// 鍓嶅洓瀛楄妭鍐呭锛氣�淺177ELF鈥�
  if (memcmp(ehdr->e_ident, ELFMAG, SELFMAG) != 0) {
      return false;
  }


  /// 32 杩樻槸 64
  int elf_class = ehdr->e_ident[EI_CLASS];
#if defined(__LP64__)
  if (elf_class != ELFCLASS64) {
      return false;
  }
#else
  if (elf_class != ELFCLASS32) {
      return false;
  }
#endif
  if (ehdr->e_ident[EI_DATA] != ELFDATA2LSB) {
      return false;
  }
  if (ehdr->e_version != EV_CURRENT) {
      return false;
  }
  if (ehdr->e_machine != GetTargetElfMachine()) {
      return false;
  }
  return true;
}

/// 鍙傝�� AOSP/bionic/linker/linker_phdr.cpp::GetTargetElfMachine()
int GodinHook::GodinELfHook::GetTargetElfMachine()
{
#if defined(__arm__)
  return EM_ARM;
#elif defined(__aarch64__)
  return EM_AARCH64;
#elif defined(__i386__)
  return EM_386;
#elif defined(__mips__)
  return EM_MIPS;
#elif defined(__x86_64__)
  return EM_X86_64;
#endif
}

GodinHook::HookModule *GodinHook::GodinELfHook::getHookModule(string name)
{
  ModuleMap::iterator it = module_map_.find(name);

  return it->second;
}
