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
		if (!dst)
		{
			PRINT_RETURN_N("Failed to write memory, destination address is invalid (0x%lx, %p, 0x%lx", dst, src, len);
		}

		int32_t ret = 0;
		int32_t prot = VM_PROT_ALL;

#if defined (__ORBIS__) || defined(__OPENORBIS__)
		if ((ret = sceKernelQueryMemoryProtection((void*)dst, NULL, NULL, &prot)) != 0)
		{
			MessageHandler::KernelPrintOut("Failed to get memory protection.. could crash? ret 0x%lx(0x%lx, %p, 0x%lx)", ret, dst, src, len);
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

	void ReadBuffer(void* dst, uint64_t src, size_t len)
	{
		if (!src)
		{
			PRINT_RETURN_N("Failed to write memory, destination address is invalid (0x%lx, %p, 0x%lx", src, dst, len);
		}

		int32_t ret = 0;
		int32_t prot = VM_PROT_ALL;

#if defined (__ORBIS__) || defined(__OPENORBIS__)
		if ((ret = sceKernelQueryMemoryProtection((void*)src, NULL, NULL, &prot)) != 0)
		{
			MessageHandler::KernelPrintOut("Failed to get memory protection.. could crash? ret 0x%lx(0x%lx, %p, 0x%lx)", ret, src, src, len);
			prot = VM_PROT_ALL;
		}
#endif

#if defined (__ORBIS__) || defined(__OPENORBIS__)
		sceKernelMprotect((void*)src, len, VM_PROT_ALL);
#endif

		memcpy((void *)src, dst, len);

#if defined (__ORBIS__) || defined(__OPENORBIS__)
		sceKernelMprotect((void*)src, len, prot);
#endif
	}
}