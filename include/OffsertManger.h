#pragma once

#if defined(__ORBIS__) || defined(__OPENORBIS__) || defined(__x86_64__) || defined(_M_X64)
#include "../include/Streams/IMemoryStream.h"

#if __clang__
#include <sys/mman.h>
#endif

#include <stdint.h>
#include <atomic>
#include <vector>

namespace xUtilty
{
	inline static bool g_OffsetMangerInit = false;

	class OffsetManger
	{
	public:
		enum Version
		{
			None,
			v1,
			v2,
			v3,
			v4,
			v5,
		};

		struct Header_t
		{
			int32_t Magic;
			int32_t Version;
			int32_t AppVersion[4]; // ex, 1 [.] 5 [.] 97 [.] 0
		};
		static_assert(sizeof(Header_t) == 0x18);

		// == v1
		struct Entry1_t
		{
			char*		name { 0 };
			uintptr_t	offset { 0 };
		};
		static_assert(sizeof(Entry1_t) == 0x10);

		// >= v2
		struct Entry2_t
		{
			int32_t	  id { 0 };
			int32_t	  flags { 0 };
			uintptr_t offset { 0 };
		};
		static_assert(sizeof(Entry2_t) == 0x10);
		
		union Entries_t
		{
			Entry1_t v1;
			Entry2_t v2;
		};
		static_assert(sizeof(Entries_t) == 0x10);
	public:
		OffsetManger();
		~OffsetManger();

		// singleton
		static OffsetManger& GetSingleton()
		{
			static OffsetManger singleton;
			return singleton;
		}

		// open and parse the DB with the Game version;
		bool		Initialize(int MAJOR, int MINOR, int BUILD, int SUB);
		
		// v1
		uintptr_t	GetOffsetFromName(const char* a_functionName);
		const char* GetNameFromOffset(uintptr_t a_offset);

		// v2
		uintptr_t	GetOffsetFromID(int32_t a_id);
		int32_t		GetIDFromOffset(uintptr_t a_offset);
	public:
		IMemoryStream*			_MemoryStream;
		Header_t				_Header;
		std::vector<Entries_t>	_offsets;
	protected:
		bool ParseV1(IStream* const a_filestream);
		bool ParseV2(IStream* const a_filestream);
	};

	// OffsetManger needs to be Initializeed before execution so we need to mark it as a constructor.
	__attribute__((constructor)) inline void INIT();
}
#endif