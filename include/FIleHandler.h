#pragma once
#include "SafeTypes.h"

#if defined(__ORBIS__)
#include <kernel.h>
#include <rtc.h>
#elif defined(__OPENORBIS__)
#include <orbis/libkernel.h>
#include <dirent.h>
#endif

#include <string>
#include <wchar.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

namespace FIleHandler
{
	// class that automates many of the main file IO stuff into simple calls
	class FIleHandler
	{
	public:
		// File path, Full if abs is true, /app0/XXX if abs is false, Type indicates if the Readstring function should read in a wide string form or not, only effects Readstring, everything else is un-effected
		FIleHandler(char* relpath, int Type, int flags);
		FIleHandler(char* relpath, int Type, int flags, bool abs);
		
		~FIleHandler();
		
		// returns if the raw file descriptor is vaild
		bool	IsDescriptorVailed();

		// returns if the stream pos is < total file size
		bool	IsStreamVaild();

		// Rewinds the stream pos by count
		// count should be a positive value, it gets converted into a negitive value within the function
		bool	Rewind(uint64_t count);

		// Jumps past the amount of bytes defined by count
		bool	Skip(uint64_t count);

		// returns the amount of bytes read
		uint64_t ReadBytes(void* dst, size_t dstLength);

		// returns the amount of bytes written
		uint64_t WriteBytes(void* srcbuf, size_t srcLength);

		// WINDOWS and UNIX new line ID's are slightly differnt, \r\n on windows and \n on UNIX, this needs to be accounted for 
		//_FN is equal to std::tolower/toupper, etc
		// returns read string length(not including terminator), sets dst to a properly termanated string
		uint64_t ReadString(char* dst, int(*_fn)(int) = NULL, uint64_t len = 512, char endl = '\n', char endl2 = '\r');

		// WINDOWS and UNIX new line ID's are slightly differnt, \r\n on windows and \n on UNIX, this needs to be accounted for 
		// returns read string length(not including terminator), sets dst to a properly termanated string
		uint64_t ReadStringW(wchar_t_t* dst, int(*_fn)(int) = NULL, uint64_t len = 512 * 2, char endl = '\n', char endl2 = '\r');


		// WINDOWS and UNIX new line ID's are slightly differnt, \r\n on windows and \n on UNIX, this needs to be accounted for 
		// returns written string length(not including terminator), and writes a string to the file stream
		// uint64_t WriteString(char* src, uint64_t len , char endl = '\0');


		struct stat* GetFileStats();

	private:
		bool		IsCopy;
		char*	    FilePath;

		int		    FileDescriptor;
		int		    FileType;
		int		    RET;

		struct stat FileStats;
		uint64_t	FDPos;
	};

	// string format
	enum FileType
	{
		UTF8,
		UTF16,
	};

}