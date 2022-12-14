#pragma once

#include <stdint.h>
#include <vector>
#include "CryptoHandler.h"

struct OffsetDataBase
{
	enum Type : uint8_t { V1 = 0, V2 = 1 };

	struct Header
	{
		uint32_t Magic;			// 0x4F534442 | OSDB
		uint16_t Version;		// 1
		uint8_t  Type;			// 0/1/X+
		uint32_t Count;			// ???
		char Platform[5];		// ORBIS
	} __attribute__((__packed__));

	struct InternalOffsetStructure
	{
		char		ID[0x28];			//
		uint64_t	offset;				//
	} __attribute__((__packed__));

	struct InternalOffsetStructureV2
	{
	public:
		uint32_t ID;        // CRC32 of the ID string(!!)
		uint64_t offset;
	};
};

namespace OrbisOffsetManger
{
	enum Flags
	{
		kNone = 0,
		kV1 = 1 << 0,
		kV2 = 1 << 1,
	};

	inline bool g_useDB = false;
	
	class OffsetManger
	{
	public:
		using DBType = OffsetDataBase::Type;
		using V1 = OffsetDataBase::InternalOffsetStructure;
		using V2 = OffsetDataBase::InternalOffsetStructureV2;
		using unionT = union { V1 v1; V2 v2; };

		OffsetManger() : m_path(NULL), m_flags(0), m_fd(NULL), m_header{ 0 } { Initialize(); }
		~OffsetManger() { if (m_path) { free((void*)m_path); } }

		void	  Initialize();
		bool	  Parse();
		uintptr_t GetOffset(uint32_t a_crc32);
		uintptr_t GetOffset(const char* a_str) { return GetOffset(CryptoHandler::GetCRC32(a_str, 0)); }
		
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
	public:
		char*													m_path{ 0 };
		uint32_t												m_flags{ 0 };
		int														m_fd{ 0 };	// temp FD, shouldn't ever stay open
		OffsetDataBase::Header									m_header{ 0 };
		std::vector<V1>											m_entiresv1;
		std::vector<V2>											m_entiresv2;
	};

}