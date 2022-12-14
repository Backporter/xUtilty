#include "../include/Platform.h"

namespace Platform
{
	// global vars for platform type, Prospero "support" is just defs that we can use in-the future. 
	bool KernelIsNeonMode = (sceKernelIsNeoMode());
	bool IsSlim = (KernelIsNeonMode == 0);
	bool IsNeo = (KernelIsNeonMode == 1);
	bool IsProspero = (IsSlim != true && IsNeo != true);
}