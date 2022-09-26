#pragma once

#include <stdio.h>
#include <stdint.h>
#include <vector>
#include <string>
#include <cstring>

namespace OrbisFileSystem
{
	enum FileSystemPathType
	{
		// /app0/
		App		 = 0,

		// /download0/
		Download = 1,
		
		// /usb_0/
		USB0	 = 2,
		
		// /usb_1/
		USB1	 = 3,
		
		// /usb_2/
		USB2	 = 4,
		
		// /usb_3/
		USB3	 = 5,
		
		// /usb_4/
		USB4	 = 6,
		
		// /usb_5/
		USB5	 = 7,
		
		// /usb_6/
		USB6	 = 8,
		
		// /usb_7/
		USB7	 = 9,
		
		// ""
		None	 = 10,
		
		// /data/
		Data	 = 11,
		
		// 192.XXX.XXX.X
		Net		 = 12,

		// /savedata0/
		SaveData0 = 13,

		// /savedata1/
		SaveData1 = 14,
		
		// /savedata2/
		SaveData2 = 15,
		
		// /savedata3/
		SaveData3 = 16,
		
		// /savedata4/
		SaveData4 = 17,
		
		// /savedata5/
		SaveData5 = 18,
		
		// /savedata6/
		SaveData6 = 19,
		
		// /savedata7/
		SaveData7 = 20,
		
		// /savedata8/
		SaveData8 = 21,

		// /savedata9/
		SaveData9 = 22,
		
		// /savedata10/
		SaveData10 = 23,
		
		// /savedata11/
		SaveData11 = 24,
		
		// /savedata12/
		SaveData12 = 25,
		
		// /savedata13/
		SaveData13 = 26,
		
		// /savedata14/
		SaveData14 = 27,
		
		// /savedata15/
		SaveData15 = 28,
		
		// /savedata16/
		SaveData16 = 29,
		
		//
		System   = 30,

		// /X/X/X/X
		Full	 = 31,

		// /_mira/
		MIRA	 = 32,

		// romfs:/
		NX_FS    = 33,
	};

	struct DirectoryEntry
	{
		std::string FileName;
		uint32_t FileType;
		uint32_t Fileno;

		DirectoryEntry(std::string _FileName, uint32_t _FileType, uint32_t _Fileno)
		{
			FileName = _FileName;
			FileType = _FileType;
			Fileno	 = _Fileno;
		}
	};

	// returns a vector list containing DirectoryEntry structures, these will contain all found *DIRECTORY* entries
	// WARNING: for any non jail'ed paths you need to specify PathType as USB(2) OR SYSTEM(7) - this will unail the prcoess, grab the entries, rejail and return the list
	// EX, auto vec = GetDirectoryEntries("/app0/data/example/");
	std::vector<DirectoryEntry> GetDirectoryEntries(const char* path, int PathType, bool combine = false);

	// returns a vector list containing DirectoryEntry structures, these will contain all found *FILE* entries
	// WARNING: for any non jail'ed paths you need to specify PathType as USB(2) OR SYSTEM(7) - this will unail the prcoess, grab the entries, rejail and return the list
	// EX, auto vec = GetDirectoryEntries("/app0/data/example/", ".wav", true, NULL);
	// EX, auto vec = GetDirectoryEntries("/app0/data/example/", nullptr, false, NULL);
	std::vector<DirectoryEntry> GetDirectoryFileEntries(const char* path, const char* extension, bool UseFilter, int PathType, bool combine = false);

	bool PathExists(int reltype, const char* relpath, bool IsDirectory);

	bool PathExists2(char* path, int reltype, const char* relpath, bool IsDirectory, bool create);

	// this will create the full system path
	bool CreateFullPath(char* buffer, int reltype, const char* Relpath, ...);

	// TLDR: Creates all the directory's from within a given path
	// This will split a given path into "chunks" and create each until the end, works with and without files attached at the end
	bool CreateDirectoryPath(char* path);

	// creates a string in buffer with the provided format
	size_t CreateString(char* buffer, const char* FMT, ...);

	// extra code, shouldn't be used unless you know what it does.
	void RD(const char* path, uint64_t offset, uint64_t DataLength, void* Data);
	void RDP(const char* path, void* pattern, size_t patternlength, int plusindex, uint64_t datalength, void* dst);
	void WD(const char* path, void* data, size_t datalen);
	void WriteFileToDisk(int reltype, const char* path, const void* data, size_t length);

	// void Write(char* Path, int PathType, void* buf, size_t bufflen);
	void Copy2(char* src, char* dst, int reltype);


	// 
	const char* GetFilenameFromPath(const char* path);
}
