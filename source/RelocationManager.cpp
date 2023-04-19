#include "../include/Mira.h"
#include "../include/SystemWrapper.h"
#include "../include/FileSystem.h"
#include "../include/RelocationManager.h"

#pragma init_seg(lib)

namespace RelocationManager
{
	static RelocationManager RelocationManger;

	uintptr_t RelocationManager::ApplicationBaseAddress = 0;

	RelocationManager::RelocationManager()
	{
		SystemWrapper::initialize();

#if __ORBIS__ || __OPENORBIS__
		if (OrbisMiraHandler::IsMira())
		{
			MessageHandler::KernelPrintOut("Mira Detected.. using mira API");
			ApplicationBaseAddress = OrbisMiraHandler::GetBaseAddress();

		}
		else
		{
			MessageHandler::KernelPrintOut("Mira not detected.. using system API");
			auto sysbaseaddr = SystemWrapper::GetModuleAddress("/app0/eboot.bin");
			ApplicationBaseAddress = !sysbaseaddr ? 0x400000 : sysbaseaddr;
		}
#endif
	}
}