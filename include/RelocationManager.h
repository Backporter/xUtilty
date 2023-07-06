#pragma once

#include <stdint.h>

namespace xUtilty
{
	class RelocationManager
	{
	public:
		RelocationManager();

		// [0] should be the main application, [1] should be libkernel.sprx, look at the klog to figure them out.
		static uintptr_t ApplicationBaseAddress[256];
	};
}