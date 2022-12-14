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
#endif

namespace Trampoline
{
	Trampoline::Trampoline() : SystemAllocatedMemory(0), SystemAllocatedLength(0), SystemAllocatedOffset(0), AllocSysAllocLen(0), AllocSysAllocAddr(0) {}

	Trampoline::~Trampoline() { SystemDeallocate(); }

	bool Trampoline::SystemAllocate(size_t TrampolineSize, size_t alignment)
	{
		int32_t ret = 0;

		if ((ret = sceKernelAllocateDirectMemory(0, sceKernelGetDirectMemorySize(), TrampolineSize, alignment, 0, &SystemAllocatedOffset)) != 0)
		{
			MessageHandler::Notify("Failed to allocate Direct Memory! sceKernelAllocateDirectMemory failed 0x%08X", ret);
			return false;
		}

		SystemAllocatedMemory = reinterpret_cast<void*>(RelocationManager::RelocationManager::ApplicationBaseAddress == 0 ? 0x400000 : RelocationManager::RelocationManager::ApplicationBaseAddress);

		if ((ret = sceKernelMapDirectMemory(&SystemAllocatedMemory, TrampolineSize, 0x02, 0, SystemAllocatedOffset, TrampolineSize)) != 0)
		{
			MessageHandler::Notify("Failed To Map Direct Memory! sceKernelMapDirectMemory returned %lx", ret);
			return false;
		}


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
			sceKernelReleaseDirectMemory(SystemAllocatedOffset, SystemAllocatedLength);
			SystemAllocatedMemory = NULL;
		}
	}

	void Trampoline::SystemRestore(off_t src, size_t src_len)
	{
		assert(sceKernelReleaseDirectMemory(src, src_len) == 0);
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

	void* Trampoline::allocate(Xbyak::CodeGenerator& code)
	{
		size_t length = code.getSize();
		void* codebuf = Take(length);
		memcpy(codebuf, code.getCode(), length);
		return codebuf;
	}

	void* Trampoline::allocatePlugin(size_t a_handle, size_t a_size)
	{
		auto mutex = Trampoline::getallocMutex();
		auto map = Trampoline::getallocMap();

		auto alloc = Take(a_size);
		if (alloc)
		{
			if (mutex.Lock())
			{
				auto entry = map.find(a_handle);
				if (entry == map.end())
				{
					auto insert = map.insert(std::make_pair(a_handle, a_size));
					assert(insert.second);
				}
				else
					entry->second += a_size;

				mutex.Unlock();
			}
		}
		else
		{
			assert(false);
		}

		return alloc;
	}

	Trampoline g_Trampoline;
}