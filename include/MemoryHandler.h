#pragma once

#include <stdint.h>
#include <iostream>
#include <stdint.h>
#include <type_traits>
#include <cstring>

#include "RelocationManager.h"
#include "SystemWrapper.h"

namespace OrbisMemoryHandler
{
	void WriteBuffer(uint64_t dst, void* src, size_t len);

	void ReadBuffer(void* dst, uint64_t src, size_t len);

	template <typename T>
	void WriteType(uintptr_t dst, T data, size_t size = sizeof(T))
	{
		if constexpr (std::is_pointer<T>::value)
		{
			WriteBuffer(dst, data, size);
		}
		else
		{
			WriteBuffer(dst, &data, size);
		}
	}

	template <typename T>
	void WriteType(uintptr_t dst, T data, bool AddBase, size_t size = sizeof(T))
	{
		if constexpr (std::is_pointer<T>::value)
		{
			WriteBuffer(dst + RelocationManager::RelocationManager::RelocationManager::ApplicationBaseAddress, data, size);
		}
		else
		{
			WriteBuffer(dst + RelocationManager::RelocationManager::RelocationManager::ApplicationBaseAddress, &data, size);
		}
	}

	template <typename T>
	void ReadType(uintptr_t dst, T data, size_t size = sizeof(T))
	{
		if constexpr (std::is_pointer<T>::value)
		{
			ReadBuffer(dst, data, size);
		}
		else
		{
			ReadBuffer(dst, &data, size);
		}
	}

	template <typename T>
	void ReadType(uintptr_t dst, T data, bool AddBase, size_t size = sizeof(T))
	{
		if constexpr (std::is_pointer<T>::value)
		{
			ReadBuffer(dst + RelocationManager::RelocationManager::RelocationManager::ApplicationBaseAddress, data, size);
		}
		else
		{
			ReadBuffer(dst + RelocationManager::RelocationManager::RelocationManager::ApplicationBaseAddress, &data, size);
		}
	}

}