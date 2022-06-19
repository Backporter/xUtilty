#include "../include/OrbisOffsertManger.h"
#include "../include/FileSystem.h"
#include "../include/OrbisSFOHandler.h"
#include "../include/MessageHandler.h"
#include "../include/SystemWrapper.h"

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
		const char* TITILE = NULL;
		const char* APP_VER = NULL;

#if defined (__ORBIS__) || defined(__OPENORBIS__)
		TITILE = OrbisSFOHandler::sfo_parser::GetSingleton()->GetKeySTR("TITLE");
		APP_VER = OrbisSFOHandler::sfo_parser::GetSingleton()->GetKeySTR("APP_VER");
		
		if (!TITILE)
			TITILE = "Skyrim";
		
		if (!APP_VER)
			APP_VER = "01.09";
#endif

		OrbisFileSystem::CreateFullPath(path, OrbisFileSystem::App, "data/CSEL/%s-%s.offset", TITILE, APP_VER);
		if (OrbisFileSystem::PathExists(OrbisFileSystem::Full, path, false))
		{
			OffsetFIle = strdup(path);
			return;
		}

		MessageHandler::KernelPrintOut("Failed to find OrbisOffsetManger's offset list (%s)", path);
		OffsetFIle = NULL;
		return;
	}

	bool      OffsetManger::Parse()
	{
	 	OffsetDataBase::InternalOffsetStructure entry{ 0 };

	 	if ((this->FD = open(OffsetFIle, 0, 0)) < 0)
	 	{
			goto error;
	 	}
	 
	 	if ((this->RET = read(this->FD, &Header, sizeof(OffsetDataBase::Header))) <= 0)
	 	{
			goto error;
		}
	 
	 	if (Header.Magic != GetPlatformID())
	 	{
			goto error;
	 	}
	 
	 	if (strcasecmp(Header.Platform, GetPlatformStringID()))
	 	{
			goto error;
	 	}
	 
	 	if (Header.Count == 0)
	 	{
			goto error;
	 	}
	 
	 	for (int i = 0; i < Header.Count; i++)
	 	{
	 		if ((this->RET = read(this->FD, &entry, sizeof(OffsetDataBase::InternalOffsetStructure))) <= 0)
	 		{
	 			MessageHandler::KernelPrintOut("Failed to read entry for 0x%lx(error id %d/%s)", this->RET, errno, strerror(errno));
				goto error;
	 		}

			this->Entries.push_back(entry);
	 	}

		goto successful;

	successful:
		close(this->FD);
		return true;
	error:
		close(this->FD);
		return false;
	}

	uint64_t  OffsetManger::GetOffset(const char* ID)
	{
		OffsetDataBase::InternalOffsetStructure entry{ 0 };

		if (!OffsetFIle)
		{
			goto safeexit;
		}

		if (!ID)
		{
			MessageHandler::KernelPrintOut("ID is invailed");
			goto safeexit;
		}

		if ((this->FD = open(OffsetFIle, 0, 0)) < 0)
		{
			MessageHandler::KernelPrintOut("Failed to open %s for 0x%lx(ID: %s)", OffsetFIle, this->FD, ID);
			goto safeexit;
		}

		if (Header.Magic != GetPlatformID())
		{
			MessageHandler::KernelPrintOut("Invaild magic got 0x%lx(ID: %s)", Header.Magic, ID);
			goto safeexit;
		}

		if (strcasecmp(Header.Platform, "ORBIS"))
		{
			MessageHandler::KernelPrintOut("Invaild platform got %s not ORBIS(ID: %s)", Header.Platform, ID);
			goto safeexit;
		}
		
		if (Header.Count == 0)
		{
			MessageHandler::KernelPrintOut("Invaild entry count(ID: %s)", ID);
			goto safeexit;
		}

		for (int i = 0; i < Header.Count; i++)
		{
			if ((this->RET = read(this->FD, &entry, sizeof(OffsetDataBase::InternalOffsetStructure))) <= 0)
			{
				MessageHandler::KernelPrintOut("Failed to read entry for 0x%lx(ID: %s, error id %d/%s)", this->RET, ID, errno, strerror(errno));
				goto safeexit;
			}

			if (strncasecmp(entry.ID, ID, 0x28) == 0)
			{
				MessageHandler::KernelPrintOut("Found %s in %s with offset of 0x%lx", ID, OffsetFIle, entry.offset);
				lseek(this->FD, 0, 0);
				close(this->FD);
				return entry.offset;
			}
		}

		MessageHandler::KernelPrintOut("Failed to find ID in database(ID: %s, Count: %d, Version: %d)", ID, Header.Count, Header.Version);
		goto safeexit;

	safeexit:
		lseek(this->FD, 0, 0);
		close(this->FD);
		return 0;
	}

	uintptr_t OffsetManger::_GetOffset(const char* ID)
	{
		auto EntriesSize = this->Entries.size();
		if (EntriesSize <= 0)
			if (!this->Parse())
				return 0;
		
		for (size_t i = 0; i < EntriesSize; i++)
		{
			auto entry = this->Entries[i];
			if (!strcasecmp(entry.ID, ID))
			{
				return entry.offset;
			}
		}

		return 0;
	}
}