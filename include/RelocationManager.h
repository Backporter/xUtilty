#pragma once

#include <stdint.h>
#include <vector>

namespace RelocationManager
{
	class RelocationManager
	{
	public:
		RelocationManager();
		
		// "/app0/eboot.bin" / whatever the game runs via LoadExec
		static uintptr_t ApplicationBaseAddress;

		// "/" + sceKernelGetFsSandboxRandomWord() + "/" + "libkernel.sprx"
		static uintptr_t LibkernelBaseAddress;
	};
}