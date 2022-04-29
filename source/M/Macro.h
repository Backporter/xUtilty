#pragma once

// contains macros for overall usage

#include <stdio.h>
#include <stdint.h>
#include "../O/OrbisMessageHandler.h"

// print the current code pos into the kernel log(debug)
#ifndef PRINT_POS
#define PRINT_POS OrbisMessageHandler::KernelPrintOut("%s %d", __FILE__, __LINE__);
#endif

// print the current code pos into the kernel log and return(debug)
#ifndef PRINT_POS_RETURN 
#define PRINT_POS_RETURN OrbisMessageHandler::KernelPrintOut("%s %d %s", __FILE__, __LINE__, THINKING); return;
#endif

// print the current code pos + custom fmt + string into the kernel log 
#ifndef PRINT_FMT
#define PRINT_FMT(fmt, ...) OrbisMessageHandler::KernelPrintOut("%s %d %s " fmt, __FILE__, __LINE__, THINKING, __VA_ARGS__);
#endif

// print the current code pos + custom fmt + string into the kernel log and return T
#ifndef PRINT_FMT_RETURN_T
#define PRINT_FMT_RETURN_T(T, fmt, ...) OrbisMessageHandler::KernelPrintOut("%s %d %s "  fmt, __FILE__, __LINE__, THINKING, __VA_ARGS__); return T;
#endif

// print the current code pos + custom fmt + string into the kernel log and return(debug)
#ifndef PRINT_RETURN
#define PRINT_RETURN(fmt, ...) OrbisMessageHandler::KernelPrintOut("%s %d %s "  fmt, __FILE__, __LINE__, THINKING, __VA_ARGS__); return;
#endif

// print the current code pos + custom string into the kernel log and return(debug)
#ifndef PRINT_STRING_RETURN 
#define PRINT_STRING_RETURN(string)  OrbisMessageHandler::KernelPrintOut("%s %d %s", __FILE__, __LINE__, string); return;
#endif

// print the current code pos + custom string into the kernel log(debug)
#ifndef PRINT_STRING
#define PRINT_STRING(string)  OrbisMessageHandler::KernelPrintOut("%s %d %s", __FILE__, __LINE__, string);
#endif

// Adds app base + RTTI pos offset(8) and defaces it,
// EXAMPLE:
// 0x1D18000 is the offset to the title, so we add 8 to get the offset directly to pointer, than deface said pointer to get the pointer to the RTTI info
#ifndef ADD_BASE_RTTI
#define ADD_BASE_RTTI(src) *(const void**)(((uintptr_t)src) + OrbisRelocationManager::OrbisRelocationManager::ApplicationBaseAddress + 8)
#endif

// adds the app base address to any pointer.
#ifndef ADD_BASE_POINTER
#define ADD_BASE_POINTER(src) (void*)(((uintptr_t)src) + OrbisRelocationManager::OrbisRelocationManager::ApplicationBaseAddress)
#endif

// 
#ifndef BOOL_TYPE_AS_STRING
#define BOOL_TYPE_AS_STRING(b) (b ? "TRUE" : "FALSE")
#endif

// 
#ifndef FN_AS_RAW_ADDRESS
#define FN_AS_RAW_ADDRESS(FN) ((uintptr_t)FN)
#endif

// standard exporting, keeps the compiller from mangling the symbols
#if defined(__ORBIS__)
#ifndef EXPORT
#define EXPORT extern "C" __declspec (dllexport)
#endif
#elif defined(__OPENORBIS__)
#ifndef EXPORT
#define EXPORT extern "C"
#endif
#endif

// same as the previous, exepct this is specific to OOSDK
#ifndef EXPORT_C
#define EXPORT_C extern "C"
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