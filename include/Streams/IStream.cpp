#include "IStream.h"

namespace xUtilty
{
	IStream::IStream(int64_t a_streamLength, int64_t a_streamCapacity, int64_t a_streamPosition, int32_t a_streamFlags) 
	{
		InitializeIStream(a_streamLength, a_streamCapacity, a_streamPosition, a_streamFlags);
	}

	void IStream::InitializeIStream(int64_t a_streamLength, int64_t a_streamCapacity, int64_t a_streamPosition, int32_t a_streamFlags)
	{
		m_streamLength	 = a_streamLength;
		m_streamCapacity = a_streamCapacity;
		m_streamPosition = a_streamPosition;

		SetFlags(a_streamFlags);
	}

	//
	size_t	 IStream::ReadString(char* dst, to_t* func, size_t len, char endl, char endl2)
	{
		size_t stringSize = 0;

		char chr = 0;

		while (true)
		{
			if (stringSize > len)
				break;

			if (m_streamPosition < m_streamCapacity)
			{
				if (Read(&chr, sizeof(char)) != sizeof(char))
				{
					return -1;
				}
			}
			else
			{
				dst[stringSize] = '\0';
				return stringSize;
			}

			if (chr == endl2)
				continue;

			if (chr == endl)
			{
				dst[stringSize] = '\0';
				return stringSize;
			}

			if (func)
				chr = func(chr);

			dst[stringSize] = chr;
			stringSize++;
		}

		//
		return -1;
	}

	size_t	 IStream::ReadStringW(wchar_t_t* dst, to_w_t* func, size_t len, wchar_t_t endl, wchar_t_t endl2)
	{
		size_t stringSize = 0;

		wchar_t_t chr = 0;

		while (true)
		{
			if (stringSize > len)
				break;

			if (m_streamPosition < m_streamCapacity)
			{
				if (Read(&chr, sizeof(wchar_t_t)) != sizeof(wchar_t_t))
				{
					return -1;
				}
			}
			else
			{
				dst[stringSize] = (wchar_t_t)L'\0';
				return stringSize;
			}

			if (chr == endl2)
				continue;

			if (chr == endl)
			{
				dst[stringSize] = (wchar_t_t)L'\0';
				return stringSize;
			}

			if (func)
				chr = func(chr);

			dst[stringSize] = chr;
			stringSize++;
		}

		//
		return -1;
	}
}