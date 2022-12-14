#pragma once

#include "CryptoHandler.h"
#include "RelocationManager.h"
#include "OrbisOffsertManger.h"
#include "MemoryHandler.h"

#include <stdio.h>
#include <stdint.h>
#include <assert.h>

template <typename T>
class Relocation
{
public:
	Relocation() { }

	Relocation(uintptr_t a_offset) { Set(a_offset); }

	Relocation(uint32_t a_id, uintptr_t a_offset) // : Relocation(a_offset)
	{
		if (OrbisOffsetManger::g_useDB)
		{
			uintptr_t s_off = OrbisOffsetManger::OffsetManger::GetSingleton()->GetOffset(a_id);
			
			if (s_off != -1)
				Set(s_off);
			else
				Set(a_offset);
		}
		else
		{
			Set(a_offset);
		}
	}

	Relocation(uint32_t a_id, uint32_t a_id2, uintptr_t a_offset) // : Relocation(a_offset)
	{
		if (OrbisOffsetManger::g_useDB)
		{
			uintptr_t s_off = OrbisOffsetManger::OffsetManger::GetSingleton()->GetOffset(a_id);
			
			if (s_off != -1)
				Set(s_off);
			else
			{
				s_off = OrbisOffsetManger::OffsetManger::GetSingleton()->GetOffset(a_id2);
				
				if (s_off != -1)
					Set(s_off);
			}
		}
		else
		{
			 Set(a_offset);
		}
	}

	Relocation(const char* a_id, uintptr_t a_offset) : Relocation(CryptoHandler::GetCRC32(a_id), a_offset)
	{}

	Relocation(const char* a_id, const char* a_id2, uintptr_t a_offset) : Relocation(a_offset, CryptoHandler::GetCRC32(a_id), CryptoHandler::GetCRC32(a_id2))
	{}
	
public:

	void Set(uintptr_t a_offset) { m_offset = reinterpret_cast <void*>(a_offset + RelocationManager::RelocationManager::ApplicationBaseAddress); }

	operator T()
	{
		return reinterpret_cast<T>(m_offset);
	}

	Relocation<T>& operator=(Relocation<T>& a_rhs)
	{
		if (a_rhs != this)
			m_offset = a_rhs.m_offset;

		return *this;
	}

	uintptr_t operator+(uintptr_t a_value)
	{
		m_offset = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(m_offset) + a_value);
		return reinterpret_cast<uintptr_t>(m_offset);
	}
	
	uintptr_t operator=(uintptr_t address)
	{
		m_offset = reinterpret_cast<void*>(address);
		return reinterpret_cast<uintptr_t>(m_offset);
	}

	uintptr_t operator+=(uint64_t count)
	{
		m_offset = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(m_offset) + count);
		return reinterpret_cast<uintptr_t>(m_offset);
	}
public:
	T GetPtr()
	{
		return reinterpret_cast<T>(m_offset);
	}

	uintptr_t GetUIntPtr() const
	{
		return reinterpret_cast <uintptr_t>(m_offset);
	}

	// vfunc stuff
	uintptr_t write_vfunc_P(int idx, void* a_newFunc)
	{
		auto newFunc = a_newFunc;

		uintptr_t addr = (uintptr_t)m_offset + (sizeof(void*) * idx);
		const auto result = *reinterpret_cast<uintptr_t*>(addr);

		OrbisMemoryHandler::WriteType<void*>(addr, &newFunc);
		return result;
	}

	uintptr_t write_vfunc(int idx, void* a_newFunc)
	{
		return write_vfunc_P(idx, a_newFunc);
	}

	template <class X>
	uintptr_t write_vfunc(int idx, X a_newFunc)
	{
		auto data = &a_newFunc;
		void* ptr = *reinterpret_cast<void**>(data);
		
		auto ret = write_vfunc_P(idx, ptr);
		return ret;
	}
private:
	void*		m_offset;
};