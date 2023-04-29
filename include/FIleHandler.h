#pragma once

// wchar_t_t
#include "../include/SafeTypes.h"

// size_t
#include <cstdio>

// xxxxxxxxx_t
#include <stdint.h>

// open
#include <fcntl.h>

// stat
#include <sys\stat.h>

// read, write, etc
#include <unistd.h>

// strerror, 
#include <string>

// errno
#include <errno.h>

// free
#include <stdlib.h>

namespace xUtilty
{
	namespace FileSystem
	{
		enum kTypeFlags
		{
			kNone = 0,

			// open flags
			kIsConstPath = 1 << 0,
			kIsAbsolutePath = 1 << 1,
			kIsRelativePath = 1 << 2,
			kIsMemoryMappedFile = 1 << 3,

			// type flags
			kUTF8 = 1 << 4,
			kUTF16 = 1 << 5,
		};

		/* this class will be removed in future versions, wonce i finish WIP/Streams/IFileStream */
		class FIleHandler
		{
		public:
			using stat_t = struct stat;
			using toT_t = char(char);
		public:
			FIleHandler(char* a_path, int a_flags, int a_openflags);
			~FIleHandler();

			//
			bool		 IsDescriptorVailed();
			bool		 IsStreamVaild();
			bool		 Rewind(int64_t count);
			bool		 Skip(uint64_t count);
			uint64_t	 ReadBytes(void* dst, size_t dstdength);
			uint64_t	 WriteBytes(void* srcbuf, size_t srcLength);
			uint64_t	 ReadString(char* dst, int(*_fn)(int) = NULL, uint64_t len = 512, char endl = '\n', char endl2 = '\r');
			uint64_t	 ReadStringW(wchar_t_t* dst, int(*_fn)(int) = NULL, uint64_t len = 512 * 2, char endl = '\n', char endl2 = '\r');
			stat_t&		 GetFileStats() { return m_filestats; }
			int			 GetLastReturnValue() { return m_ret; }
		protected:
			uint64_t	m_flags;
			char*	    m_filepath;
			int		    m_fd;
			int		    m_ret;
			stat_t		m_filestats{ 0 };
			int64_t		m_fdpos;
		};
	}
}