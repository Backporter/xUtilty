#include "../include/GPU.h"

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