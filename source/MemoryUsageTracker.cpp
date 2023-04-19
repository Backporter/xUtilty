#include "../include/SystemWrapper.h"
#include "../include/MemoryUsageTracker.h"

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
#endif
	}

	size_t	MemoryUsageTracker::GetCurrentSystemSize()		
	{ 
#if __ORBIS__
		return stats.currentSystemSize;			
#endif
	}

	size_t	MemoryUsageTracker::GetMaxInuseSize()			
	{ 
#if __ORBIS__
		return stats.maxInuseSize;							   
#endif
	}

	size_t	MemoryUsageTracker::GetCurrentInuseSize()		
	{ 
#if __ORBIS__
		return stats.currentInuseSize;						   
#endif
	}

	size_t	MemoryUsageTracker::GetCurrentFreeMemory()		
	{ 
#if __ORBIS__
		return stats.currentSystemSize - stats.currentInuseSize; 
#endif
	}

	size_t  MemoryUsageTracker::GetCurrentAllocatedMemory() {														   }
}