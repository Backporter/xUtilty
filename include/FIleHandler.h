#pragma once

#include "SafeTypes.h"

#if defined(__ORBIS__)
#include <kernel.h>
#include <rtc.h>
#elif defined(__OPENORBIS__)
#include <orbis/libkernel.h>
#include <dirent.h>
#endif

#include <stdint.h>
#include <string>

namespace FIleHandler
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

	class FIleHandler
	{
	public:
		using stat_t = struct stat;
		using toT_t = char(char);
	public:
		/*
			a_flags - bitwise flags defined in kTypeFlags for class logic.
			a_openflags - bitwise flags, passed directly to open().
		*/
		FIleHandler(char* a_path, int a_flags, int a_openflags);
		~FIleHandler();
	public:
		bool		 IsDescriptorVailed();
		bool		 IsStreamVaild();
		bool		 Rewind(uint64_t count);
		bool		 Skip(uint64_t count);
		uint64_t	 ReadBytes(void* dst, size_t dstLength);
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
		uint64_t	m_fdpos;
	};
}