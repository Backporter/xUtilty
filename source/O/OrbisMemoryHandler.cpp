#include <stdint.h>
#include <iostream>

#include "../O/OrbisMessageHandler.h"
#include "../M/Macro.h"

#if defined(__ORBIS__)
#include <kernel.h>
#elif defined(__OPENORBIS__)
#include <orbis/libkernel.h>
#endif

namespace OrbisMemoryHandler
{
	void WriteBuffer(uint64_t dst, void* src, size_t len)
	{
		int32_t ret = 0;
		int32_t prot = VM_PROT_ALL;

		if ((ret = sceKernelQueryMemoryProtection((void*)dst, NULL, NULL, &prot)) != 0)
		{
			OrbisMessageHandler::KernelPrintOut("Failed to get memory protection.. could crash? ret 0x%lx", ret);
			prot = VM_PROT_ALL;
		}

		sceKernelMprotect((void*)dst, len, VM_PROT_ALL);
		memcpy((void *)dst, src, len);
		sceKernelMprotect((void*)dst, len, prot);
	}

}