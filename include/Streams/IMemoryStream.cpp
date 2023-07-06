#include "../MessageHandler.h"

#include "IMemoryStream.h"

#include <memory>

namespace xUtilty
{
	IMemoryStream::IMemoryStream() : IStream(0, 0, 0, 0)
	{}

	IMemoryStream::IMemoryStream(size_t a_size) : IStream(0, 0, 0, 0)
	{
		Initialize(a_size);
	}

	IMemoryStream::~IMemoryStream() 
	{ 
		Dispose(); 
	}

	void	IMemoryStream::Dispose()
	{
		if (!TestFlags(IStream::kFlags::kFlagIsManaged) && !TestFlags(IStream::kFlags::kFlagIsDisposed) && m_buffer)
		{
			free((void*)m_buffer);
			SetFlags(IStream::kFlags::kFlagIsDisposed);
		}
	}

	int64_t	IMemoryStream::Read(void* a_buffer, size_t a_bufflen)
	{
		if (IsVaild())
		{
			if (!a_buffer || !a_bufflen)
			{
				KernelPrintOut("Invailed buffer passed into MemoryStream.. ignoring.");
			}
			else
			{
				if (m_streamPosition + a_bufflen > m_streamCapacity)
				{
					Grow(m_streamLength + a_bufflen + IStream::IO_STREAM_DEFAULT_SIZE, m_streamCapacity);
				}

				memcpy(a_buffer, (const void*)&m_buffer[m_streamPosition], a_bufflen);
				m_streamPosition += a_bufflen;
			}

			return a_bufflen;
		}

		return 0;
	}

	void	IMemoryStream::Write(void* a_buffer, size_t a_bufflen)
	{
		if (m_streamPosition + a_bufflen > m_streamCapacity)
		{
			if (!Grow(m_streamLength + a_bufflen + IStream::IO_STREAM_DEFAULT_SIZE, m_streamCapacity))
				return;
		}

		if (m_buffer)
		{
			if (!a_buffer || !a_bufflen)
			{
				Notify("Invaild buffer/size passed into IMemoryStream::Write(void*, size_t)");
			}
			else
			{
				memcpy(&m_buffer[m_streamPosition], a_buffer, a_bufflen);
				m_streamPosition += a_bufflen;
				m_streamLength += a_bufflen;

			}
		}
	}

	int64_t IMemoryStream::Seek(int64_t a_offset, int a_seekType)
	{
		if (m_streamPosition + a_offset > m_streamCapacity)
			Grow(m_streamLength + a_offset + IStream::IO_STREAM_DEFAULT_SIZE, m_streamCapacity);

		switch (a_seekType)
		{
		case SEEK_CUR:
			m_streamPosition += a_offset;
		case SEEK_END:
			m_streamPosition = m_streamCapacity + a_offset;
		case SEEK_SET:
			m_streamPosition = a_offset;
		}

		return m_streamPosition;
	}

	void	IMemoryStream::Flush()
	{ 
	}
	
	void	IMemoryStream::Truncate(int64_t a_newSize)
	{
		Grow(a_newSize, GetStreamCapacity());
	}

	bool	IMemoryStream::Resize(size_t a_newSize, size_t a_oldSize)
	{
		return Grow(a_newSize, a_oldSize);
	}


	void	IMemoryStream::Initialize(size_t a_size)
	{
		//
		InitializeIStream(0, a_size, 0, 0);
		//

		m_buffer = (int8_t*)malloc(m_streamCapacity);
		if (!m_buffer)
		{
			KernelPrintOut("[Debug] IMemoryStream::Initialize Malloc Faild");
			return;
		}

		memset(m_buffer, 0, m_streamCapacity);

		SetFlags(IStream::kFlags::kFlagCanRead);
		SetFlags(IStream::kFlags::kFlagCanWrite);
		SetFlags(IStream::kFlags::kFlagCanSeek);
	}

	bool	IMemoryStream::Grow(size_t a_newSize, size_t a_oldSize)
	{
		void* mem = realloc(m_buffer, a_newSize);
		if (!mem)
		{
			KernelPrintOut("realloc failed... [0x%lx]", a_newSize);
			return false;
		}

		m_buffer = (int8_t*)mem;
		m_streamLength = 0;
		m_streamCapacity = a_newSize;
		m_streamPosition = 0;
		return true;
	}
}