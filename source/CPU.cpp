#include "../include/CPU.h"

//
#include "../include/SystemWrapper.h"

namespace xUtilty
{
	namespace CPU
	{
		uint64_t GetFrequency()
		{
			uint64_t ret = 0;
			ret = SystemWrapper::sceKernelGetCpuFrequency();
			return ret;
		}
	
		uint64_t GetFrequency(int core)
		{
			return GetFrequency();
		}
	
	}
}