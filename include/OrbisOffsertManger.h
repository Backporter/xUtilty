#pragma once

#include <stdint.h>
#include <vector>
#include <cstdlib>
#include <string>
#include <wchar.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

struct OffsetDataBase
{
	struct Header
	{
		uint32_t Magic;			// 0x4F534442 | OSDB
		uint16_t Version;		// 1
		uint8_t  pad04;			// 0/1
		uint32_t Count;			// ???
		char Platform[5];		// ORBIS
	} __attribute__((__packed__));
	
	struct InternalOffsetStructure
	{
		char		ID[0x28];			//
		uint64_t	offset;				//
	} __attribute__((__packed__));
};

namespace OrbisOffsetManger
{
	class OffsetManger
	{
	public:
		OffsetManger() 
		{
			Initialize();
		}

		virtual ~OffsetManger() 
		{
			free((void*)this->OffsetFIle);
		}

		// this will parse the offset database that matches the SFO's TITILE_ID and APP_VER key
		virtual void Initialize();

		virtual bool Parse();

		// on the fly parsing
		virtual uint64_t  GetOffset(const char* ID);

		virtual uintptr_t _GetOffset(const char* ID);

		static const int   GetPlatformID()
		{
#if defined (__ORBIS__) || defined(__OPENORBIS__)
			return 0x4244534F;
#elif defined (__SWITCH__) || defined(__PLATFORM_NX__)
			return 0x4E584442;
#endif
		}

		static const char* GetPlatformStringID()
		{
#if defined (__ORBIS__) || defined(__OPENORBIS__)
			return "ORBIS";
#elif defined (__SWITCH__) || defined(__PLATFORM_NX__)
			return "NENDX";
#endif
		}

		static OffsetManger* GetSingleton()
		{
			static OffsetManger OffsetManger;
			return &OffsetManger;
		}

	private:
		const char* OffsetFIle;
		bool		IsSandboxPath;
		int			FD;
		int			RET;

		OffsetDataBase::Header									Header;
		std::vector<OffsetDataBase::InternalOffsetStructure>	Entries;
	};

}