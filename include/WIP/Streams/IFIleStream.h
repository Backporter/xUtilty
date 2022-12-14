#pragma once

#include "IStream.h"

namespace 
{
	class IFileStream : public IStream
	{
	public:
		IFileStream() : IStream(0, 0, 0, 0) { }
		IFileStream(handle_t a_handle, int32_t a_flags) : IStream(0, 0, 0, a_flags)		{ Open(a_handle, a_flags); }
		IFileStream(char* a_path, int32_t a_flags) : IStream(0, 0, 0, a_flags)			{ Open(a_path, a_flags); }

		bool	Open(handle_t a_handle, int32_t a_flags)
		{
			InitializeIStream(0, 0, 0, a_flags);

			stat_t s_stats;

			if (a_handle > 0)
			{
				m_handle = a_handle;

				if (!fstat(m_handle, &s_stats))
				{
					m_streamLength = s_stats.st_size;
					m_streamCapacity = s_stats.st_size;
				}

				if ((m_streamPosition = lseek(m_handle, 0, SEEK_CUR)) < 0)
					m_streamPosition = 0;

				return true;
			}

			MessageHandler::KernelPrintOut("invaled handle(0x%lx) passed to IFileStream::IFileStream(handle_t,int32_t)", a_handle);
			return false;
		}

		bool	Open(const char* a_path, int32_t a_flags)
		{
			InitializeIStream(0, 0, 0, a_flags);

			stat_t	s_stats;
			int		s_openflags = 0;

			if (IsT(IStream::kFlags::kFlagReadMode))
			{
				s_openflags = O_RDONLY;
				PRINT_POS;
			}

			if (IsT(IStream::kFlags::kFlagWriteMode))
			{
				s_openflags = O_WRONLY;
				PRINT_POS;
			}

			if (IsT(IStream::kFlags::kFlagReadWriteMode))
			{
				s_openflags = O_RDWR;
				PRINT_POS;
			}

			if (IsT(IStream::kFlags::kFlagDirectory))
			{
				s_openflags |= O_DIRECTORY;
				PRINT_POS;
			}

			if (IsT(IStream::kFlags::kFlagCreate))
			{
				s_openflags |= O_CREAT;
				PRINT_POS;
			}

			if (IsT(IStream::kFlags::kFlagTruncate))
			{
				s_openflags |= O_TRUNC;
				PRINT_POS;
			}

			if (IsT(IStream::kFlags::kFlagAppend))
			{
				s_openflags |= O_APPEND;
				PRINT_POS;
			}

			if (IsT(IStream::kFlags::kFlagDirect))
			{
				s_openflags |= O_DIRECT;
				PRINT_POS;
			}

			m_handle = open(a_path, s_openflags, 0000777);
			if (m_handle > 0)
			{
				if (!fstat(m_handle, &s_stats))
				{
					m_streamLength = s_stats.st_size;
					m_streamCapacity = s_stats.st_size;
				}

				return true;
			}

			MessageHandler::KernelPrintOut("[0x%lx] [%s] [%s] [0x%lx] [0000777]", m_handle, a_path, strerror(errno), s_openflags);
			return false;
		}

		bool	IsVaild() 
		{ 
			return (m_handle > 0); 
		}
		
		void	Dispose() override 
		{ 
			if (IsT(IStream::kFlags::kFlagIsDisposed))
				return;

			if (m_handle > 0) 
			{ 
				close(m_handle); 
			} 
		}

		int64_t	Read(void* a_dst, size_t a_size) override
		{
			if (m_handle > 0 && a_dst && a_size)
			{
				if (read(m_handle, a_dst, a_size) == a_size)
					return a_size;
				else
				{
					MessageHandler::KernelPrintOut("IFileStream::Read(void*,size_t) failed with (%s)", strerror(errno));
					return -1;
				}
			}

			return 0;
		}

		void	Write(void* a_buff, size_t a_len) override
		{
			if (m_handle > 0 && a_buff && a_len)
			{
				if (write(m_handle, a_buff, a_len) != a_len)
					MessageHandler::KernelPrintOut("IFileStream::Write(void*,size_t) failed with (%s)", strerror(errno));
			}
		}

		int64_t	Seek(int64_t a_offset, int a_seekType) override
		{
			int64_t s_ret = 0;

			if (m_handle > 0)
			{
				if ((s_ret = lseek(m_handle, a_offset, a_seekType)) > 0)
					return s_ret;
				else
				{
					MessageHandler::KernelPrintOut("IFileStream::Seek(int64_t,int) failed with (%s)", strerror(errno));
					return -1;
				}
				
			}

			return 0;
		}

		void	Flush() override 
		{
			if (fsync(m_handle) < 0)
				MessageHandler::KernelPrintOut("IFileStream::Flush() failed with (%s)", strerror(errno));
		}

		void Truncate(int64_t a_size)
		{
			int ret = 0;

			if (m_handle > 0)
			{
				if (ret = ftruncate(m_handle, a_size) != 0)
				{
					MessageHandler::KernelPrintOut("ftruncate failed with %d", ret);
				}

			}
		}
	protected:
		handle_t m_handle{ -1 };
	};
}