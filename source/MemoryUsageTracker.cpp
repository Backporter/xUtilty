#include "../include/SystemWrapper.h"
#include "../include/MemoryUsageTracker.h"

namespace MemoryUtil
{
	MemoryUsageTracker::MemoryUsageTracker()
	{
		stats.size = sizeof(stats);
		stats.version = 0x0001U;
		stats.reserved1 = 0;
		stats.maxSystemSize = 0;
		stats.currentSystemSize = 0;
		stats.maxInuseSize = 0;
		stats.currentInuseSize = 0;
	}

	int		MemoryUsageTracker::Update()					{ return malloc_stats(&stats);							   }
	size_t	MemoryUsageTracker::GetMaxSystemSize()			{ return stats.maxSystemSize;							   }
	size_t	MemoryUsageTracker::GetCurrentSystemSize()		{ return stats.currentSystemSize;						   }
	size_t	MemoryUsageTracker::GetMaxInuseSize()			{ return stats.maxInuseSize;							   }
	size_t	MemoryUsageTracker::GetCurrentInuseSize()		{ return stats.currentInuseSize;						   }
	size_t	MemoryUsageTracker::GetCurrentFreeMemory()		{ return stats.currentSystemSize - stats.currentInuseSize; }
	size_t  MemoryUsageTracker::GetCurrentAllocatedMemory() {														   }
}