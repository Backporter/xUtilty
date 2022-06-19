#include "../include/MemoryHandler.h"
#include "../include/MessageHandler.h"
#include "../include/Macro.h"

#if defined(__ORBIS__)
#include <kernel.h>
#elif defined(__OPENORBIS__)
#include <orbis/libkernel.h>
#endif

#include <stdint.h>
#include <iostream>


namespace OrbisMemoryHandler
{
	void WriteBuffer(uint64_t dst, void* src, size_t len)
	{
		int32_t ret = 0;
		int32_t prot = VM_PROT_ALL;

#if defined (__ORBIS__) || defined(__OPENORBIS__)
		if ((ret = sceKernelQueryMemoryProtection((void*)dst, NULL, NULL, &prot)) != 0)
		{
			MessageHandler::KernelPrintOut("Failed to get memory protection.. could crash? ret 0x%lx", ret);
			prot = VM_PROT_ALL;
		}
#endif

#if defined (__ORBIS__) || defined(__OPENORBIS__)
		sceKernelMprotect((void*)dst, len, VM_PROT_ALL);
#endif

		memcpy((void *)dst, src, len);

#if defined (__ORBIS__) || defined(__OPENORBIS__)
		sceKernelMprotect((void*)dst, len, prot);
#endif
	}

}