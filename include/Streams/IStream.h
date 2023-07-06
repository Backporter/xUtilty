#pragma once

#include "../bitFlags.h"

// wchar_t_t
#include "../Types.h"

#include <sys/stat.h>
#include <stdint.h>
#include <cstdio>

namespace xUtilty
{
	class IStream : public BitFlags<uint64_t>
	{
	public:
		// std::tolower/std::toupper
		using to_t = char(char);

		// std::towlower/std::towupper
		using to_w_t = wchar_t_t(wchar_t_t);

		// Minimum amount of memory that should be allocated when using memory.
		static constexpr size_t IO_STREAM_DEFAULT_SIZE = 128;

		enum kFlags
		{
			kFlagNone			= 0 << 0, // no flags
			
			// open flags:
			kFlagReadMode		= 1 << 1, // R
			kFlagWriteMode		= 1 << 2, // W
			kFlagReadWriteMode  = 1 << 3, // RW
			
			kFlagDirectory		= 1 << 4,
			kFlagCreate			= 1 << 5,
			kFlagTruncate		= 1 << 6,
			kFlagAppend			= 1 << 7,
			kFlagDirect			= 1 << 8,

			// property flags:
			kFlagCanRead		= 1 << 9, // opened with read perms(if write is also set, stream is R+W)
			kFlagCanWrite		= 1 << 10, // open with write perms(if read is also set, stream is R+W)
			kFlagCanSeek		= 1 << 11, // properly opened.
			
			// internal flags:
			kFlagIsManaged		= 1 << 12, // stream is externally managed
			kFlagIsDisposed		= 1 << 14, // stream has be closed and destroyed and cannot be R/W to.
		};

	public:
		IStream(int64_t a_streamLength, int64_t	a_streamCapacity, int64_t a_streamPosition, int32_t a_streamFlags);
		virtual ~IStream() = default;

		//
		void InitializeIStream(int64_t a_streamLength, int64_t a_streamCapacity, int64_t a_streamPosition, int32_t a_streamFlags);

		// add
		virtual void	Dispose() = 0;
		virtual int64_t	Read(void*, size_t) = 0;
		virtual void	Write(void*, size_t) = 0;
		virtual int64_t	Seek(int64_t, int) = 0;
		virtual void	Flush() = 0;
		virtual void	Truncate(int64_t) = 0;
		

		// addons
		size_t	 ReadString(char* dst, to_t* func = nullptr, size_t len = 512, char endl = '\n', char endl2 = '\r');
		size_t	 ReadStringW(wchar_t_t* dst, to_w_t* func = nullptr, size_t len = 512 * 2, wchar_t_t endl = (wchar_t_t)L'\n', wchar_t_t endl2 = (wchar_t_t)L'\r');

		// Get
		inline int64_t  GetStreamLength()   { return m_streamLength;		}
		inline int64_t  GetStreamCapacity() { return m_streamCapacity;		}
		inline int64_t  GetStreamPosition() { return m_streamPosition;		}
		inline uint64_t GetStreamFlags()    { return GetFlags();			}
	protected:
		int64_t m_streamLength;
		int64_t	m_streamCapacity;
		int64_t m_streamPosition;
	};
}