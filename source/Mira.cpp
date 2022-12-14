#if defined (__ORBIS__) || defined(__OPENORBIS__)
#include <stdint.h>
#include "../include/INIHandler.h"
#include "../include/MessageHandler.h"
#include "../include/SystemWrapper.h"
#include "../include/Mira.h"

#if defined(__ORBIS__)
#include <kernel.h>
#elif defined(__OPENORBIS__)
#include <orbis/libkernel.h>
#endif

namespace OrbisMiraHandler
{
	std::vector<ProcessModuleList::Module> g_processmodulelist;

	std::vector<ProcessModuleList::Module> ProcessModules(int m_DriverHandle)
	{
		int m_ProcessId = -1;

		DriverIoctl s_Ioctl =
		{
			.Input = reinterpret_cast<void*>(m_ProcessId),
			.InputSize = sizeof(m_ProcessId),
			.Output = nullptr,
			.OutputSize = 0
		};

		if (m_DriverHandle <= 0)
		{
			return std::vector<ProcessModuleList::Module>();
		}

		auto s_Ret = SystemWrapper::ioctl(m_DriverHandle, MIRA_PROCESS_MODULE_LIST, &s_Ioctl);
		if (s_Ret != 0)
		{
			return std::vector<ProcessModuleList::Module>();
		}

		ProcessModuleList* s_ModuleList = static_cast<ProcessModuleList*>(s_Ioctl.Output);
		if (s_ModuleList == nullptr)
		{
			return std::vector<ProcessModuleList::Module>();
		}

		auto s_Modules = s_ModuleList->Modules;
		auto s_ModuleCount = s_ModuleList->ModuleCount;

		return std::vector<ProcessModuleList::Module>(s_Modules, s_Modules + s_ModuleCount);
	}

	void InitializeModueList()
	{
		if (g_processmodulelist.size() <= 0)
		{
			int fd = open("/dev/mira", 0, 0);
			if (fd < 0)
			{
				return;
			}
			g_processmodulelist = ProcessModules(fd);
			close(fd);
		}
	}

	bool ReadMemory(void* p_Address, void* p_OutData, uint32_t p_Size)
	{
		if (!p_Address || !p_OutData || !p_Size)
			return false;

		int m_ProcessId = -1;
		int m_DriverHandle = open("/dev/mira", 0, 0);

		if (m_DriverHandle < 0)
		{
			close(m_DriverHandle);
			return false;
		}

		ProcessReadMemory s_ReadMemory =
		{
			.ProcessId = m_ProcessId,
			.Address = p_Address,
			.DataSize = p_Size
		};

		DriverIoctl s_Ioctl =
		{
			.Input = &s_ReadMemory,
			.InputSize = sizeof(s_ReadMemory),
			.Output = p_OutData,
			.OutputSize = p_Size
		};

		auto s_Ret = SystemWrapper::ioctl(m_DriverHandle, MIRA_PROCESS_READ_MEMORY, &s_Ioctl);
		if (s_Ret != 0)
		{
			close(m_DriverHandle);
			return false;
		}

		close(m_DriverHandle);
		return true;
	}

	bool WriteMemory(void* p_Address, void* p_InData, uint32_t p_Size)
	{
		if (!p_Address || !p_InData || !p_Size)
			return false;

		int m_ProcessId = -1;
		int m_DriverHandle = open("/dev/mira", 0, 0);

		if (m_DriverHandle < 0)
		{
			close(m_DriverHandle);
			return false;
		}

		ProcessWriteMemory s_WriteMemory =
		{
			.ProcessId = m_ProcessId,
			.Address = p_Address,
			.DataSize = p_Size,
			.Data = p_InData,
		};

		DriverIoctl s_Ioctl =
		{
			.Input = &s_WriteMemory,
			.InputSize = sizeof(s_WriteMemory),
			.Output = nullptr,
			.OutputSize = 0
		};

		auto s_Ret = SystemWrapper::ioctl(m_DriverHandle, MIRA_PROCESS_WRITE_MEMORY, &s_Ioctl);
		if (s_Ret != 0)
		{
			close(m_DriverHandle);
			return false;
		}

		close(m_DriverHandle);
		return true;
	}

	bool ProtectMemory(void* p_Address, uint32_t p_Size, int32_t p_Protection)
	{
		if (!p_Address || !p_Size)
			return false;

		int m_ProcessId = -1;
		int m_DriverHandle = open("/dev/mira", 0, 0);

		ProcessProtectMemory s_ProtectMemory =
		{
			.ProcessId = m_ProcessId,
			.Protection = p_Protection,
			.Size = p_Size,
			.Address = p_Address
		};

		DriverIoctl s_Ioctl =
		{
			.Input = &s_ProtectMemory,
			.InputSize = sizeof(s_ProtectMemory),
			.Output = nullptr,
			.OutputSize = 0
		};

		auto s_Ret = SystemWrapper::ioctl(m_DriverHandle, MIRA_PROCESS_PROTECT_MEMORY, &s_Ioctl);
		if (s_Ret != 0)
		{
			close(m_DriverHandle);
			return false;
		}

		close(m_DriverHandle);
		return true;

	}

	uintptr_t GetBaseAddress(const char* path)
	{
		int ret = 0;
		uintptr_t addr = 0x400000;

		if (g_processmodulelist.size() <= 0)
		{
			int fd = open("/dev/mira", 0, 0);
			if (fd < 0)
			{
				return addr;
			}
			g_processmodulelist = ProcessModules(fd);
			close(fd);
		}

		for (auto l_Module : g_processmodulelist)
		{
			if (strcasecmp(l_Module.Path, path) == 0)
			{
				MessageHandler::KernelPrintOut("va: (%p) textsz: (%p) data: (%p), datasz: (%p), path: (%s), pltgot: (%p), entry: (%p) realloc: (%p)", l_Module.VirtualAddressBase, l_Module.TextSize, l_Module.DataBase, l_Module.DataSize, l_Module.Path, l_Module.PltGot, l_Module.Entry, l_Module.ReallocBase);
				addr = (uintptr_t)l_Module.ReallocBase;
				break;
			}
		}

		return addr;
	}

	bool IsMira()
	{
		int fd = open("/dev/mira", 0, 0);
		if (fd < 0)
		{
			close(fd);
			return false;
		}
		close(fd);
		return true;
	}

}
#endif