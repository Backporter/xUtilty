#include "../include/Trampoline.h"

#include "../include/RelocationManager.h"
#include "../include/Logger.h"
#include "../include/SystemWrapper.h"
#include "../include/RelocationManager.h"

#include <assert.h>
#include <stdio.h>
#include <stddef.h>
#include <iostream>

#if defined(__ORBIS__)
#include <kernel.h>
#elif defined(__OPENORBIS__)
#include <orbis/libkernel.h>
#endif

namespace xUtilty
{
	void Trampoline::default_deallocator(void* a_data, size_t a_size, int64_t a_physicalAddress)
	{
		if (a_data)
		{
			SystemWrapper::ReleaseDirectMemory(a_physicalAddress, a_size);
		}
	}

	Trampoline::~Trampoline() 
	{ 
		release();
	}

	bool Trampoline::create(size_t a_size, size_t a_alignment, deallocator_t* a_freefunc, void* a_pModuleBase)
	{
		// invalid size, break.
		if (a_size == 0)
			return false;

		if (SystemWrapper::AllocateDirectMemory(0, SystemWrapper::GetDirectMemorySize(), a_size, a_alignment, 0, m_physicalAddress))
		{
			Log::GetSingleton(Log::LoggerInstance::KModuleLog)->Write("[Fatal Error] Failed to Allocate memory for Trampoline [%s:%s:%d", __FILE__, __FUNCTION__, __LINE__);
			return false;
		}

		if (a_pModuleBase)
			m_data = reinterpret_cast<uint8_t*>(a_pModuleBase);
		else
			m_data = reinterpret_cast<uint8_t*>(RelocationManager::ApplicationBaseAddress);

		if (a_freefunc)
			m_deallocator = a_freefunc;
		else
			m_deallocator = default_deallocator;

		if (SystemWrapper::MapDirectMemory((void**)&m_data, a_size, 0x02, 0, m_physicalAddress, a_size))
		{
			Log::GetSingleton(Log::LoggerInstance::KModuleLog)->Write("[Fatal Error] Failed to map memory for Trampoline [%s:%s:%d", __FILE__, __FUNCTION__, __LINE__);
			return false;
		}

		if (SystemWrapper::Mprotect((const void*)m_data, a_size, VM_PROT_ALL))
		{
			Log::GetSingleton(Log::LoggerInstance::KModuleLog)->Write("[Fatal Error] Failed to Mprotect memory for Trampoline [%s:%s:%d", __FILE__, __FUNCTION__, __LINE__);
			return false;
		}

		m_capacity = a_size;
		m_size = 0;

		return true;
	}

	void* Trampoline::allocate(size_t a_size)
	{
		auto mem = m_data + m_size;
		m_size += a_size;
		log_usage();
		return mem;
	}

	void* Trampoline::allocate(Xbyak::CodeGenerator& a_code)
	{
		auto mem = allocate(a_code.getSize());
		log_usage();
		memcpy(mem, a_code.getCode(), a_code.getSize());
		return mem;
	}

	void Trampoline::log_usage()
	{
		auto pct = (static_cast<double>(m_size) / static_cast<double>(m_capacity)) * 100.0;

		Log::GetSingleton(Log::KModuleLog)->Write("%zu / %zu (%f)", m_size, m_capacity, pct);
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
			Log::GetSingleton(Log::KModuleLog)->Write("Failed to allocate %Iu for %Iu", a_size, a_handle);
		}

		return alloc;
	}
}