#pragma once

// xxxxxxx_t
#include <stdint.h>

namespace xUtilty
{
	namespace CPU
	{
		uint64_t GetFrequency();
		uint64_t GetFrequency(int core);
	}
}