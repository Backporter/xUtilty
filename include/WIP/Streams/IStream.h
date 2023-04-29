#pragma once

#include "../../MessageHandler.h"

#include <stdint.h>
#include <sys\stat.h>
#include <memory>
#include <errno.h>
#include <exception>
#include <fcntl.h>
#include <errno.h>

#if __ORBIS__
#include <kernel.h>
#include <unistd.h>
#elif __OPENORBIS__
#include <orbis/libkernel.h>
#include <unistd.h>
#endif

namespace 
{
	class IStream
	{
	public:
		static constexpr size_t IO_STREAM_DEFAULT_SIZE = 128;
		using handle_t = int;
		using stat_t = struct stat;

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
		IStream(int64_t a_streamLength, int64_t	a_streamCapacity, int64_t a_streamPosition, int32_t a_streamFlags) : m_streamLength(a_streamLength), m_streamCapacity(a_streamCapacity), m_streamPosition(a_streamPosition), m_streamFlags(a_streamFlags) { }
		
		void InitializeIStream(int64_t a_streamLength, int64_t a_streamCapacity, int64_t a_streamPosition, int32_t a_streamFlags)
		{
			m_streamLength		= a_streamLength;
			m_streamCapacity	= a_streamCapacity;
			m_streamPosition	= a_streamPosition;
			m_streamFlags		= a_streamFlags;
		}

		virtual void	Dispose() = 0;
		virtual int64_t	Read(void*, size_t) = 0;
		virtual void	Write(void*, size_t) = 0;
		virtual int64_t	Seek(int64_t, int) = 0;
		virtual void	Flush() = 0;
	public:
		int64_t GetStreamLength()   { return m_streamLength;		   }
		int64_t	GetStreamCapacity() { return m_streamCapacity;		   }
		int64_t GetStreamPosition() { return m_streamPosition;		   }
		int32_t GetStreamFlags()    { return m_streamFlags;			   }

		bool	NotT(int a)			{ return (m_streamFlags & a) == 0; }
		bool	IsT(int a)		    { return (m_streamFlags & a) == a; }
		void	SetT(int a)			{ m_streamFlags |= a;              }
		void	RemoveT(int a)		{ m_streamFlags &= ~a;			   }
	protected:
		int64_t m_streamLength;
		int64_t	m_streamCapacity;
		int64_t m_streamPosition;
		int32_t m_streamFlags;
	};
}