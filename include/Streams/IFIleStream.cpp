//
#include "../MessageHandler.h"
#include "../SystemWrapper.h"

//
#include "IFIleStream.h"

namespace xUtilty
{
	IFileStream::IFileStream() : IStream(0, 0, 0, 0)
	{}

	IFileStream::IFileStream(int a_handle, int32_t a_flags) : IStream(0, 0, 0, a_flags) 
	{ 
		Open(a_handle, a_flags); 
	}

	IFileStream::IFileStream(char* a_path, int32_t a_flags) : IStream(0, 0, 0, a_flags) 
	{ 
		Open(a_path, a_flags); 
	}

	IFileStream::~IFileStream()
	{
		if (m_handle > 0)
		{
			SystemWrapper::close(m_handle);
		}
	}

	bool IFileStream::Open(int a_handle, int32_t a_flags)
	{
		InitializeIStream(0, 0, 0, a_flags);

		struct stat s_stats;

		if (a_handle > 0)
		{
			m_handle = a_handle;

			if (!SystemWrapper::fstat(m_handle, &s_stats))
			{
				m_streamLength	 = 0;
				m_streamCapacity = s_stats.st_size;
			}

			if ((m_streamPosition = SystemWrapper::lseek(m_handle, 0, 1)) < 0)
			{
				m_streamPosition = 0;
			}

			return true;
		}

		KernelPrintOut("invaled handle(0x%lx) passed to IFileStream::IFileStream(handle_t,int32_t)", a_handle);
		return false;
	}

	bool IFileStream::Open(const char* a_path, int32_t a_flags)
	{
		InitializeIStream(0, 0, 0, a_flags);

		int s_openflags { 0 };
		struct stat	s_stats;
		
		if (TestFlags(IStream::kFlags::kFlagReadMode))
		{
			// O_RDONLY
			s_openflags = 0x0000;
		}

		if (TestFlags(IStream::kFlags::kFlagWriteMode))
		{
			// O_WRONLY
			s_openflags = 0x0001;
		}

		if (TestFlags(IStream::kFlags::kFlagReadWriteMode))
		{
			// O_RDWR
			s_openflags = 0x0002;
		}

		if (TestFlags(IStream::kFlags::kFlagDirectory))
		{
			// O_DIRECTORY
			s_openflags |= 0x00020000;
		}

		if (TestFlags(IStream::kFlags::kFlagCreate))
		{
			// O_CREAT
			s_openflags |= 0x0200;
		}

		if (TestFlags(IStream::kFlags::kFlagTruncate))
		{
			// O_TRUNC
			s_openflags |= 0x0400;
		}

		if (TestFlags(IStream::kFlags::kFlagAppend))
		{
			// O_APPEND
			s_openflags |= 0x0008;
		}

		if (TestFlags(IStream::kFlags::kFlagDirect))
		{
			// O_DIRECT
			s_openflags |= 0x00010000;
		}

		m_handle = SystemWrapper::open(a_path, s_openflags, 0000777);
		if (m_handle > 0)
		{
			if (!SystemWrapper::fstat(m_handle, &s_stats))
			{
				m_streamLength	 = 0;
				m_streamCapacity = s_stats.st_size;
			}

			return true;
		}

		return false;
	}

	void	IFileStream::Dispose()
	{
		if (TestFlags(IStream::kFlags::kFlagIsDisposed))
			return;

		if (m_handle > 0)
		{
			SystemWrapper::close(m_handle);
			SetFlags(IStream::kFlags::kFlagIsDisposed);
		}
	}

	int64_t	IFileStream::Read(void* a_dst, size_t a_size)
	{
		if (m_handle > 0 && a_dst && a_size)
		{
			if (SystemWrapper::read(m_handle, a_dst, a_size) == a_size)
			{
				m_streamPosition += a_size;
				return a_size;
			}
			else
			{
				PRINT_POS;
				return -1;
			}
		}

		return 0;
	}
	
	void	IFileStream::Write(void* a_buff, size_t a_len)
	{
		if (m_handle > 0 && a_buff && a_len)
		{
			if (SystemWrapper::write(m_handle, a_buff, a_len) != a_len)
			{
				PRINT_POS;
			}
			else
			{
				m_streamPosition += a_len;
				m_streamLength += a_len;
			}
		}
	}

	int64_t	IFileStream::Seek(int64_t a_offset, int a_seekType)
	{
		int64_t s_ret = 0;

		if (m_handle > 0)
		{
			if ((s_ret = SystemWrapper::lseek(m_handle, a_offset, a_seekType)) != -1)
			{
				m_streamPosition = a_offset;
				return s_ret;
			}
			else
			{
				PRINT_POS;
				return -1;
			}

		}

		return 0;
	}

	void	IFileStream::Flush()
	{
		if (SystemWrapper::fsync(m_handle) < 0)
			PRINT_POS;
	}

	void	IFileStream::Truncate(int64_t a_size)
	{
		int ret = 0;

		if (m_handle > 0)
		{
			if ((ret = SystemWrapper::ftruncate(m_handle, a_size)) != 0)
			{
				PRINT_POS;
			}
			else
			{
				m_streamPosition = a_size;
				m_streamCapacity = a_size;
			}
		}
	}
}