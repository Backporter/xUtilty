#pragma once

#if __ORBIS__
#include <kernel.h>
#elif __OPENORBIS__
#include <orbis/libkernel.h>
#endif

#include "Macro.h"

namespace Platform
{
	// global vars for platform type, Prospero "support" is just defs that we can use in-the future. 
	extern bool KernelIsNeonMode;
	extern bool IsSlim;
	extern bool IsNeo;
	extern bool IsProspero;
}