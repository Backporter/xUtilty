#include <assert.h>
#include <stdio.h>
#include <stddef.h>
#include <iostream>

#include "../T/Trampoline.h"
#include "../O/OrbisRelocationManager.h"
#include "../M/Macro.h"

#if defined(__ORBIS__)
#include <kernel.h>
#elif defined(__OPENORBIS__)
#include <orbis/libkernel.h>
#endif

namespace Trampoline
{
	Trampoline::Trampoline()
	{
		SystemAllocatedMemory  = nullptr;
		SystemAllocatedLength  = 0;
		SystemAllocatedOffset  = 0;
		AllocSysAllocLen	   = 0;
		AllocSysAllocAddr	   = nullptr;
	}

	Trampoline::~Trampoline()
	{
		SystemDeallocate();
	}

	bool Trampoline::SystemAllocate(size_t TrampolineSize)
	{
		int32_t ret = 0;
		if ((ret = sceKernelAllocateDirectMemory(0, sceKernelGetDirectMemorySize(), TrampolineSize, TrampolineSize, 0, &SystemAllocatedOffset)) != 0)
		{
			OrbisMessageHandler::Notify("Failed to allocate Direct Memory! sceKernelAllocateDirectMemory failed 0x%08X", ret);
			return false;
		}

		SystemAllocatedMemory = reinterpret_cast<void*>(OrbisRelocationManager::OrbisRelocationManager::ApplicationBaseAddress == 0 ? 0x400000 : OrbisRelocationManager::OrbisRelocationManager::ApplicationBaseAddress);

		if ((ret = sceKernelMapDirectMemory(&SystemAllocatedMemory, TrampolineSize, 0x02, 0, SystemAllocatedOffset, TrampolineSize)) != 0)
		{
			OrbisMessageHandler::Notify("Failed To Map Direct Memory! sceKernelMapDirectMemory returned %lx", ret);
			return false;
		}

		if ((ret = sceKernelMprotect(SystemAllocatedMemory, TrampolineSize, VM_PROT_ALL)) != 0)
		{
			OrbisMessageHandler::Notify("Failed to change protection![this is fatal]... sceKernelMprotect returned %lx", ret);
			return false;
		}

		SystemAllocatedLength = TrampolineSize;
		AllocSysAllocLen = 0;
		return SystemAllocatedMemory != nullptr && ret == 0;
	}

	void Trampoline::SystemDeallocate()
	{
		if (SystemAllocatedMemory)
		{
			sceKernelReleaseDirectMemory(SystemAllocatedOffset, SystemAllocatedLength);
			SystemAllocatedMemory = nullptr;
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

	Trampoline g_Trampoline;
}