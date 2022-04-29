#include "../M/Mira.h"
#include "../O/OrbisSystemWrapper.h"
#include "../O/OrbisRelocationManager.h"
#include "../O/OrbisFileSystem.h"

#pragma warning(disable: 4073)
#pragma init_seg(lib)

namespace OrbisRelocationManager
{
	static OrbisRelocationManager ConsoleRElocManger;

	uintptr_t OrbisRelocationManager::ApplicationBaseAddress = 0;
	
	OrbisRelocationManager::OrbisRelocationManager()
	{
		OrbisSystemWrapper::initialize();

		ApplicationBaseAddress = OrbisMiraHandler::GetBaseAddress();
	}
}