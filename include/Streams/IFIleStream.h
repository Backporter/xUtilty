#pragma once

#include "IStream.h"

namespace xUtilty
{
	class IFileStream : public IStream
	{
	public:
		IFileStream();
		IFileStream(int a_handle, int32_t a_flags);
		IFileStream(char* a_path, int32_t a_flags);
		~IFileStream() override;

		// override (IStream)
		void	Dispose() override;
		int64_t	Read(void* a_dst, size_t a_size) override;
		void	Write(void* a_buff, size_t a_len) override;
		int64_t	Seek(int64_t a_offset, int a_seekType) override;
		void	Flush() override;
		void	Truncate(int64_t a_size) override;

		// used internally.
		bool		Open(int a_handle, int32_t a_flags);
		bool		Open(const char* a_path, int32_t a_flags);
		inline bool	IsVaild() { return m_handle > 0; }
	protected:
		int m_handle { -1 };
	};
}