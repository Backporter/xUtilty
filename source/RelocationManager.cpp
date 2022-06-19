#include "../include/Mira.h"
#include "../include/SystemWrapper.h"
#include "../include/FileSystem.h"
#include "../include/RelocationManager.h"

#if defined(__SWITCH__) || defined(PLATFORM_NX)
#include "../include/switch/include/nn.h"
#include "../include/switch/include/svc.h"
Result memGetMap(MemoryInfo* info, uint64_t addr) {
    uint32_t map;
    return svcQueryMemory(info, &map, addr);
}

uint64_t memGetMapAddr(uint64_t addr) {
    MemoryInfo map;
    memGetMap(&map, addr);
    return map.addr;
}
#endif

#pragma warning(disable: 4073)
#pragma init_seg(lib)

namespace RelocationManager
{
	static RelocationManager RelocationManger;

	uintptr_t RelocationManager::ApplicationBaseAddress = 0;
	
	RelocationManager::RelocationManager()
	{
		OrbisSystemWrapper::initialize();

#if defined (__ORBIS__) || defined(__OPENORBIS__)
		ApplicationBaseAddress = OrbisMiraHandler::GetBaseAddress();
#elif defined(__SWITCH__) || defined(PLATFORM_NX)
		ApplicationBaseAddress = memGetMapAddr((uint64_t)nninitStartup);
#endif
	}
}