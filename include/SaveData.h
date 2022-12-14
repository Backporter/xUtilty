#pragma once

#include <stdint.h>

namespace SaveGame
{
	struct SAVE
	{
		uint32_t		Magic;
		uint32_t		Version;

		unsigned long	OrginalSize;
		unsigned long	CompressedSize;
	};
}