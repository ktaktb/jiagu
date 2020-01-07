/*===============================================================
*   Copyright (C) 2016 All rights reserved.
*
*   文件名称：hook_module.cpp
*   创 建 者：genglei.cuan@godinsec.com
*   创建日期：2016年05月16日
*   描    述：对ELF文件的操作
*
*   更新日志：
*
================================================================*/


#include "hook_module.h"
#include <assert.h>
#include "elf_relocs.h"
#include "utils.h"
#include <unistd.h>//android cacheflush
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>

#if defined(_TEST_DEBUG_ANDROID)
#define LOGTEST(...)   __android_log_print(3,"testsec", __VA_ARGS__)
#else
#define LOGTEST(...)
#endif

/// AOSP6.0/bionic/libc/kernel/uapi/linux/elf.h
#define DT_LOOS 0x6000000d
/// AOSP6.0/bionic/libc/include/elf.h
/* Android compressed rel/rela sections */
#define DT_ANDROID_REL (DT_LOOS + 2)
#define DT_ANDROID_RELSZ (DT_LOOS + 3)

#define DT_ANDROID_RELA (DT_LOOS + 4)
#define DT_ANDROID_RELASZ (DT_LOOS + 5)

#define DT_GNU_HASH 0x6ffffef5

#define powerof2(x) ((((x)-1)&(x))==0)

//void **backFuncAddr;





bool GodinHook::HookModule::initModule()
{

  this->ehdr_ = reinterpret_cast<ELFW(Ehdr)*>(this->base_addr_);
  this->shdr_ = reinterpret_cast<ELFW(Shdr)*>(this->base_addr_ + this->ehdr_->e_shoff);
  this->phdr_ = reinterpret_cast<ELFW(Phdr)*>(this->base_addr_ + this->ehdr_->e_phoff);

  /// 判断elf模块是否可以被hook
  if((ET_EXEC != this->ehdr_->e_type) && (ET_DYN != this->ehdr_->e_type))
    return false;

  /// 计算bias，后续内存操作均需要加上此偏移值
  if(0 == this->bias_addr_){
    for(int i=0;i<this->ehdr_->e_phnum;i++){
      /// elf文件地址+load段在elf文件内偏移-load段虚拟地址
      if(PT_LOAD == (this->phdr_)[i].p_type){
        this->bias_addr_ = reinterpret_cast<ELFW(Addr>(this->ehdr_))+(this->phdr_)[i].p_offset - (this->phdr_)[i].p_vaddr;
        /// 以第一个PT_LOAD为基准
        //printf("-------------this->bias_addr_ %p\n",this->bias_addr_);
        break;
      }
    }
  }
  /// 获取dynamic section地址以及大小
  for(int i=0;i<this->ehdr_->e_phnum;i++){
    if(PT_DYNAMIC == (this->phdr_)[i].p_type){
      this->dyn_ptr_  = reinterpret_cast<ELFW(Dyn)*>(this->bias_addr_ + (this->phdr_)[i].p_vaddr);
      this->dyn_size_ = (this->phdr_)[i].p_memsz;
    }
  }
  if(0 == this->dyn_size_)
    return false;
  /// 开始解析dynamic section
  /// dynamic最有一项为DT_NULL
  for (ELFW(Dyn)* dyn = this->dyn_ptr_; dyn->d_tag != DT_NULL; ++dyn){
    switch(dyn->d_tag)
    {
      /// 符号表
      case DT_SYMTAB:
        this->sym_ptr_ = reinterpret_cast<ELFW(Sym) *>(this->bias_addr_ + dyn->d_un.d_ptr);
        break;
      /// 字符串表
      case DT_STRTAB:
        this->strtab_ = reinterpret_cast<const char *>(this->bias_addr_  + dyn->d_un.d_ptr);
        break;
      case DT_PLTREL:
        #if defined(USE_RELA)
        if (dyn->d_un.d_val != DT_RELA) {
          //DL_ERR("unsupported DT_PLTREL in \"%s\"; expected DT_RELA", get_realpath());
          return false;
        }
        #else
        if (dyn->d_un.d_val != DT_REL) {
          //DL_ERR("unsupported DT_PLTREL in \"%s\"; expected DT_REL", get_realpath());
          return false;
        }
        #endif
        break;
#if defined(USE_RELA)
      case DT_RELA:
        dyn_rela_ = reinterpret_cast<ELFW(Rela)*>(this->bias_addr_ + dyn->d_un.d_ptr);
        break;

      case DT_RELASZ:
        dyn_rela_count_ = dyn->d_un.d_val / sizeof(ELFW(Rela));
        break;

      case DT_ANDROID_RELA:
        android_relocs_ = reinterpret_cast<uint8_t*>(this->bias_addr_ + dyn->d_un.d_ptr);
        break;

      case DT_ANDROID_RELASZ:
        android_relocs_size_ = dyn->d_un.d_val;
        break;

      case DT_ANDROID_REL:
       // DL_ERR("unsupported DT_ANDROID_REL in \"%s\"", get_realpath());
        return false;

      case DT_ANDROID_RELSZ:
       // DL_ERR("unsupported DT_ANDROID_RELSZ in \"%s\"", get_realpath());
        return false;

      case DT_RELAENT:
        if (dyn->d_un.d_val != sizeof(ELFW(Rela))) {
          //DL_ERR("invalid DT_RELAENT: %zd", static_cast<size_t>(dyn->d_un.d_val));
          return false;
        }
        break;

      // ignored (see DT_RELCOUNT comments for details)
      case DT_RELACOUNT:
        break;

      case DT_REL:
       // DL_ERR("unsupported DT_REL in \"%s\"", get_realpath());
        return false;

      case DT_RELSZ:
        //DL_ERR("unsupported DT_RELSZ in \"%s\"", get_realpath());
        return false;

#else
      case DT_REL:
             dyn_rel_ = reinterpret_cast<ELFW(Rel)*>(this->bias_addr_ + dyn->d_un.d_ptr);
             break;

           case DT_RELSZ:
             dyn_rel_count_ = dyn->d_un.d_val / sizeof(ELFW(Rel));
             break;

           case DT_RELENT:
             if (dyn->d_un.d_val != sizeof(ELFW(Rel))) {
              // DL_ERR("invalid DT_RELENT: %z#include <ma>d", static_cast<size_t>(dyn->d_un.d_val));
               return false;
             }
             break;

           case DT_ANDROID_REL:
             android_relocs_ = reinterpret_cast<uint8_t*>(this->bias_addr_ + dyn->d_un.d_ptr);
             break;

           case DT_ANDROID_RELSZ:
             android_relocs_size_ = dyn->d_un.d_val;
             break;

           case DT_ANDROID_RELA:
            // DL_ERR("unsupported DT_ANDROID_RELA in \"%s\"", get_realpath());
             return false;

           case DT_ANDROID_RELASZ:
           //  DL_ERR("unsupported DT_ANDROID_RELASZ in \"%s\"", get_realpath());
             return false;

           // "Indicates that all RELATIVE relocations have been concatenated together,
           // and specifies the RELATIVE relocation count."
           //
           // TODO: Spec also mentions that this can be used to optimize relocation process;
           // Not currently used by bionic linker - ignored.
          // case DT_RELCOUNT:
          //   break;

           case DT_RELA:
            // DL_ERR("unsupported DT_RELA in \"%s\"", get_realpath());
             return false;

           case DT_RELASZ:
            // DL_ERR("unsupported DT_RELASZ in \"%s\"", get_realpath());
             return false;
#endif
      case DT_JMPREL:
#if defined(USE_RELA)
        plt_rela_ = reinterpret_cast<ELFW(Rela)*>(this->bias_addr_ + dyn->d_un.d_ptr);
#else
        plt_rel_ = reinterpret_cast<ELFW(Rel)*>(this->bias_addr_ + dyn->d_un.d_ptr);
#endif
        break;

      case DT_PLTRELSZ:
#if defined(USE_RELA)
        plt_rela_count_ = dyn->d_un.d_val / sizeof(ELFW(Rela));
#else
        plt_rel_count_ = dyn->d_un.d_val / sizeof(ELFW(Rel));
#endif
        break;

      case DT_HASH:
        {
          this->nbucket_ = reinterpret_cast<uint32_t*>(this->bias_addr_ + dyn->d_un.d_ptr)[0];
          this->nchain_ = reinterpret_cast<uint32_t*>(this->bias_addr_ + dyn->d_un.d_ptr)[1];
          this->bucket_ = reinterpret_cast<uint32_t*>(this->bias_addr_ + dyn->d_un.d_ptr + 8);
          this->chain_ = reinterpret_cast<uint32_t*>(this->bias_addr_ + dyn->d_un.d_ptr + 8 + nbucket_ * 4);
         // printf("nbucket: %d, nchain: %d, bucket: %p, chain:%p\n", this->nbucket_, this->nchain_, this->bucket_, this->chain_);
        //  printf("this is elf hash!\n");
          break;
        }
      case DT_GNU_HASH:
        {
          gnu_nbucket_ = reinterpret_cast<uint32_t*>(this->bias_addr_ + dyn->d_un.d_ptr)[0];
          // skip symndx
          /// 不要省略
          gnu_symndx_ = reinterpret_cast<uint32_t*>(this->bias_addr_ + dyn->d_un.d_ptr)[1];
          gnu_maskwords_ = reinterpret_cast<uint32_t*>(this->bias_addr_ + dyn->d_un.d_ptr)[2];
          gnu_shift2_ = reinterpret_cast<uint32_t*>(this->bias_addr_ + dyn->d_un.d_ptr)[3];

          gnu_bloom_filter_ = reinterpret_cast<ELFW(Addr)*>(this->bias_addr_ + dyn->d_un.d_ptr + 16);
          gnu_bucket_ = reinterpret_cast<uint32_t*>(gnu_bloom_filter_ + gnu_maskwords_);
          // amend chain for symndx = header[1]
          gnu_chain_ = gnu_bucket_ + gnu_nbucket_ -
              reinterpret_cast<uint32_t*>(this->bias_addr_ + dyn->d_un.d_ptr)[1];

          if (!powerof2(gnu_maskwords_)) {
          //  LOGV("invalid maskwords for gnu_hash = 0x%x, in \"%s\" expecting power to two",
         //       gnu_maskwords_, module_name_.c_str());
            return false;
          }
         //  LOGV("gnu_nbucket_: %d, gnu_maskwords_: %d, gnu_shift2_: %p, gnu_chain_:%p\n", gnu_nbucket_, gnu_maskwords_,
         //       gnu_shift2_, gnu_chain_);
          this->gnu_maskwords_ -= 1;
          this->is_has_gnu_hash_ = true;
         // printf("this is gnu hash!\n");
          break;
        }
    }
  }
  return true;
}

uint32_t GodinHook::HookModule::gnuHash(const char *s)
{
  uint32_t h = 5381;

  for (unsigned char c = *s; c != '\0'; c = *++s)
      h = h * 33 + c;

  return h;
}

uint32_t GodinHook::HookModule::elfHash(const char *name_)
{
    const uint8_t* name = reinterpret_cast<const uint8_t*>(name_);
    uint32_t h = 0, g;

    while (*name) {
      h = (h << 4) + *name++;
      g = h & 0xf0000000;
      h ^= g;
      h ^= g >> 24;
    }
    return h;
}

///  很邪门 gnuLookup 找不到啊，按道理说不应该找不到，但确实找不到
///  没辙了，只能使用最笨的办法了
bool GodinHook::HookModule::externalLookup(char const* symbol, ELFW(Sym) **sym, int *symidx)
{
  for(int i=0;i<this->gnu_symndx_;++i){
    if(0 == strcmp(reinterpret_cast<const char*>(this->strtab_+this->sym_ptr_[i].st_name),symbol)){
      *sym = this->sym_ptr_ + i;
      *symidx = i;
     // LOGV("-------->find %s",symbol);
      return true;
    }
  }
  return false;
}



/// 参考AOSP6.0/bionic/linker/linker.cpp
/// 适当删减
bool GodinHook::HookModule::elfLookUp(char const* symbol, ELFW(Sym) **sym, int *symbol_index)
{
  uint32_t hash = elfHash(symbol);
  *sym = NULL;
 // printf("SEARCH %s in %s@%p h=%x(elf) %zd\n",
        //     symbol, this->module_name_.c_str(),
        //     reinterpret_cast<void*>(this->base_addr_), hash, hash % nbucket_);



  for (uint32_t n = bucket_[hash % nbucket_]; n != 0; n = chain_[n]) {
    ELFW(Sym)* s = this->sym_ptr_ + n;

    if (strcmp(this->strtab_+s->st_name, symbol) == 0) {
   //   printf("FOUND %s in %s (%p) %zd\n",
   //             symbol, this->module_name_.c_str(),
   //              reinterpret_cast<void*>(s->st_value),
   //              static_cast<size_t>(s->st_size));
      *symbol_index = n;
      *sym = s;
      return true;
    }
  }

//  printf("NOT FOUND %s in %s@%p %x %zd\n",
//             symbol, this->module_name_.c_str(),
//             reinterpret_cast<void*>(this->base_addr_), hash, hash % nbucket_);

  *symbol_index = 0;
  return false;
}

/// 参考AOSP6.0/bionic/linker/linker.cpp
/// 适当删减
bool GodinHook::HookModule::gnuLookup(char const* symbol, ELFW(Sym) **sym, int *symbol_index)
{

    uint_fast32_t hash = gnuHash(symbol);
    uint32_t h2 = hash >> gnu_shift2_;
    uint32_t bloom_mask_bits = sizeof(ELFW(Addr))*8;
    uint32_t word_num = (hash / bloom_mask_bits) & gnu_maskwords_;
    ELFW(Addr) bloom_word = gnu_bloom_filter_[word_num];
    *symbol_index = 0;
    *sym = NULL;

//    LOGV("SEARCH %s in %s@%p (gnu)\n",
//        symbol, this->module_name_.c_str(), reinterpret_cast<void*>(this->base_addr_));

//    LOGV("n= %d (gnu)\n",
//       this->gnu_bucket_[hash % this->gnu_nbucket_]);

    if ((1 & (bloom_word >> (hash % bloom_mask_bits)) & (bloom_word >> (h2 % bloom_mask_bits))) == 0) {
//      LOGV("NOT FOUND %s in %s@%p\n",
//          symbol, this->module_name_.c_str(), reinterpret_cast<void*>(this->base_addr_));

      return false;
    }

    uint32_t n = this->gnu_bucket_[hash % this->gnu_nbucket_];

    if (n == 0) {
//      LOGV("NOT FOUND %s in %s@%p\n",
//          symbol, this->module_name_.c_str(), reinterpret_cast<void*>(this->base_addr_));
      return false;
    }


    do {
      ELFW(Sym)* s = this->sym_ptr_ + n;
      /// s->name 是索引值
      if (((gnu_chain_[n] ^ hash) >> 1) == 0 &&
          strcmp(this->strtab_ + s->st_name, symbol) == 0){
//            LOGV("FOUND %s in %s (%p) %zd\n",
//                   symbol, this->module_name_.c_str(), reinterpret_cast<void*>(s->st_value),
//                   static_cast<size_t>(s->st_size));
            *symbol_index = n;
            *sym = s;
            return true;
      }
    } while ((gnu_chain_[n++] & 1) == 0);

//    LOGV("NOT FOUND %s in %s@%p\n",
//        symbol, this->module_name_.c_str(), reinterpret_cast<void*>(this->base_addr_));

    return false;
}

bool GodinHook::HookModule::findSymbolByName(const char *symbol, ELFW(Sym) **sym, int *symidx)
{
  /// 首先判断字符串表和符号表是否存在
  if(this->strtab_ == NULL || this->sym_ptr_ == NULL)
    return false;
  /// 如果gnu hash存在，则以此为基准查找
  /// 否则，以elf hash为基准查找
  bool ret = (this->is_has_gnu_hash_) ? gnuLookup(symbol,sym,symidx):elfLookUp(symbol,sym,symidx);
  if(ret)
    return true;
  else{
    if(this->is_has_gnu_hash_)
      return externalLookup(symbol,sym,symidx);
  }
}

/// TODO: 添加对DT_ANDROID_* 类型的支持
bool GodinHook::HookModule::hook(const char *originalFunc, void *newFunc, void **backFunc)
{
     /// 检查参数
    assert(originalFunc);
    assert(newFunc);
    assert(backFunc);


    /// 查找originalFunc所在的symbol table entry 以及索引
    ELFW(Sym) *sym = NULL;
    int symbol_index = 0;
    if(!(this->findSymbolByName(originalFunc,&sym,&symbol_index)) || NULL == sym){

      LOGV("cant find symbol:%s",originalFunc);
      return false;
    }


    /// 暂且不考虑 DT_ANDROID_* 类型
#if defined(USE_RELA)
    /// 此时肯定是64位
    for(uint32_t i=0;i < this->plt_rela_count_;i++){
      ELFW(Xword)  r_info = 0;
      ELFW(Addr)  r_offset = 0;
      ELFW(Rela) *rela = this->plt_rela_ + i;
      r_info = rela->r_info;
      r_offset = rela->r_offset;
      if((symbol_index == ELF64_R_SYM(r_info)) && (R_GENERIC_JUMP_SLOT == ELF64_R_TYPE(r_info)))
      {
      //  printf("-------hook 64 it ---------\n");
        void * originalFuncAddr = reinterpret_cast<void *>(this->bias_addr_ + r_offset);
      //  printf("--originalFuncAddr = %p\n",originalFuncAddr);
     //   printf("--rel = %p\n",*(void**)originalFuncAddr);
        this->hookFunction(originalFuncAddr,newFunc,backFunc);
      }

    }

    for(uint32_t i=0;i < this->dyn_rela_count_;i++){
      ELFW(Xword)  r_info = 0;
      ELFW(Addr)  r_offset = 0;
      ELFW(Rela) *rela = this->dyn_rela_ + i ;
      r_info = rela->r_info;
      r_offset = rela->r_offset;
      if((symbol_index == ELF64_R_SYM(r_info)) && ((R_GENERIC_GLOB_DAT == ELF64_R_TYPE(r_info)) || (R_GENERIC_ABS == ELF64_R_TYPE(r_info)))){
      //  printf("-------hook 64 rel.dyn  ---------\n");
        void * originalFuncAddr = reinterpret_cast<void *>(this->bias_addr_ + r_offset);
      //  printf("--originalFuncAddr = %p\n",originalFuncAddr);
      //  printf("--rel = %p\n",*(void**)originalFuncAddr);
        this->hookFunction(originalFuncAddr,newFunc,backFunc);
      }
    }

#else
      /// 此时肯定是32位
      for(uint32_t i=0;i < this->plt_rel_count_;i++){
        ELFW(Word)  r_info = 0;
        ELFW(Addr)  r_offset = 0;
        ELFW(Rel) *rel = this->plt_rel_ + i;
        r_info = rel->r_info;
        r_offset = rel->r_offset;

        if((symbol_index == ELF32_R_SYM(r_info)) && (R_GENERIC_JUMP_SLOT == ELF32_R_TYPE(r_info)))
        {
       //   printf("-------hook it ---------\n");
          void * originalFuncAddr = reinterpret_cast<void *>(this->bias_addr_ + r_offset);
       //   printf("--originalFuncAddr = %p\n",originalFuncAddr);
       //   printf("--rel = %p\n",*(void**)originalFuncAddr);
          this->hookFunction(originalFuncAddr,newFunc,backFunc);
        }

      }

      for(uint32_t i=0;i < this->dyn_rel_count_;i++){
        ELFW(Word)  r_info = 0;
        ELFW(Word)  r_offset = 0;
        ELFW(Rel) *rel = this->dyn_rel_ + i ;
        r_info = rel->r_info;
        r_offset = rel->r_offset;
        if((symbol_index == ELF32_R_SYM(r_info)) && ((R_GENERIC_GLOB_DAT == ELF32_R_TYPE(r_info)) || (R_GENERIC_ABS == ELF32_R_TYPE(r_info)))){
      //  printf("-------hook rel.dyn  ---------\n");
        void * originalFuncAddr = reinterpret_cast<void *>(this->bias_addr_ + r_offset);
     //   printf("--originalFuncAddr = %p\n",originalFuncAddr);
      //  printf("--rel = %p\n",*(void**)originalFuncAddr);
        this->hookFunction(originalFuncAddr,newFunc,backFunc);
        }
      }
#endif

      return true;
}

bool GodinHook::HookModule::unhook(const char *originalFunc, void *backFunc)
{
  /// 检查参数
 assert(originalFunc);
 assert(backFunc);
 /// 查找originalFunc所在的symbol table entry 以及索引
 ELFW(Sym) *sym = NULL;
 int symbol_index = 0;
 if(!(this->findSymbolByName(originalFunc,&sym,&symbol_index)) || NULL == sym){
   return false;
 }
#if defined(USE_RELA)
    /// 此时肯定是64位
    for(uint32_t i=0;i < this->plt_rela_count_;i++){
      ELFW(Xword)  r_info = 0;
      ELFW(Addr)  r_offset = 0;
      ELFW(Rela) *rela = this->plt_rela_ + i;
      r_info = rela->r_info;
      r_offset = rela->r_offset;
      if((symbol_index == ELF64_R_SYM(r_info)) && (R_GENERIC_JUMP_SLOT == ELF64_R_TYPE(r_info)))
      {
      //  printf("-------unhook 64 it ---------\n");
        void * originalFuncAddr = reinterpret_cast<void *>(this->bias_addr_ + r_offset);
        this->unhookFunction(originalFuncAddr,backFunc);
      }

    }

    for(uint32_t i=0;i < this->dyn_rela_count_;i++){
      ELFW(Xword)  r_info = 0;
      ELFW(Addr)  r_offset = 0;
      ELFW(Rela) *rela = this->dyn_rela_ + i ;
      r_info = rela->r_info;
      r_offset = rela->r_offset;
      if((symbol_index == ELF64_R_SYM(r_info)) && ((R_GENERIC_GLOB_DAT == ELF64_R_TYPE(r_info)) || (R_GENERIC_ABS == ELF64_R_TYPE(r_info)))){
      //  printf("-------hook 64 rel.dyn  ---------\n");
        void * originalFuncAddr = reinterpret_cast<void *>(this->bias_addr_ + r_offset);
        this->unhookFunction(originalFuncAddr,backFunc);
      }
    }

#else
 /// 此时肯定是32位
 for(uint32_t i=0;i < this->plt_rel_count_;i++){
   ELFW(Word)  r_info = 0;
   ELFW(Addr)  r_offset = 0;
   ELFW(Rel) *rel = this->plt_rel_ + i;
   r_info = rel->r_info;
   r_offset = rel->r_offset;

   if((symbol_index == ELF32_R_SYM(r_info)) && (R_GENERIC_JUMP_SLOT == ELF32_R_TYPE(r_info)))
   {
     //printf("-------unhook it ---------\n");
     void * originalFuncAddr = reinterpret_cast<void *>(this->bias_addr_ + r_offset);
     this->unhookFunction(originalFuncAddr,backFunc);
   }

 }

 for(uint32_t i=0;i < this->dyn_rel_count_;i++){
   ELFW(Word)  r_info = 0;
   ELFW(Addr)  r_offset = 0;
   ELFW(Rel) *rel = this->dyn_rel_ + i ;
   r_info = rel->r_info;
   r_offset = rel->r_offset;
   if((symbol_index == ELF32_R_SYM(r_info)) && ((R_GENERIC_GLOB_DAT == ELF32_R_TYPE(r_info)) || (R_GENERIC_ABS == ELF32_R_TYPE(r_info)))){
  // printf("-------unhook rel.dyn  ---------\n");
   void * originalFuncAddr = reinterpret_cast<void *>(this->bias_addr_ + r_offset);
   this->unhookFunction(originalFuncAddr,backFunc);
   }
 }

#endif
 return true;
}

static int clear_cache(void* addr, size_t len)
{
    void *end = (uint8_t *)addr + len;
    return syscall(0xf0002, addr, end);
}

bool GodinHook::HookModule::hookFunction(void *originalFunc, void *newFunc, void **backFunc)
{
  /// 检查参数
  assert(originalFunc);
  assert(newFunc);
  assert(backFunc);

  /// 新函数与原函数不能是同一个函数
  if((*(void**)originalFunc) == newFunc)
      return false;

 /// 备份原方法
    *backFunc = *(void **)originalFunc;
    LOGTEST("backFunc %x", *backFunc);
    LOGTEST("orginalFunc %x", *(void **)originalFunc);
    //*backFuncAddr = *(void **)originalFunc;

  /// 去保护(SEAndroid或许对此有限制)
  if(!Utils::unProtectMemory(originalFunc,sizeof(void*))){

    perror("unprotect Memory failed!!!\n");
    return false;
  }
 // printf("--newFunc = %p\n",newFunc);
  *(void**)originalFunc = newFunc;
  LOGTEST("orginalFunc %x", *(void **)originalFunc);
  LOGTEST("newFunc %x", newFunc);
 // printf("--originalFuncAddr = %p\n",*(void**)originalFunc);
 // Utils::protectMemory(originalFunc,sizeof(void *));

  /// 刷新指令缓存
  //clear_cache(originalFunc,sizeof(void*));
  clear_cache(originalFunc,sizeof(void*));

}

bool GodinHook::HookModule::unhookFunction(void *originalFunc, void *backFunc)
{
  /// 检查参数
  assert(originalFunc);
  assert(backFunc);

  /// 一样就不需要还原了
  if((*(void**)originalFunc) == backFunc)
      return true;

  /// 去保护(SEAndroid或许对此有限制)
  if(!Utils::unProtectMemory(originalFunc,sizeof(void *))){

    perror("unprotect Memory failed!!!\n");
    return false;
  }
  *(void**)originalFunc = backFunc;
 // Utils::protectMemory(originalFunc,sizeof(void *));

  /// 刷新指令缓存
  clear_cache(originalFunc,sizeof(void*));

}

bool GodinHook::HookModule::isSymbolAllreadyHooked(string symbol)
{
  HookSymbol::iterator it = this->hook_symbol_set_.find(symbol);
  if(it != hook_symbol_set_.end())
    return true;
  else
    return false;
}

void GodinHook::HookModule::addHookedSymbol(string symbol)
{
  if(isSymbolAllreadyHooked(symbol))
    return;
  else
    this->hook_symbol_set_.insert(symbol);
}

void GodinHook::HookModule::removeHookedSymbol(string symbol)
{
  if(isSymbolAllreadyHooked(symbol)){
    this->hook_symbol_set_.erase(symbol);
  }
}




