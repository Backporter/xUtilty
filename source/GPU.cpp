#include "../include/GPU.h"

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
	uint64_t GetFrequency()
	{
#if defined(__ORBIS__)
		uint64_t ret = sce::Gnm::getGpuCoreClockFrequency();
#else
		uint64_t ret = 0;
#endif	
		return ret;
	}
}