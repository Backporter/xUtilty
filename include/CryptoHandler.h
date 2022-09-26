#pragma once
#include <stdint.h>
#include <cstring>

namespace CryptoHandler
{
	void GetCRC32(uint32_t* a_out, const char* a_source);
	void GetCRC32_2(uint32_t* a_out, const char* a_source, uint32_t a_start = 0);
}