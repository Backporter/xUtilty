#include "../include/GPU.h"

//
#include "../include/SystemWrapper.h"

namespace xUtilty
{
	namespace GPU
	{
		uint64_t GetFrequency()
		{
#if defined(__ORBIS__)
			uint64_t ret = SystemWrapper::getGpuCoreClockFrequency();
#else
			uint64_t ret = 0;
#endif	
			return ret;
		}
	}
}