#pragma once

#include "IStream.h"

//
#include <stdlib.h>

namespace xUtilty
{
	class IMemoryStream : public IStream
	{
	public:
		IMemoryStream();
		IMemoryStream(size_t a_size);
		~IMemoryStream() override;

		// 
		void	Dispose() override;
		int64_t	Read(void* a_buffer, size_t a_bufflen) override;
		void	Write(void* a_buffer, size_t a_bufflen) override;
		int64_t	Seek(int64_t a_offset, int a_seekType) override;
		void	Flush() override;
		void	Truncate(int64_t) override;
		
		bool			Resize(size_t a_newSize, size_t a_oldSize);
		void			Initialize(size_t a_size = IStream::IO_STREAM_DEFAULT_SIZE);
		inline bool		IsVaild() { return m_buffer != nullptr; }
		inline int8_t*	ToArray() { return m_buffer; }
	protected:
		int8_t* m_buffer { nullptr };
	private:
		bool Grow(size_t a_newSize, size_t a_oldSize);
	};
}
