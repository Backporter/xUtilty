#pragma once

#include "../../../OrbisUtil/source/O/OrbisRelocationManager.h"
#include "../../../OrbisUtil/source/O/OrbisOffsertManger.h"

#include <stdio.h>
#include <stdint.h>

template <typename T>
class Relocation
{
public:
	Relocation() = delete;

	Relocation(uintptr_t offset)
		:Offset(reinterpret_cast <void*>(offset + OrbisRelocationManager::OrbisRelocationManager::ApplicationBaseAddress))
	{
	}

	Relocation(const char* ID, uintptr_t defaultvalue)
	: 
		dboffset(OrbisOffsetManger::OffsetManger::GetSingleton()->GetOffset(ID)),
		Offset(reinterpret_cast <void*>((dboffset == 0 ? defaultvalue : dboffset) + OrbisRelocationManager::OrbisRelocationManager::ApplicationBaseAddress))
	{
	}

	operator T()
	{
		return reinterpret_cast <T>(Offset);
	}

	uintptr_t operator+= (uint64_t count)
	{
		Offset = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(Offset) + count);
		return reinterpret_cast<uintptr_t>(Offset);
	}

	uintptr_t GetUIntPtr() const
	{
		return reinterpret_cast <uintptr_t>(Offset);
	}

	// operator T() doesn't work with T* so we need to do this to stop the compiller from returning this instead of the pointer...
	T GetPtr()
	{
		return reinterpret_cast<T>(Offset);
	}

private:
	uint64_t	dboffset;
	void*		Offset;
};