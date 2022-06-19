#pragma once

#include <stdint.h>
#include <vector>

namespace RelocationManager
{
	class RelocationManager
	{
	public:
		RelocationManager();
		static uintptr_t ApplicationBaseAddress;
	};
}