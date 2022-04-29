#pragma once

#include <stdint.h>
#include <vector>

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
		OffsetManger() {};
		virtual ~OffsetManger() {};

		// this will parse the offset database that matches the SFO's TITILE_ID and APP_VER key
		virtual void Initialize();

		// on the fly parsing
		virtual uint64_t GetOffset(const char* ID);

		static OffsetManger* GetSingleton()
		{
			static OffsetManger OffsetManger;
			return &OffsetManger;
		}

	private:
		int RET;

		// should equal /app0/Data/OOM/XXXXXXXXX-XXXX.offset
		const char* OffsetFIle;
		int FD;
	};

}