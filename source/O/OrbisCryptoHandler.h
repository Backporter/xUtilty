#pragma once
#include <stdint.h>

// nost PS4 specific, just keeping the naming scheme
namespace OrbisCryptoHandler
{
	void GetCRC32(uint32_t* Out, const char* Source);
}