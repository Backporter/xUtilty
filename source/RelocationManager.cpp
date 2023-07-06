#include "../include/RelocationManager.h"

//
#include "../include/Mira.h"
#include "../include/SystemWrapper.h"

#pragma init_seg(lib)

namespace xUtilty
{
	static RelocationManager RelocationManger;

	uintptr_t RelocationManager::ApplicationBaseAddress[256] = { };

	RelocationManager::RelocationManager()
	{
		SystemWrapper::initialize();

#if __ORBIS__ || __OPENORBIS__
		if (OrbisMiraHandler::IsMira())
		{
			KernelPrintOut("Mira Detected.. using mira API");
			OrbisMiraHandler::SetModuleArray(ApplicationBaseAddress);

			// ApplicationBaseAddress[0] = OrbisMiraHandler::GetBaseAddress();
		}
		else
		{
			KernelPrintOut("Mira not detected.. using system API");
			SystemWrapper::SetModuleArray(ApplicationBaseAddress);

			// auto sysbaseaddr = SystemWrapper::GetModuleAddress("/app0/eboot.bin");
			// ApplicationBaseAddress[0] = !sysbaseaddr ? 0x400000 : sysbaseaddr;
		}
#endif
	}
}