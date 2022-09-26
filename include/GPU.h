#pragma once
#include <stdint.h>

#if defined(__ORBIS__)
#include <kernel.h>
#include <gnmx.h>
#include <gnm.h>
#include <gpu_address.h>
#elif defined (__OPENORBIS__)
#include <orbis/libkernel.h>

#endif

namespace GPU
{
	uint64_t GetFrequency();
}