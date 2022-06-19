#include <assert.h>
#include <stdio.h>
#include <stddef.h>
#include <iostream>

#include "../include/Trampoline.h"
#include "../include/RelocationManager.h"
#include "../include/Macro.h"

#if defined(__ORBIS__)
#include <kernel.h>
#elif defined(__OPENORBIS__)
#include <orbis/libkernel.h>
#elif defined(__SWITCH__) || defined(PLATFORM_NX)
#include <switch.h>
enum MemoryPermission
{
	MemoryPermission_None = 0x0,  //!<  Access to memory is prohibited.
	MemoryPermission_ReadOnly = 0x1,  //!<  Memory is read-only.
	MemoryPermission_WriteOnly = 0x2,  //!<  Memory is write-only.
	MemoryPermission_ReadWrite = 0x3,  //!<  Memory can be read from and written to.
	MemoryPermission_ReadExecute = 0x5,  //!<  Memory is available to read.
};

extern int32_t AllocateMemoryBlock(uintptr_t*, size_t);
extern void SetMemoryPermission(uintptr_t, size_t, MemoryPermission);
extern void FreeMemoryBlock(uintptr_t, size_t);
#endif


namespace Trampoline
{
	Trampoline::Trampoline():
		SystemAllocatedMemory(0), SystemAllocatedLength(0), SystemAllocatedOffset(0), AllocSysAllocLen(0), AllocSysAllocAddr(0) {}

	Trampoline::~Trampoline()
	{
		SystemDeallocate();
	}

	bool Trampoline::SystemAllocate(size_t TrampolineSize)
	{
		int32_t ret = 0;

#if defined (__ORBIS__) || defined(__OPENORBIS__)
		if ((ret = sceKernelAllocateDirectMemory(0, sceKernelGetDirectMemorySize(), TrampolineSize, TrampolineSize, 0, &SystemAllocatedOffset)) != 0)
		{
			MessageHandler::Notify("Failed to allocate Direct Memory! sceKernelAllocateDirectMemory failed 0x%08X", ret);
			return false;
		}
#endif
		SystemAllocatedMemory = reinterpret_cast<void*>(RelocationManager::RelocationManager::ApplicationBaseAddress == 0 ? 0x400000 : RelocationManager::RelocationManager::ApplicationBaseAddress);

		// nn::os::AllocateMemoryBlock(SystemAllocatedMemory, TrampolineSize);
#if defined (__ORBIS__) || defined(__OPENORBIS__)
		if ((ret = sceKernelMapDirectMemory(&SystemAllocatedMemory, TrampolineSize, 0x02, 0, SystemAllocatedOffset, TrampolineSize)) != 0)
		{
			MessageHandler::Notify("Failed To Map Direct Memory! sceKernelMapDirectMemory returned %lx", ret);
			return false;
		}
#elif defined(__SWITCH__) || defined(PLATFORM_NX)

#endif

#if defined (__ORBIS__) || defined(__OPENORBIS__)
		if ((ret = sceKernelMprotect(SystemAllocatedMemory, TrampolineSize, VM_PROT_ALL)) != 0)
		{
			MessageHandler::Notify("Failed to change protection![this is fatal]... sceKernelMprotect returned %lx", ret);
			return false;
		}
#endif
		SystemAllocatedLength = TrampolineSize;
		AllocSysAllocLen = 0;
		return SystemAllocatedMemory != NULL && ret == 0;
	}

	void Trampoline::SystemDeallocate()
	{
		if (SystemAllocatedMemory)
		{
#if defined (__ORBIS__) || defined(__OPENORBIS__)
			sceKernelReleaseDirectMemory(SystemAllocatedOffset, SystemAllocatedLength);
#elif defined(__SWITCH_) || defined(PLATFORM_NX)
			FreeMemoryBlock(SystemAllocatedMemory, SystemAllocatedLength);
#endif
			SystemAllocatedMemory = NULL;
		}
	}

	void* Trampoline::StartTake()
	{
		assert(SystemAllocatedMemory);
		assert(!AllocSysAllocAddr);

		AllocSysAllocAddr = ((uint8_t *)SystemAllocatedMemory) + AllocSysAllocLen;

		return AllocSysAllocAddr;
	}

	void  Trampoline::RestoreAllocated(const void* allocated)
	{
		assert(SystemAllocatedMemory);
		assert(AllocSysAllocAddr);

		size_t len = uintptr_t(allocated) - uintptr_t(AllocSysAllocAddr);
		assert(len <= Remain());

		AllocSysAllocLen += len;
		AllocSysAllocAddr = nullptr;
	}

	void* Trampoline::Take(size_t size)
	{
		assert(SystemAllocatedMemory);

		void * result = nullptr;

		if (size <= Remain())
		{
			result = ((uint8_t *)SystemAllocatedMemory) + AllocSysAllocLen;
			AllocSysAllocLen += size;
		}

		return result;
	}

#if defined (__ORBIS__) || defined(__OPENORBIS__)
	void* Trampoline::allocate(Xbyak::CodeGenerator& code)
	{
		size_t length = code.getSize();
		void* codebuf = Take(length);
		memcpy(codebuf, code.getCode(), length);
		return codebuf;
	}
#elif defined(__SWITCH__) || defined(PLATFORM_NX)
	void* Trampoline::allocate(Xbyak_aarch64::CodeGenerator& code)
	{
		size_t length = code.getSize();
		void* codebuf = Take(length);
		memcpy(codebuf, code.getCode(), length);
		return codebuf;
	}
#endif

	Trampoline g_Trampoline;
}