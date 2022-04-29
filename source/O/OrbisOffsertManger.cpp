#include "../O/OrbisOffsertManger.h"
#include "../O/OrbisFileSystem.h"
#include "../O/OrbisSFOHandler.h"

#if defined(__ORBIS__)
#include <kernel.h>
#elif defined(__OPENORBIS__)
#include <orbis/libkernel.h>
#endif

namespace OrbisOffsetManger
{
	void OffsetManger::Initialize()
	{
		char path[260] { 0 };

		auto TITILE = OrbisSFOHandler::sfo_parser::GetSingleton()->GetKeySTR("TITLE");
		auto APP_VER = OrbisSFOHandler::sfo_parser::GetSingleton()->GetKeySTR("APP_VER");

		if (!OrbisFileSystem::CreateFullPath(path, OrbisFileSystem::App, "data/CSEL/%s-%s.offset", TITILE, APP_VER))
		{
			OrbisMessageHandler::KernelPrintOut("Failed to create full path...");
			OffsetFIle = NULL;
			return;
		}

		if (!OrbisFileSystem::PathExists(OrbisFileSystem::Full, path, false))
		{
			OrbisMessageHandler::KernelPrintOut("Failed to find OrbisOffsetManger's offset list (%s)", path);
			OffsetFIle = NULL;
			return;
		}

		OffsetFIle = strdup(path);
	}

	uint64_t OffsetManger::GetOffset(const char* ID)
	{
		OffsetDataBase::InternalOffsetStructure entry{ 0 };
		OffsetDataBase::Header Header{ 0 };

		if (!ID)
		{
			OrbisMessageHandler::KernelPrintOut("ID is invailed");
			return 0;
		}

		if ((this->FD = sceKernelOpen(OffsetFIle, O_RDONLY, 0000555)) < 0)
		{
			OrbisMessageHandler::KernelPrintOut("Failed to open %s for 0x%lx", OffsetFIle, this->FD);
			return 0;
		}

		if ((this->RET = sceKernelRead(this->FD, &Header, sizeof(OffsetDataBase::Header))) <= 0)
		{
			OrbisMessageHandler::KernelPrintOut("Failed to read header for 0x%lx", this->RET);
			goto safeexit;
		}
		
		if (Header.Magic != 0x4244534F)
		{
			OrbisMessageHandler::KernelPrintOut("Invaild magic got 0x%lx", Header.Magic);
			goto safeexit;
		}

		if (strcasecmp(Header.Platform, "ORBIS"))
		{
			OrbisMessageHandler::KernelPrintOut("Invaild platform got %s not ORBIS", Header.Platform);
			goto safeexit;
		}
		
		if (Header.Count == 0)
		{
			OrbisMessageHandler::KernelPrintOut("Invaild entry count");
			goto safeexit;
		}

		for (int i = 0; i < Header.Count; i++)
		{
			if ((this->RET = sceKernelRead(this->FD, &entry, sizeof(OffsetDataBase::InternalOffsetStructure))) <= 0)
			{
				OrbisMessageHandler::KernelPrintOut("Failed to read entry for 0x%lx", this->RET);
				goto safeexit;
			}

			if (strncasecmp(entry.ID, ID, 0x28) == 0)
			{
				OrbisMessageHandler::KernelPrintOut("Found %s in %s with offset of 0x%lx", ID, OffsetFIle, entry.offset);
				sceKernelClose(this->FD);
				return entry.offset;
			}
		}

		goto safeexit;

	safeexit:
		sceKernelClose(this->FD);
		return 0;
	}

}