#pragma once
#include "SystemWrapper.h"

#include <memory>
#include <stdlib.h>
#include <errno.h>
#include <string>

namespace Memory
{
#if defined (__ORBIS__) || defined(__OPENORBIS__)
	static OrbisSystemWrapper::OrbisLibcMallocManagedSize size{ 0 };
#endif
	size_t GetCurrentFreeMemoryLibc();
	size_t GetCurrentAllocatedMemoryLibc();

	size_t GetCurrentFreeMemory();
	size_t GetCurrentAllocatedMemory();

}