#include "OrbisFIleHandler.h"
#include "OrbisMessageHandler.h"

namespace OrbisFIleHandler
{
	OrbisFIleHandler::OrbisFIleHandler(char* relpath, int Type, bool abs)
		:FDPos(0)
	{
		if (!relpath)
			return;

		char path[260]{ 0 };

		if (abs)
		{
			sprintf(path, "%s", relpath);
		}
		else
		{
			sprintf(path, "/app0/%s", relpath);
		}

		this->FilePath = strdup(path);
		this->FileType = Type;

		if ((this->FileDescriptor = sceKernelOpen(path, O_RDONLY, 0000555)) < 0)
		{
			OrbisMessageHandler::KernelPrintOut("%s Failed to open file for 0x%lx", __FUNCTION__, this->FileDescriptor);
			return;
		}

		if ((this->RET = sceKernelFstat(this->FileDescriptor, &this->FileStats)) != 0)
		{
			OrbisMessageHandler::KernelPrintOut("%s failed to gather file stats for 0x%lx", __FUNCTION__, this->RET);
			return;
		}
	}

	OrbisFIleHandler::~OrbisFIleHandler()
	{
		free(this->FilePath);

		if ((this->RET = sceKernelClose(this->FileDescriptor)) != 0)
		{
			OrbisMessageHandler::KernelPrintOut("%s Failed to close file for 0x%lx", __FUNCTION__, this->RET);
			return;
		}
	}

	bool	 OrbisFIleHandler::IsDescriptorVailed()
	{
		return (this->FileDescriptor > 0) ? sceKernelLseek(this->FileDescriptor, 0, 1) == 0 : false;
	}

	bool	 OrbisFIleHandler::IsStreamVaild()
	{
		return this->FDPos < this->FileStats.st_size;
	}

	bool	 OrbisFIleHandler::Rewind(uint64_t count)
	{
		uint64_t negcount = count - (count * 2);

		// invalid file
		if (!this->FileDescriptor)
			return false;
		
		// prevent a underflow
		if (this->FDPos == 0)
			return false;

		// underflow
		if ((this->FDPos - count) < 0)
			return false;

		if ((this->RET = sceKernelLseek(this->FileDescriptor, negcount, 1)) >= 0)
		{
			this->FDPos -= negcount;
			return true;
		}

		return false;
	}

	bool	 OrbisFIleHandler::Skip(uint64_t count)
	{
		// invalid file
		if (!this->FileDescriptor)
			return false;

		// prevent a overflow
		if (this->FDPos == this->FileStats.st_size)
			return false;

		// overflow
		if ((this->FDPos + count) < this->FileStats.st_size)
			return false;

		if ((this->RET = sceKernelLseek(this->FileDescriptor, count, 1)) >= 0)
		{
			this->FDPos += count;
			return true;
		}

		return false;
	}

	uint64_t OrbisFIleHandler::ReadBytes(void* dst, size_t dstLength)
	{
		if (!this->FileDescriptor)
			return -1;

		if (this->FDPos >= this->FileStats.st_size)
			return -1;

		if ((this->RET = sceKernelRead(this->FileDescriptor, dst, dstLength)) > 0)
		{
			this->FDPos += dstLength;
			return dstLength;
		}
		
		return -1;
	}

	uint64_t OrbisFIleHandler::WriteBytes(void* src, size_t srcLength)
	{
		if (!this->FileDescriptor)
			return -1;

		if (this->FDPos >= this->FileStats.st_size)
			return -1;

		if ((this->RET = sceKernelWrite(this->FileDescriptor, src, srcLength)) > 0)
		{
			OrbisMessageHandler::KernelPrintOut("%s %d", __FUNCTION__, __LINE__);
			this->FDPos += srcLength;
			return srcLength;
		}


		OrbisMessageHandler::KernelPrintOut("%s failed with 0x%lx", __FUNCTION__, this->RET);
		return -1;
	}

	uint64_t OrbisFIleHandler::ReadStringW(wchar_t* dst, int(*_fn)(int), uint64_t len, char endl, char endl2)
	{
		// string length
		uint64_t stringlength = 0;

		// string character 
		wchar_t chr = 0;

		while (true)
		{
			// string is outside buffer length
			if (stringlength > len)
				break;

			// returns if the current position is < file size and can read any left bytes
			if (this->IsStreamVaild())
			{
				// reads bytes into a buffer with the size of length and returns the size read, -1 in case of error
				if (this->ReadBytes(&chr, sizeof(wchar_t)) == -1)
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

	uint64_t OrbisFIleHandler::ReadString(char* dst, int(*_fn)(int), uint64_t len, char endl, char endl2)
	{
		// UTF-16 specific reader... wchar_t...
		if (this->FileType == UTF16)
		{
			return this->ReadStringW((wchar_t*)dst, _fn, len, endl, endl2);
		}

		// string length
		uint64_t stringlength = 0;
		
		// string character 
		unsigned char chr = 0;

		while (true)
		{
			// string is outside buffer length
			if (stringlength > len)
				break;

			// returns if the current position is < file size and can read any left bytes
			if (this->IsStreamVaild())
			{
				// reads bytes into a buffer with the size of length and returns the size read, -1 in case of error
				if (this->ReadBytes(&chr, sizeof(unsigned char)) == -1)
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

