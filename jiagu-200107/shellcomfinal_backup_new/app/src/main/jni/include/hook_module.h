/*===============================================================
*   Copyright (C) 2016 All rights reserved.
*
*   文件名称：hook_module.h
*   创 建 者：genglei.cuan@godinsec.com
*   创建日期：2016年05月16日
*   描    述：对ELF文件的操作
*
*   更新日志：
*
================================================================*/

#ifndef HOOKMODULE_H
#define HOOKMODULE_H

#include "godin_type.h"
#include <string>
#include <set>

using namespace std;
// Android uses RELA for aarch64 and x86_64. mips64 still uses REL.
#if defined(__aarch64__) || defined(__x86_64__)
#define USE_RELA 1
#endif

namespace GodinHook {

  /**
   * @brief The HookModule class
   * 对调用hook函数的elf文件的抽象
   */
  class HookModule{


  public:
    HookModule(string name,size_t base_addr):module_name_(name),base_addr_(base_addr),
                                             bias_addr_(0),isRegistered(false),ehdr_(NULL),
                                             phdr_(NULL),shdr_(NULL),dyn_ptr_(NULL),dyn_size_(0),
                                             sym_ptr_(NULL),sym_size_(0),
                                             dyn_rela_(NULL),dyn_rela_count_(0),dyn_rel_(NULL),dyn_rel_count_(0),is_has_gnu_hash_(false){}

   bool initModule();

   ELFW(Addr) getBias(){ return this->bias_addr_;}

   /**
    * @brief findSymbolByName
    * 查找symbol对应的Symbol table entry地址和
    * 其在Symbol table中的索引
    * @param symbol(input)
    * 待查找的符号名称
    * @param sym(out)
    * symbol对应的Symbol table entry地址
    * @param symidx(out)
    * 在Symbol table中的索引
    * @return
    * 返回true，表示查找成功；
    * 返回false,表示查找失败；
    */
   bool findSymbolByName(const char *symbol,  ELFW(Sym) **sym, int *symidx);

   bool hook(const char *originalFunc, void *newFunc, void **backFunc);

   bool unhook(const char *originalFunc, void *backFunc);


   /**
    * @brief isSymbolAllreadyHooked
    * 符号对应的方法是否已经被hook
    * @param symbol
    * 方法名字
    * @return
    * true,已经被hook；
    * false,没有被hook;
    */
   bool isSymbolAllreadyHooked(string symbol);

   /**
    * @brief addHookedSymbol
    * 将已被hook的symbol加入set中
    * @param symbol
    * 已被hook的sybol
    */
   void addHookedSymbol(string symbol);
   void removeHookedSymbol(string symbol);
   string getModuleName(){return this->module_name_;}
   bool getIsRegistered() const{return this->isRegistered;}
   void setIsRegistered(bool value){this->isRegistered = true;}

  private:

   /**
    * @brief gnuHash
    * Android 6.0之后采用gnuhash
    * @param s
    * @return
    * 返回字符串对应的gnu hash 值
    */
   static uint_fast32_t gnuHash (const char *name);

   /**
    * @brief elfHash
    * Android 6.0之前采用elf hash
    * @param name
    * @return
    * 返回字符串对应的elf hash 值
    */
   static uint32_t elfHash(const char * name);

   /**
    * @brief gnuLookup
    * 利用gnu hash 查找symbol对应的Symbol table entry地址和
    * 其在Symbol table中的索引
    * @param symbol(input)
    * 待查找的符号名称
    * @param sym(out)
    * symbol对应的Symbol table entry地址
    * @param symidx(out)
    * 在Symbol table中的索引
    * @return
    * 返回true，表示查找成功；
    * 返回false,表示查找失败；
    */
   bool gnuLookup(char const* symbol, ELFW(Sym) **sym, int *symidx);
   /**
    * @brief elfLookUp
    * 利用elf hash 查找symbol对应的Symbol table entry地址和
    * 其在Symbol table中的索引
    * @param symbol(input)
    * 待查找的符号名称
    * @param sym(out)
    * symbol对应的Symbol table entry地址
    * @param symidx(out)
    * 在Symbol table中的索引
    * @return
    * 返回true，表示查找成功；
    * 返回false,表示查找失败；
    */
   bool elfLookUp(char const* symbol, ELFW(Sym) **sym, int *symidx);


   bool hookFunction(void* originalFunc, void *newFunc, void **backFunc);

   bool unhookFunction(void* originalFunc, void *backFunc);

  protected:

      typedef set<string> HookSymbol;
      HookSymbol hook_symbol_set_;

      ELFW(Addr)      base_addr_;    /// < elf 模块在内存中的基地址
      ELFW(Addr)      bias_addr_;    /// < LOAD段实际加载的地址与通过elf信息计算出来的地址的差值
      string          module_name_;  /// < elf 模块的名字（绝对路径+名字）
      bool            isRegistered;   /// < 是否注册
      bool            is_has_gnu_hash_; /// < 6.0之后采用此hash



      /// 针对__LP64__
      ELFW(Rela)* plt_rela_;      /// < elf 文件 relocation(rela) headers 地址
      size_t plt_rela_count_;     /// < relocation(rela) headers entry个数
      ELFW(Rela)* dyn_rela_;
      size_t dyn_rela_count_;


      /// 针对32
      ELFW(Rel)* plt_rel_;        /// < elf 文件 relocation(rela) headers 地址
      size_t plt_rel_count_;      /// < relocation(rel) headers entry个数
      ELFW(Rel)* dyn_rel_;
      size_t dyn_rel_count_;

      uint8_t* android_relocs_;
      size_t android_relocs_size_;
  protected:

      ELFW(Ehdr)  *ehdr_;      /// < elf 文件 headers 地址
      ELFW(Phdr)  *phdr_;      /// < elf 文件 program headers 地址
      ELFW(Shdr)  *shdr_;      /// < elf 文件 section headers 地址

      ELFW(Dyn)   *dyn_ptr_;   /// < elf 文件 dynamic headers
      ELFW(Word)   dyn_size_;   /// < dynamic headers 大小

      ELFW(Sym)    *sym_ptr_;  /// < elf 文件 symbol headers 地址
      ELFW(Word)   sym_size_;  /// < symbol headers 大小



      const char* strtab_; /// < elf 文件字符串表地址
  protected:
      //for elf hash
       size_t nbucket_;
       size_t nchain_;
       uint32_t* bucket_;
       uint32_t* chain_;

      //for gnu hash
       size_t gnu_nbucket_;
       uint32_t* gnu_bucket_;
       uint32_t* gnu_chain_;
       uint32_t gnu_maskwords_;
       uint32_t gnu_shift2_;
       ELFW(Addr)* gnu_bloom_filter_;

  };

}
#endif // HOOKMODULE_H
