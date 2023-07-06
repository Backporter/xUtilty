#pragma once

#include <stdint.h>

#if __ORBIS__
#include <machine\endian.h>
#elif __OPENORBIS__
#include <byteswap.h>
#endif

#ifndef THINKING
#define THINKING "🤔"
#endif

#ifndef MACRO_SWAP 
#define MACRO_SWAP(TEST, b, c) ((TEST ? b : c))
#endif

#ifndef DEBUG_BUILD_MACRO_SWITCH
#define DEBUG_BUILD_MACRO_SWITCH(a, b)  MACRO_SWAP(_DEBUG, a, b)
#endif

#ifndef MIN3
#define MIN3(x,y,z)  ((y) <= (z) ? ((x) <= (y) ? (x) : (y)) : ((x) <= (z) ? (x) : (z)))
#endif

#ifndef MAX3
#define MAX3(x,y,z)  ((y) >= (z) ? ((x) >= (y) ? (x) : (y)) : ((x) >= (z) ? (x) : (z)))
#endif

#ifndef BOOL_TYPE_AS_STRING
#define BOOL_TYPE_AS_STRING(b) (b ? "TRUE" : "FALSE")
#endif

// 
#ifndef FN_AS_RAW_ADDRESS
#define FN_AS_RAW_ADDRESS(FN) ((uintptr_t)FN)
#endif

#ifndef EXPORT_C
#define EXPORT_C extern "C"
#endif

#ifndef EXPORT
#if defined(__ORBIS__)
#define EXPORT extern "C" __declspec (dllexport)
#else
#define EXPORT EXPORT_C
#endif
#endif

#ifndef RW_MODE
#define MOD_RW 0000777
#endif

#ifndef R_MODE
#define R_MODE 0000555
#endif

#ifndef VM_PROT_NONE
#define	VM_PROT_NONE		0x00
#endif

#ifndef VM_PROT_READ
#define VM_PROT_READ		0x01
#endif

#ifndef VM_PROT_WRITE
#define VM_PROT_WRITE		0x02
#endif

#ifndef VM_PROT_EXECUTE
#define VM_PROT_EXECUTE		0x04
#endif

#ifndef VM_PROT_DEFAULT
#define VM_PROT_DEFAULT		(VM_PROT_READ | VM_PROT_WRITE)
#endif

#ifndef VM_PROT_ALL
#define VM_PROT_ALL			(VM_PROT_READ | VM_PROT_WRITE | VM_PROT_EXECUTE)
#endif

#ifndef O_RDONLY
#define	O_RDONLY	0x0000
#endif

#ifndef O_WRONLY
#define	O_WRONLY	0x0001
#endif

#ifndef O_RDWR
#define	O_RDWR		0x0002
#endif

#ifndef O_ACCMODE
#define	O_ACCMODE	0x0003
#endif


// https://stackoverflow.com/a/35261673
#ifndef PRINT_SIZEOF
#define PRINT_SIZEOF(T) char (*__kaboom)[sizeof(T)] = 1;
#endif

// https://stackoverflow.com/questions/69208830/how-can-i-print-the-offset-of-a-struct-member-at-compile-time
#ifndef PRINT_OFFSETOF
#define PRINT_OFFSETOF(A, B) char (*__daniel_kleinstein_is_cool)[sizeof(char[offsetof(A, B)])] = 1
#endif 

#ifndef PRINT_CPLUSPLUS
#define PRINT_CPLUSPLUS char (*XD)[sizeof(char[__cplusplus])] = 1
#endif 

#ifndef STRINGIFY
#define STRINGIFYEX(a) #a
#define STRINGIFY(a) STRINGIFYEX(a)
#endif

#ifndef COMBINE
#define COMBINE(str1, str2) (str1 str2)
#endif

#ifndef COMBINE2
#define COMBINE2(str1, str2) str1 str2
#endif

#ifndef NAKED
#define NAKED __attribute__((naked))
#endif

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

#ifndef FORCE_CRASH
#define FORCE_CRASH __asm volatile("int3");
#endif

#ifndef MAKE_CTOR
#define MAKE_CTOR __attribute__((constructor))
#endif

#ifndef __noinline
#define	__noinline	__attribute__ ((__noinline__))
#endif