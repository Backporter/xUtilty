#pragma once

// xxxx_t
#include <stdint.h>

// size_t
#include <cstdio>

// std::string
#include <string>

// std::vector
#include <vector>

namespace xUtilty
{
	namespace FileSystem
	{
		enum
		{
			// "/app0/"
			App = 0,

			// "/download0/"
			Download = 1,

			// "/usb_0/"
			USB0 = 2,

			// "/usb_1/"
			USB1 = 3,

			// "/usb_2/"
			USB2 = 4,

			// "/usb_3/"
			USB3 = 5,

			// "/usb_4/"
			USB4 = 6,

			// "/usb_5/"
			USB5 = 7,

			// "/usb_6/"
			USB6 = 8,

			// "/usb_7/"
			USB7 = 9,

			// ""
			None = 10,

			// /data/
			Data = 11,

			// 192.XXX.XXX.X
			Net = 12,

			// "/savedata0/"
			SaveData0 = 13,

			// "/savedata1/"
			SaveData1 = 14,

			// "/savedata2/"
			SaveData2 = 15,

			// "/savedata3/"
			SaveData3 = 16,

			// "/savedata4/"
			SaveData4 = 17,

			// "/savedata5/"
			SaveData5 = 18,

			// "/savedata6/"
			SaveData6 = 19,

			// "/savedata7/"
			SaveData7 = 20,

			// "/savedata8/"
			SaveData8 = 21,

			// "/savedata9/"
			SaveData9 = 22,

			// "/savedata10/"
			SaveData10 = 23,

			// "/savedata11/"
			SaveData11 = 24,

			// "/savedata12/"
			SaveData12 = 25,

			// "/savedata13/"
			SaveData13 = 26,

			// "/savedata14/"
			SaveData14 = 27,

			// "/savedata15/"
			SaveData15 = 28,

			// "/savedata16/"
			SaveData16 = 29,

			// "/"
			System = 30,

			//
			Full = 31,

			// "/_mira/"
			MIRA = 32,
		};

		struct DirectoryEntry
		{
		public:
			DirectoryEntry(std::string _FileName, uint32_t _FileType, uint32_t _Fileno)
			{
				m_filename = _FileName;
				m_filetype = _FileType;
				m_fileno = _Fileno;
			}
		public:
			std::string m_filename;
			uint32_t	m_filetype;
			uint32_t	m_fileno;
		};

		std::vector<DirectoryEntry> GetDirectoryEntries(const char* path, const char* extension, bool UseFilter, int PathType, bool combine = false);

		bool		Mount(int rel);
		bool		Unmount(int rel);
		bool		PathExists(int, const char*, bool);
		bool		CreateFullPath(char* buffer, int reltype, const char* Relpath, ...);
		size_t		CreateString(char* buffer, const char* FMT, ...);
		bool		CreateDirectoryPath(char* path);
		const char* GetFilenameFromPath(const char* path);

		void		WriteFileToDisk(int reltype, const char* path, const void* data, size_t length);
		bool		Dump(const char* TypeName, void* base, int size);
	}

}