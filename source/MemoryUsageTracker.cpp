#include "../include/MemoryUsageTracker.h"

namespace Memory
{ 
	size_t GetCurrentFreeMemoryLibc()
	{
#if defined (__ORBIS__) || defined(__OPENORBIS__)
		memset(&size, 0, sizeof(size));
		size.size = sizeof(size);
		size.version = 0x0001U;
		size_t isize = 0;

		// because idk if it's exported lets have a backup, k?
		if (OrbisSystemWrapper::sceKernelInternalMemoryGetAvailableSize && !OrbisSystemWrapper::sceKernelInternalMemoryGetAvailableSize(&isize))
		{
			return isize;
		}

#if defined(__ORBIS__)
		auto ret = malloc_stats(&size);
#else
		auto ret = 1;
#endif
		if (!ret)
		{
			return size.currentSystemSize - size.currentInuseSize;
		}
#endif
		return 0;
	}

	size_t GetCurrentAllocatedMemoryLibc()
	{
#if defined (__ORBIS__) || defined(__OPENORBIS__)
		memset(&size, 0, sizeof(size));
		size.size = sizeof(size);
		size.version = 0x0001U;

#if defined(__ORBIS__)
		auto ret = malloc_stats(&size);
#else 
		auto ret = 1;
#endif
		if (!ret)
		{
			return size.currentInuseSize;
		}
#endif
		return 0;
	}

	size_t GetCurrentFreeMemory()
	{
#if defined (__ORBIS__) || defined(__OPENORBIS__)
		auto ret = 0;
		int a_free = 0;
		int a_total = 0;
		
		if (OrbisSystemWrapper::get_page_table_stats && (ret = OrbisSystemWrapper::get_page_table_stats(-1, 1, &a_total, &a_free)) != -1)
		{
			MessageHandler::KernelPrintOut(strerror(errno));
			return a_free;
		}

		MessageHandler::KernelPrintOut(strerror(errno));
		MessageHandler::KernelPrintOut("%p 0x%lx", OrbisSystemWrapper::get_page_table_stats, ret);
#endif
		return 0;
	}

	size_t GetCurrentAllocatedMemory()
	{
#if defined (__ORBIS__) || defined(__OPENORBIS__)
		auto ret = 0;
		int a_free = 0;
		int a_total = 0;

		if (OrbisSystemWrapper::get_page_table_stats && (ret = OrbisSystemWrapper::get_page_table_stats(-1, 1, &a_total, &a_free)) != -1)
		{
			MessageHandler::KernelPrintOut(strerror(errno));
			return a_total - a_free;
		}

		MessageHandler::KernelPrintOut(strerror(errno));
		MessageHandler::KernelPrintOut("%p 0x%lx", OrbisSystemWrapper::get_page_table_stats, ret);
#endif
		return 0;
	}
}