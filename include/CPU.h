#pragma once
#include "SystemWrapper.h"

#if defined(__ORBIS__)
#include <kernel.h>
#include <razorcpu.h>
#elif defined (__OPENORBIS__)
#include <orbis/libkernel.h>
#endif

namespace CPU
{
	uint64_t GetFrequency();
	uint64_t GetFrequency(int core);
}