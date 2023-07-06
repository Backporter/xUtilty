#if defined(__ORBIS__) || defined(__OPENORBIS__) || defined(__x86_64__) || defined(_M_X64)
#include "../include/OffsertManger.h"

//
#include "../include/MessageHandler.h"
#include "../include/Streams/IFIleStream.h"

#include <assert.h>

namespace xUtilty
{
	OffsetManger::OffsetManger()
	{
		//
	}

	OffsetManger::~OffsetManger()
	{
		//
		PRINT_POS_N;
	}

	bool OffsetManger::Initialize(int MAJOR, int MINOR, int BUILD, int SUB)
	{
		if (g_OffsetMangerInit)
			return true;

		g_OffsetMangerInit = true;

		// allocate a new memory stream;
		if (!_MemoryStream)
		{
			_MemoryStream = new IMemoryStream();
		}

		//
		uint16_t skip;
		char buffer[260] { 0 };
		IFileStream filestream;

		// create the path
		sprintf(buffer, "/_mira/version-%d-%d-%d-%d.bin", MAJOR, MINOR, BUILD, SUB);

		// open the file and grow the MemoryStream to fit it
		if (filestream.Open(buffer, IStream::kFlags::kFlagReadMode))
		{
			//
			size_t filesize = filestream.GetStreamCapacity();

			// setup the memory stream with the size of the file so it can be read in full
			_MemoryStream->Initialize(filesize);
			
			// zero-fill
			memset(_MemoryStream->ToArray(), 0, filesize);

			// read the whole file into memory.
			filestream.Read((void*)_MemoryStream->ToArray(), filesize);
			
			// read the header and check it
			if (_MemoryStream->Read(&_Header, sizeof(Header_t)) > 0)
			{
				//
				_MemoryStream->Read(&skip, 2);
				if (skip != 0x0A && skip != 0x0A0D)
					_MemoryStream->Seek(-2, SEEK_CUR);

				// confirm version match.
				if (_Header.AppVersion[0] != MAJOR || _Header.AppVersion[1] != MINOR || _Header.AppVersion[2] != BUILD || _Header.AppVersion[3] != SUB)
				{
					KernelPrintOut("version mismatch in %s [this is fatal on anything greater then %d.%d.%d.%d]", buffer, MAJOR, MINOR, BUILD, SUB);
					return false;
				}

				// check the version and parse it with the proper logic.
				if (_Header.Version == v1)
				{
					ParseV1(_MemoryStream);
				}
				else if (_Header.Version == v2)
				{
					ParseV2(_MemoryStream);
				}

				return true;
			}
			else
			{
				//
				KernelPrintOut("Failed to read %s [this is fatal on anything greater then %d.%d.%d.%d]", buffer, MAJOR, MINOR, BUILD, SUB);
				return false;

			}
		}
		else
		{
			//
			KernelPrintOut("Failed to open %s [this is fatal on anything greater then %d.%d.%d.%d]", buffer, MAJOR, MINOR, BUILD, SUB);
			return false;
		}
	}
	
	uintptr_t OffsetManger::GetOffsetFromName(const char* a_functionName)
	{
		if (_Header.Version == v1)
		{
			for (auto entry : _offsets)
			{
				if (!strcasecmp(a_functionName, entry.v1.name))
				{
					return entry.v1.offset;
				}
			}
		}

		return 0;
	}

	const char* OffsetManger::GetNameFromOffset(uintptr_t a_offset)
	{
		if (_Header.Version == v1)
		{
			for (auto& entry : _offsets)
			{
				if (entry.v1.offset == a_offset)
				{
					return entry.v1.name;
				}
			}
		}

		return 0;
	}

	uintptr_t OffsetManger::GetOffsetFromID(int32_t a_id)
	{
		for (auto entry : _offsets)
		{
			if (entry.v2.id == a_id)
			{
				return entry.v2.offset;
			}
		}

		return 0;
	}

	int32_t	OffsetManger::GetIDFromOffset(uintptr_t a_offset)
	{
		for (auto entry : _offsets)
		{
			if (entry.v2.offset == a_offset)
			{
				return entry.v2.id;
			}
		}

		return 0;
	}

	bool OffsetManger::ParseV1(IStream* const a_filestream)
	{
		char buffer[512];
	
		while (a_filestream->GetStreamPosition() < a_filestream->GetStreamCapacity())
		{
			Entries_t entry {  };
			auto memsteam = static_cast<IMemoryStream* const>(a_filestream);

			// string
			entry.v1.name = (char*)(&memsteam->ToArray()[memsteam->GetStreamPosition()]);

			// both debug and advances the stream position.
			memsteam->ReadString(buffer, nullptr, sizeof(buffer), '\0', '\n');
		
			// read the offset and advances the stream position.
			memsteam->Read(&entry.v1.offset, 8);

			// push onto vector
			_offsets.push_back(entry);
		}

		return _offsets.size() > 0;
	}

	bool OffsetManger::ParseV2(IStream* const a_filestream)
	{
		while (a_filestream->GetStreamPosition() < a_filestream->GetStreamCapacity())
		{
			Entries_t entry {  };
			if (a_filestream->Read(&entry.v2, sizeof(Entry2_t) != sizeof(Entry2_t)))
			{
				break;
			}

			_offsets.push_back(entry);
		}

		return _offsets.size() > 0;
	}

	__attribute__((constructor)) inline void INIT()
	{
		OffsetManger::GetSingleton().Initialize(GAME_BUILD_MINOR, GAME_BUILD_MAJOR, GAME_BUILD_BUILD, GAME_BUILD_SUB);
	}
}
#endif