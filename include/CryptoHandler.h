#pragma once

// xxxxx_t
#include <stdint.h>

// strlen
#include <string.h>

namespace xUtilty
{
	namespace CryptoHandler
	{
		uint32_t GetCRC32(const char* a_str, uint32_t a_start = 0);
		void     GetCRC32(uint32_t& a_dst, const char* a_str, uint32_t a_start = 0);
	}
}