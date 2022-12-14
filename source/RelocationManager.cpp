#include "../include/Mira.h"
#include "../include/SystemWrapper.h"
#include "../include/FileSystem.h"
#include "../include/RelocationManager.h"

#pragma init_seg(lib)

namespace RelocationManager
{
	static RelocationManager RelocationManger;

	uintptr_t RelocationManager::ApplicationBaseAddress = 0;
	uintptr_t RelocationManager::LibkernelBaseAddress = 0;

	RelocationManager::RelocationManager()
	{
		SystemWrapper::initialize();

		ApplicationBaseAddress = OrbisMiraHandler::GetBaseAddress();
	}
}