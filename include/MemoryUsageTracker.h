#pragma once

// size_t
#include <cstdio>

#include <stdint.h>

#if __ORBIS__
#include <mspace.h>
#elif __OPENORBIS__
#endif

#include <errno.h>

namespace xUtilty
{
	namespace MemoryUtil
	{
		class MemoryUsageTracker
		{
		public:
			MemoryUsageTracker();
			~MemoryUsageTracker() = default;

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
#if __ORBIS__
			SceLibcMallocManagedSize stats;
#endif
		};
	}
}