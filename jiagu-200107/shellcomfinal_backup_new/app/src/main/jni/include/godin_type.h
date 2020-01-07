#ifndef GODIN_TYPE_H
#define GODIN_TYPE_H

#include <elf.h>
#include <unistd.h>

/// 参考AOSP linker.h
/// 主要是为了支持32/64平台
#if defined(__LP64__)
#define ELFW(what) Elf64_ ## what
#else
#define ELFW(what) Elf32_ ## what
#endif


#endif // GODIN_TYPE_H
