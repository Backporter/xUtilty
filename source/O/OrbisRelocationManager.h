#pragma once

#include <stdint.h>
#include <vector>

namespace OrbisRelocationManager
{
	class OrbisRelocationManager
	{
	public:
		OrbisRelocationManager();
		static uintptr_t ApplicationBaseAddress;			// base game.
	};
}