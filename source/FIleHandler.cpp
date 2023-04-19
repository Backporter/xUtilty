#include "../include/FIleHandler.h"
#include "../include/MessageHandler.h"
#include "../include/Macro.h"
#include "../include/SafeTypes.h"
#include "../include/SystemWrapper.h"

#include <unistd.h>

namespace FIleHandler
{
	FIleHandler::FIleHandler(char* a_path, int a_flags, int a_openflags) : 
		m_flags(a_flags), m_filepath(0), m_fd(0), m_ret(0), m_fdpos(0)
	{
		char buffer[260]{ 0 };
		
		if (!a_path)
			return;

		if ((a_flags & kTypeFlags::kIsConstPath) != 0)
			m_filepath = a_path;
		else
		{
			if ((m_flags & kTypeFlags::kIsAbsolutePath) != 0)
				sprintf(buffer, "%s", a_path);
			else if ((m_flags & kTypeFlags::kIsRelativePath) != 0)
				sprintf(buffer, COMBINE(ROOT, "%s"), a_path);

			m_filepath = strdup(buffer);
		}

		if ((m_fd = open(m_filepath, a_openflags, 0000777)) < 0)
		{
			MessageHandler::KernelPrintOut("%s Failed to open %s for 0x%lx(%s)", __FUNCTION__, m_filepath, m_fd, strerror(errno));
			return;
		}

		if ((m_ret = fstat(m_fd, &m_filestats)) != 0)
		{
			MessageHandler::KernelPrintOut("%s failed to gather file stats for 0x%lx", __FUNCTION__, m_ret);
			return;
		}
	}

	FIleHandler::~FIleHandler()
	{
		if ((m_ret = close(m_fd)) != 0)
			MessageHandler::KernelPrintOut("%s Failed to close file(%d/%s) for 0x%lx(%s)", __FUNCTION__, m_fd, m_filepath, m_ret, strerror(errno));

		if ((m_flags & kTypeFlags::kIsConstPath) == 0)
			free((void*)m_filepath);
	}

	bool	 FIleHandler::IsDescriptorVailed()
	{
		return (m_fd > 0) ? lseek(m_fd, 0, 1) == 0 : false;
	}

	bool	 FIleHandler::IsStreamVaild()
	{
		return m_fdpos < m_filestats.st_size;
	}

	bool	 FIleHandler::Rewind(uint64_t count)
	{
		uint64_t negcount = count - (count * 2);

		// invalid file
		if (!m_fd)
			return false;
		
		// prevent a underflow
		if (m_fdpos == 0)
			return false;

		// underflow
		if ((m_fdpos - count) < 0)
			return false;

		if ((m_ret = lseek(m_fd, negcount, 1)) >= 0)
		{
			m_fdpos -= negcount;
			return true;
		}

		return false;
	}

	bool	 FIleHandler::Skip(uint64_t count)
	{
		// invalid file
		if (!m_fd)
			return false;

		// prevent a overflow
		if (m_fdpos == m_filestats.st_size)
			return false;

		// overflow
		if ((m_fdpos + count) < m_filestats.st_size)
			return false;

		if ((m_ret = lseek(m_fd, count, 1)) >= 0)
		{
			m_fdpos += count;
			return true;
		}

		return false;
	}

	uint64_t FIleHandler::ReadBytes(void* dst, size_t dstdength)
	{
		if (!m_fd)
			return -1;

		if (m_fdpos >= m_filestats.st_size)
			return -1;

		if ((m_ret = read(m_fd, dst, dstdength)) > 0)
		{
			m_fdpos += dstdength;
			return dstdength;
		}
		
		return -1;
	}

	uint64_t FIleHandler::WriteBytes(void* src, size_t srcLength)
	{
		if (!m_fd)
			return -1;

		if ((m_ret = write(m_fd, src, srcLength)) > 0)
		{
			m_fdpos += srcLength;
			return srcLength;
		}

		MessageHandler::KernelPrintOut("%s failed with 0x%lx(%s)", __FUNCTION__, m_ret, strerror(errno));
		return -1;
	}

	uint64_t FIleHandler::ReadStringW(wchar_t_t* dst, int(*_fn)(int), uint64_t len, char endl, char endl2)
	{
		// string length
		uint64_t stringlength = 0;

		// string character 
		wchar_t_t chr = 0;

		while (true)
		{
			// string is outside buffer length
			if (stringlength > len)
				break;

			// returns if the current position is < file size and can read any left bytes
			if (this->IsStreamVaild())
			{
				// reads bytes into a buffer with the size of length and returns the size read, -1 in case of error
				if (this->ReadBytes(&chr, sizeof(wchar_t_t)) == -1)
					return -1;
			}
			else
			{
				// termanate the stirng
				dst[stringlength] = 0;

				// return the string length
				return stringlength;
			}

			// skip the endl, '\r'
			if (chr == endl2)
				continue;

			// we are at the end of the string('\n'), termanate and return the string length
			if (chr == endl)
			{
				dst[stringlength] = 0;
				return stringlength;
			}

			if (_fn)
				chr = _fn(chr);

			// set the string character and increase the length
			dst[stringlength] = chr;
			stringlength++;
		}

		// return on error
		return -1;
	}

	uint64_t FIleHandler::ReadString(char* dst, int(*_fn)(int), uint64_t len, char endl, char endl2)
	{
		// UTF-16 specific reader... wchar_t...
		if ((m_flags & kTypeFlags::kUTF16) != 0)
		{
			return ReadStringW((wchar_t_t*)dst, _fn, len, endl, endl2);
		}

		// string length
		uint64_t stringlength = 0;
		
		// string character 
		unsigned char chr = 0;

		while (true)
		{
			// string is outside buffer length
			if (stringlength > len)
			{
				break;
			}

			// returns if the current position is < file size and can read any left bytes
			if (IsStreamVaild())
			{
				// reads bytes into a buffer with the size of length and returns the size read, -1 in case of error
				if (ReadBytes(&chr, sizeof(unsigned char)) == -1)
				{
					return -1;
				}
			}
			else
			{
				// termanate the stirng
				dst[stringlength] = 0;

				// return the string length
				return stringlength;
			}

			// skip the endl, '\r'
			if (chr == endl2)
				continue;

			// we are at the end of the string('\n'), termanate and return the string length
			if (chr == endl)
			{
				dst[stringlength] = 0;
				return stringlength;
			}

			if (_fn)
				chr = _fn(chr);

			// set the string character and increase the length
			dst[stringlength] = chr;
			stringlength++;
		}

		// return on error
		return -1;
	}
}