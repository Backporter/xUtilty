#include "../include/CPU.h"

namespace CPU
{
	uint64_t GetFrequency()
	{
		uint64_t ret = 0;
#if defined (__ORBIS__) || defined(__OPENORBIS__)
		if (OrbisSystemWrapper::sceKernelGetCpuFrequency)
		{
			ret = OrbisSystemWrapper::sceKernelGetCpuFrequency();
		}
#endif
		return ret;
	}

	uint64_t GetFrequency(int core)
	{
		return GetFrequency();
	}

}