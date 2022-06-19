#pragma once
#include <stdint.h>
#include <cstring>

namespace CryptoHandler
{
	void GetCRC32(uint32_t* Out, const char* Source);
}