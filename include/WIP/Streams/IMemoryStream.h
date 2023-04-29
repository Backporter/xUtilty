#pragma once

#include "../../MessageHandler.h"
#include "IStream.h"

namespace
{
	class IMemoryStream : public IStream
	{
	public:
		IMemoryStream(bool a_init = true, size_t a_size = IStream::IO_STREAM_DEFAULT_SIZE) : IStream(a_size, a_size, 0, 0)
		{
			if (a_init)
			{
				Initialize(a_size);
			}
		}


		void Initialize(size_t a_size = IStream::IO_STREAM_DEFAULT_SIZE)
		{
			InitializeIStream(a_size, a_size, 0, 0);

			void* mem = malloc(m_streamLength);
			if (!mem)
			{
				xUtilty::Notify("malloc failed");
				return;
			}

			memset(mem, 0, m_streamLength);
			m_buffer = (int8_t*)mem;

			SetT(IStream::kFlags::kFlagCanRead);
			SetT(IStream::kFlags::kFlagCanWrite);
			SetT(IStream::kFlags::kFlagCanSeek);
		}
		
		// ~IMemoryStream() override  { Dispose(); }

		void	Dispose() override
		{
			if (!IsT(IStream::kFlags::kFlagIsManaged) && !IsT(IStream::kFlags::kFlagIsDisposed) && m_buffer)
			{
				free((void*)m_buffer);
				SetT(IStream::kFlags::kFlagIsDisposed);
			}
		}
	
		int64_t	Read(void* a_buffer, size_t a_bufflen) override
		{
			if (m_buffer)
			{
				if (!a_buffer || !a_bufflen)
				{
					xUtilty::KernelPrintOut("Invailed buffer passed into MemoryStream.. ignoring.");
				}
				else
				{
					if (m_streamPosition + a_bufflen > m_streamLength)
						Grow(m_streamLength + a_bufflen + IStream::IO_STREAM_DEFAULT_SIZE, m_streamLength);

					a_buffer = (void*)(&m_buffer[m_streamPosition]);
					m_streamPosition += a_bufflen;
				}

				return a_bufflen;
			}

			return 0;
		}

		void	Write(void* a_buffer, size_t a_bufflen) override
		{
			if (m_streamPosition + a_bufflen > m_streamLength)
			{
				if (!Grow(m_streamLength + a_bufflen + IStream::IO_STREAM_DEFAULT_SIZE, m_streamLength))
					return;
			}

			if (m_buffer)
			{
				if (!a_buffer || !a_bufflen)
				{
					xUtilty::Notify("Invaild buffer/size passed into IMemoryStream::Write(void*, size_t)");
				}
				else
				{
					memcpy(&m_buffer[m_streamPosition], a_buffer, a_bufflen);
					m_streamPosition += a_bufflen;

				}
			}
		}

		int64_t	Seek(int64_t a_offset, int a_seekType) override
		{
			if (m_streamPosition + a_offset > m_streamLength)
				Grow(m_streamLength + a_offset + IStream::IO_STREAM_DEFAULT_SIZE, m_streamLength);

			switch (a_seekType)
			{
			case SEEK_CUR:
				m_streamPosition += a_offset;
			case SEEK_END:
				m_streamPosition = m_streamLength + a_offset;
			case SEEK_SET:
				m_streamPosition = a_offset;
			}

			return m_streamPosition;
		}

		void	Flush() override { }

		bool	IsVaild() { return m_buffer != 0; }
		
		int8_t* ToArray() { return m_buffer; }

		bool	Grow(size_t a_newSize, size_t a_oldSize)
		{
			void* mem = realloc(m_buffer, a_newSize);
			if (!mem)
			{
				xUtilty::KernelPrintOut("realloc failed... [0x%lx]", a_newSize);
				return false;
			}

			m_buffer = (int8_t*)mem;
			m_streamLength = a_newSize;
			m_streamCapacity = a_newSize;
			return true;
		}
	protected:
		int8_t* m_buffer{ 0 };
	};
}
