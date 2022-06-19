#pragma once

// contains macros for overall usage

#include <stdio.h>
#include <stdint.h>

#include "MessageHandler.h"
#include "RelocationManager.h"

#ifndef AS
#define AS(OBJ, T) dynamic_cast<T>(OBJ);
#endif

#ifndef RW_MODE
#define MOD_RW 0000777
#endif
#ifndef R_MODE
#define R_MODE 0000555
#endif

// print the current code pos into the kernel log(debug)
#ifndef PRINT_POS
#define PRINT_POS MessageHandler::KernelPrintOut("%s %d", __FILE__, __LINE__);
#endif

// print the current code pos into the kernel log and return(debug)
#ifndef PRINT_POS_RETURN 
#define PRINT_POS_RETURN MessageHandler::KernelPrintOut("%s %d %s", __FILE__, __LINE__, THINKING); return;
#endif

// print the current code pos + custom fmt + string into the kernel log 
#ifndef PRINT_FMT
#define PRINT_FMT(fmt, ...) MessageHandler::KernelPrintOut("%s %d %s " fmt, __FILE__, __LINE__, THINKING, __VA_ARGS__);
#endif

// print the current code pos + custom fmt + string into the kernel log and return T
#ifndef PRINT_FMT_RETURN_T
#define PRINT_FMT_RETURN_T(T, fmt, ...) MessageHandler::KernelPrintOut("%s %d %s "  fmt, __FILE__, __LINE__, THINKING, __VA_ARGS__); return T;
#endif

// print the current code pos + custom fmt + string into the kernel log and return(debug)
#ifndef PRINT_RETURN
#define PRINT_RETURN(fmt, ...) MessageHandler::KernelPrintOut("%s %d %s "  fmt, __FILE__, __LINE__, THINKING, __VA_ARGS__); return;
#endif

// print the current code pos + custom string into the kernel log and return(debug)
#ifndef PRINT_STRING_RETURN 
#define PRINT_STRING_RETURN(string)  MessageHandler::KernelPrintOut("%s %d %s", __FILE__, __LINE__, string); return;
#endif

// print the current code pos + custom string into the kernel log(debug)
#ifndef PRINT_STRING
#define PRINT_STRING(string)  MessageHandler::KernelPrintOut("%s %d %s", __FILE__, __LINE__, string);
#endif

// Adds app base + RTTI pos offset(8) and defaces it,
// EXAMPLE:
// 0x1D18000 is the offset to the title, so we add 8 to get the offset directly to pointer, than deface said pointer to get the pointer to the RTTI info
#ifndef ADD_BASE_RTTI
#define ADD_BASE_RTTI(src) *(const void**)(((uintptr_t)src) + RelocationManager::RelocationManager::ApplicationBaseAddress + 8)
#endif

// adds the app base address to any pointer.
#ifndef ADD_BASE_POINTER
#define ADD_BASE_POINTER(src) (void*)(((uintptr_t)src) + RelocationManager::RelocationManager::ApplicationBaseAddress)
#endif

// 
#ifndef BOOL_TYPE_AS_STRING
#define BOOL_TYPE_AS_STRING(b) (b ? "TRUE" : "FALSE")
#endif

// 
#ifndef FN_AS_RAW_ADDRESS
#define FN_AS_RAW_ADDRESS(FN) ((uintptr_t)FN)
#endif

#ifndef EXPORT
#if defined(__ORBIS__)
#define EXPORT extern "C" __declspec (dllexport)
#else
#define EXPORT extern "C"
#endif
#endif

// same as the previous, exepct this is specific to OOSDK
#ifndef EXPORT_C
#define EXPORT_C extern "C"
#endif

#ifndef COMBINE
#define COMBINE(str1, str2) str1 str2
#endif

// memory flags
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
#define	O_RDONLY	0x0000		/* open for reading only */
#endif

#ifndef O_WRONLY
#define	O_WRONLY	0x0001		/* open for writing only */
#endif

#ifndef O_RDWR
#define	O_RDWR		0x0002		/* open for reading and writing */
#endif

#ifndef O_ACCMODE
#define	O_ACCMODE	0x0003		/* mask for above modes */
#endif

// https://stackoverflow.com/questions/69208830/how-can-i-print-the-offset-of-a-struct-member-at-compile-time
#ifndef PRINT_OFFSETOF
#define PRINT_OFFSETOF(A, B) char (*__daniel_kleinstein_is_cool)[sizeof(char[offsetof(A, B)])] = 1
#endif 

#ifndef STRINGIFY
#define STRINGIFYEX(a) #a
#define STRINGIFY(a) STRINGIFYEX(a)
#endif

#ifndef NAKED
#define NAKED __attribute__((naked))
#endif

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

#ifndef SHORT_TYPES
#define SHORT_TYPES
typedef uint8_t  BYTE;
typedef uint32_t UI32;
typedef uint64_t UI64;
typedef int8_t	 SBYTE;
typedef int32_t  I32;
typedef int64_t  I64;
#endif

#if defined(__ORBIS__) || defined(__OPENORBIS__)
#define ROOT "/app0/"
#elif defined(PLATFORM_NX) || defined(__SWITCH__)
#define ROOT "rom:/"
#endif