#pragma once

#if defined (__ORBIS__) || defined(__OPENORBIS__)
#include "MessageHandler.h"

#include <stdint.h>
#include <stdio.h>
#include <string>
#include <vector>

namespace OrbisSFOHandler
{
	static uint32_t SFOMagic = 0x46535000;
	static uint32_t SFOVersion = 0x101;
	static uint32_t SFOHeaderSize = 0x14;
	static uint32_t SFOEntrySize = 0x10;

	struct sfo
	{
		enum
		{
			SFO_UTF8 = 516,
			SFO_UINT = 1028,
		};

		struct Header
		{
			uint32_t Magic;					// 0x46535000(LE) 0x505346(BE)
			uint32_t Version;				// 0x101(LE) 0x1010000(BE)
			uint32_t KeyTableOffset;		// ?? ??
			uint32_t ValueTableOffset;		// ?? ??
			uint32_t EntryCount;			// ?? ??
		};
		static_assert(sizeof(Header) == 0x14, "SFO Header Size Mismatch");

		struct Entry
		{
			uint16_t KeyOffset;				// ??
			uint16_t Format;				// ??
			uint32_t Size;					// ??
			uint32_t MaxSize;				// ??
			uint32_t ValueOffset;			// ??
		};
		static_assert(sizeof(Entry) == 0x10, "SFO Entry Size Mismatch");
	};

	class sfo_parser
	{
	public:
		sfo_parser();
		virtual ~sfo_parser();

		// this will parse the info of whatever app is found to be mounted in /mnt/sandbox/pfsmnt/ with the prefix of "-app0", meaning this could fail if you have more than one app open
		virtual void ParseCurrentProcessSFO();

		// this will parse a SFO based of a full path, if you give a full path that's outside of /app0/ you need to make sure the first are is TRUE
		virtual bool ParseSFO(bool FixJailedState, char* Path);

		// singleton implementation
		static sfo_parser* GetSingleton()
		{
			static sfo_parser parser;
			return &parser;
		}

		// returns a the value of a key in string form(this will only work with string based keys, int keys will require GetKeyUINT to get called, not this)
		const char* GetKeySTR(const char* Key);

		// same as GetKeySTR but returns a int key(casted from a int, to a uint64 to match the internal structure
		uint64_t	GetKeyUINT(const char* Key);

		struct KeyInfo
		{
			union Data
			{
				uint64_t    i;
				const char* s;
			};

			int64_t		Type;
			Data		Data;
			const char* Key;
		};

	private:
		std::vector<KeyInfo> Data;
	};

	// sfo_parser needs to be Initializeed before execution so we need to mark it as a constructor.
	__attribute__((constructor)) inline void INIT();
}
#endif