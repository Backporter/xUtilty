#include "../include/MemoryUsageTracker.h"

//
#include "../include/SystemWrapper.h"

namespace xUtilty
{
	namespace MemoryUtil
	{
		MemoryUsageTracker::MemoryUsageTracker()
		{
#if __ORBIS__
			stats.size = sizeof(stats);
			stats.version = 0x0001U;
			stats.reserved1 = 0;
			stats.maxSystemSize = 0;
			stats.currentSystemSize = 0;
			stats.maxInuseSize = 0;
			stats.currentInuseSize = 0;
#endif
		}

		int		MemoryUsageTracker::Update()
		{
#if __ORBIS__
			return malloc_stats(&stats);
#else
			return 0;
#endif
		}

		size_t	MemoryUsageTracker::GetMaxSystemSize()
		{
#if __ORBIS__
			return stats.maxSystemSize;
#else
			return 0;
#endif
		}

		size_t	MemoryUsageTracker::GetCurrentSystemSize()
		{
#if __ORBIS__
			return stats.currentSystemSize;
#else
			return 0;
#endif
		}

		size_t	MemoryUsageTracker::GetMaxInuseSize()
		{
#if __ORBIS__
			return stats.maxInuseSize;
#else
			return 0;
#endif
		}

		size_t	MemoryUsageTracker::GetCurrentInuseSize()
		{
#if __ORBIS__
			return stats.currentInuseSize;
#else
			return 0;
#endif
		}

		size_t	MemoryUsageTracker::GetCurrentFreeMemory()
		{
#if __ORBIS__
			return stats.currentSystemSize - stats.currentInuseSize;
#else
			return 0;
#endif
		}

		size_t  MemoryUsageTracker::GetCurrentAllocatedMemory()
		{
#if __ORBIS__ 
			return stats.currentSystemSize;
#else
			return 0;
#endif
		}
	}
}