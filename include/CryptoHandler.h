#pragma once

#include <stdint.h>

namespace CryptoHandler
{
	uint32_t GetCRC32(const char* a_str, uint32_t a_start = 0);
	void     GetCRC32(uint32_t& a_dst, const char* a_str, uint32_t a_start = 0);
}