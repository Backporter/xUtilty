#if defined (__ORBIS__) || defined(__OPENORBIS__)
#include "../include/OrbisSFOHandler.h"
#include "../include/SystemWrapper.h"
#include "../include/FileSystem.h"


#if defined(__ORBIS__)
#include <kernel.h>
#elif defined(__OPENORBIS__)
#include <orbis/libkernel.h>
#endif

namespace OrbisSFOHandler
{
	sfo_parser::sfo_parser()
	{
	}

	sfo_parser::~sfo_parser()
	{
		// because we used strdup on both the key and the data.s, we need to free it

		for (int i = 0; i < this->Data.size(); i++)
		{
			if (this->Data[i].Type == sfo::SFO_UTF8)
			{
				free((void*)this->Data[i].Data.s);
				free((void*)this->Data[i].Key);
			}
		}
	}

	bool sfo_parser::ParseSFO(bool FixJailedState, char* Path)
	{
		if (FixJailedState)
			OrbisSystemWrapper::Jailbreak();

		FILE* FD;
		sfo::Header Header;
		std::vector<sfo::Entry> Entry;

		if (!(FD = fopen(Path, "rb")))
		{
			return false;
		}

		fread(&Header, sizeof(Header), 1, FD);

		for (int i = 0; i < Header.EntryCount; i++)
		{
			sfo::Entry e;
			fread(&e, sizeof(e), 1, FD);
			Entry.push_back(e);
		}

		for (int i = 0; i < Header.EntryCount; i++)
		{
			long pos = ftell(FD);
			int size = 0;
			uint8_t b = 0;
			bool loo = true;

			while (loo)
			{
				fread(&b, 1, 1, FD);
				size++;
				loo = (b != 0x00);
			}

			fseek(FD, pos, SEEK_SET);

			char keybuffer[512];
			fread(keybuffer, size, 1, FD);

			KeyInfo KeyInfo;
			KeyInfo.Key = strdup(keybuffer);

			fseek(FD, (Header.ValueTableOffset + Entry[i].ValueOffset), SEEK_SET);
			if (Entry[i].Format == sfo::SFO_UTF8)
			{
				char valuebuff[512];
				fread(valuebuff, Entry[i].Size, 1, FD);
				KeyInfo.Data.s = strdup(valuebuff);
				KeyInfo.Type = sfo::SFO_UTF8;
			}
			else if (Entry[i].Format == sfo::SFO_UINT)
			{
				uint64_t value = 0;
				fread(&value, sizeof(uint32_t), 1, FD);
				KeyInfo.Data.i = value;
				KeyInfo.Type = sfo::SFO_UINT;
			}

			fseek(FD, pos + size, SEEK_SET);
			Data.push_back(KeyInfo);
		}

		fclose(FD);
		Entry.clear();
		
		if (FixJailedState)
			OrbisSystemWrapper::Jail();

		return true;
	}

	void sfo_parser::ParseCurrentProcessSFO()
	{
		auto dirs = OrbisFileSystem::GetDirectoryEntries("/mnt/sandbox/pfsmnt/", 7);
		for (auto dir : dirs)
		{
			auto ID = dir.FileName.substr(dir.FileName.size() - 5, 5);

			if (strcasecmp(ID.c_str(), "-app0") != 0)
			{
				continue;
			}

			char path[PATH_MAX];
			sprintf(path, "/system_data/priv/appmeta/%s/param.sfo", dir.FileName.substr(0, 9).c_str());

			if (!this->ParseSFO(true, path))
			{
				MessageHandler::KernelPrintOut("failed to parse sfo");
			}
		}
	}

	const char* sfo_parser::GetKeySTR(const char* Key)
	{
		for (int i = 0; i < Data.size(); i++)
		{
			if (strcasecmp(Data[i].Key, Key) == 0)
			{
				return Data[i].Data.s;
			}
		}

		return nullptr;
	}

	uint64_t	sfo_parser::GetKeyUINT(const char* Key)
	{
		for (int i = 0; i < Data.size(); i++)
		{
			if (strcasecmp(Data[i].Key, Key) == 0)
			{
				return Data[i].Data.i;
			}
		}

		return -1;
	}
}
#endif