#pragma once

#include <stdint.h>

namespace xUtilty
{
	class RelocationManager
	{
	public:
		RelocationManager();

		// "eboot.bin"
		static uintptr_t ApplicationBaseAddress;
	};
}