#include "../include/Trampoline.h"

#if INCLUDE_TRAPOLINE
#include "../include/RelocationManager.h"
#include "../include/Logger.h"
#include "../include/SystemWrapper.h"
#include "../include/RelocationManager.h"

#include <assert.h>
#include <stdio.h>
#include <stddef.h>
#include <iostream>

#if __ORBIS__
#include <kernel.h>
#elif __OPENORBIS__
#include <orbis/libkernel.h>
#endif

namespace xUtilty
{
	Trampoline::~Trampoline()  { release(); }

	std::pair<void*, int64_t> Trampoline::allocate_sys(size_t a_size, size_t a_alignment, void* a_pModuleBase)
	{
		int64_t s_physicalAddress;
		void* pData = nullptr;

#if __ORBIS__ || __OPENORBIS__
		if (SystemWrapper::AllocateDirectMemory(0, SystemWrapper::GetDirectMemorySize(), a_size, a_alignment, 0, s_physicalAddress) < 0)
		{
			Log::GetSingleton(Log::LoggerInstance::kModuleLog)->Write(Log::logLevel::kFatal, "Failed to Allocate memory for Trampoline [%s:%s:%d]", __FILE__, __FUNCTION__, __LINE__);
		
			return { 0, 0 };
		}

		pData = a_pModuleBase;

		if (SystemWrapper::MapDirectMemory((void**)&pData, a_size, 0x02, 0, s_physicalAddress, a_size) < 0)
		{
			Log::GetSingleton(Log::LoggerInstance::kModuleLog)->Write(Log::logLevel::kFatal, "Failed to map memory for Trampoline [%s:%s:%d]", __FILE__, __FUNCTION__, __LINE__);
			
			return { 0, 0 };
		}

		if (SystemWrapper::Mprotect((const void*)pData, a_size, VM_PROT_ALL) != 0)
		{
			Log::GetSingleton(Log::LoggerInstance::kModuleLog)->Write(Log::logLevel::kFatal, "Failed to Mprotect memory for Trampoline [%s:%s:%d]", __FILE__, __FUNCTION__, __LINE__);

			return { 0, 0 };
		}
#elif __SWITCH__

#elif _WIN32 || _WIN64

#endif
		return { pData, s_physicalAddress };
	}

	bool Trampoline::create(size_t a_size, size_t a_alignment, void* a_pModuleBase)
	{
		// invalid size, break.
		if (a_size == 0)
		{
			return false;
		}

		// check if a_pModuleBase is invaild and the magic number isn't set.
		if (a_alignment != 0xDEADBEEF)
		{
			a_pModuleBase = (void*)RelocationManager::ApplicationBaseAddress[0];
		}
		else
		{
			a_alignment = 0;
		}

#if _DEBUG
		PRINT_FMT("%zu %zu %p", a_size, a_alignment, a_pModuleBase);
#endif

		auto mem = allocate_sys(a_size, a_alignment, a_pModuleBase);
		if (!mem.first)
		{
			return false;
		}

		Setup(
			mem.first,
			mem.second,
			a_size,
			[](void* a_dat, size_t a_size, int64_t a_physicalAddress) { SystemWrapper::ReleaseDirectMemory(a_physicalAddress, a_size); }
		);

		return true;
	}

	void* Trampoline::allocate(size_t a_size)
	{
		if (a_size > (m_capacity - m_size))
		{
			Log::GetSingleton(0)->Write(Log::logLevel::kFatal, "Failed to allocate memory");
			__asm volatile ("int3");
		}

		auto mem = m_data + m_size;
		m_size += a_size;

		return mem;
	}
#if __clang__ || _WIN64
	void* Trampoline::allocate(Xbyak::CodeGenerator& a_code)
	{
		auto mem = allocate(a_code.getSize());
		log_usage();
		memcpy(mem, a_code.getCode(), a_code.getSize());
		return mem;
	}
#endif
	void  Trampoline::log_usage()
	{
		auto pct = (static_cast<double>(m_size) / static_cast<double>(m_capacity)) * 100.0;

		Log::GetSingleton(Log::kModuleLog)->Write(Log::logLevel::kDebug, "Trampoline size %zu, Trampoline capacity %zu, Trampoline usage percent %f.", m_size, m_capacity, pct);
	}

	void* Trampoline::allocatePlugin(size_t a_handle, size_t a_size)
	{
		void* alloc = allocate(a_size);
		if (alloc)
		{
			std::lock_guard<std::mutex> locker(m_PluginMaplock);

			auto entry = m_PluginMap.find(a_handle);
			if (entry == m_PluginMap.end())
			{
				auto insert = m_PluginMap.insert(std::make_pair(a_handle, a_size));
			}
			else
			{
				entry->second += a_size;
			}
		}
		else
		{
			Log::GetSingleton(Log::kModuleLog)->Write(Log::logLevel::kFatal, "Failed to allocate %Iu for %Iu", a_size, a_handle);
			assert(false);
		}

		return alloc;
	}
}
#endif