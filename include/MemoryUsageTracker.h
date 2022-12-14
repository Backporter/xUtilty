#pragma once

#include <stdint.h>

#if __ORBIS__
#include <mspace.h>
#elif __OPENORBIS__
#include <orbis/_types/libc.h>
#endif

#include <errno.h>

namespace MemoryUtil
{
	class MemoryUsageTracker
	{
	public:
		MemoryUsageTracker();
		~MemoryUsageTracker() { }
		
		int		Update();
		size_t	GetMaxSystemSize();
		size_t	GetCurrentSystemSize();

		size_t	GetMaxInuseSize();
		size_t	GetCurrentInuseSize();

		size_t	GetCurrentFreeMemory();
		size_t	GetCurrentAllocatedMemory();

		static MemoryUsageTracker* GetSingleton()
		{
			static MemoryUsageTracker s;
			return &s;
		}
	public:
		SceLibcMallocManagedSize stats;
	};
}