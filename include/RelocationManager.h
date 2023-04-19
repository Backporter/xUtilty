#pragma once

#include <stdint.h>
#include <vector>

namespace RelocationManager
{
	class RelocationManager
	{
	public:
		RelocationManager();
		
		// "eboot.bin"
		static uintptr_t ApplicationBaseAddress;
	};
}