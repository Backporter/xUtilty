#pragma once

#include "type_traits.h"

#include <stdint.h>
#include <array>

namespace OrbisMemoryHandler
{
	void DumpRegisters();

	void safe_write(uint64_t dst, const void* src, size_t len);
	void safe_read(const void* dst, uint64_t src, size_t len);

	template <class T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
	void safe_write(uintptr_t a_dst, const T& a_data)
	{
		safe_write(a_dst, std::addressof(a_data), sizeof(T));
	}

	template <class T, std::enable_if_t<std::is_pointer<T>::value, int> = 0>
	void safe_write(uintptr_t a_dst, const T a_ptr)
	{
		safe_write(a_dst, a_ptr, sizeof(T));
	}
}