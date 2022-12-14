#pragma once

#include <stdint.h>

namespace CPU
{
	uint64_t GetFrequency();
	uint64_t GetFrequency(int core);
}