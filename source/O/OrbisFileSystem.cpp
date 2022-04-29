#include <stdint.h>
#include <iostream>
#include <vector>
#include <stdio.h>     
#include <stdarg.h>

#if defined(__ORBIS__)
#include <kernel.h>
#include <rtc.h>
#include <sys/file.h>
#elif defined(__OPENORBIS__)
#include <orbis/libkernel.h>
#include <dirent.h>
#endif

#include "../O/OrbisSystemWrapper.h"
#include "../O/OrbisFileSystem.h"

namespace OrbisFileSystem
{
	// https://github.com/KuromeSan/ps5-lib-il2cpp/blob/382f6daac6ea2da96d9e26ea44926df214b76d35/libil2cpp/os/PS5/Directory.cpp
	
	std::vector<DirectoryEntry> GetDirectoryFileEntries(const char* path, const char* extension, bool UseFilter, int PathType)
	{
		static std::vector<DirectoryEntry> DirectoryEntries;
		
		bool SwapCreed = (PathType == USB0 || PathType == USB1 || PathType == USB2 || PathType == USB3 || PathType == USB4 || PathType == USB5 || PathType == USB6 || PathType == USB7 || PathType == System);

		if (SwapCreed)
			OrbisSystemWrapper::Jailbreak();

		int fd = sceKernelOpen(path, O_RDONLY | O_DIRECTORY, 0000555);
		if (fd < 0) 
		{
			OrbisMessageHandler::Notify("sceKernelOpen(%s) failed with 0x%lx", path, fd);
			return DirectoryEntries;
		}

		struct stat dirstat;
		fstat(fd, &dirstat);

		char* buffer = (char*)malloc(dirstat.st_blksize);

		int usedbufferlen = 0;
		int curpos = dirstat.st_blksize;

		while (true)
		{
			if (curpos >= usedbufferlen)
			{
				int ret = sceKernelGetdents(fd, buffer, dirstat.st_blksize);
				if (ret <= 0)
				{
					break;
				}

				usedbufferlen = ret;
				curpos = 0;
			}

			dirent *entry = (dirent*)(buffer + curpos);
			curpos += entry->d_reclen;

			if (strcasecmp(entry->d_name, ".") == 0 || strcasecmp(entry->d_name, "..") == 0)
				continue;

			if (UseFilter)
			{
				// compare the extension and add it
				std::string filename(entry->d_name);
				std::string ext = filename.substr(filename.size() - 4, 4);

				if (strcasecmp(ext.c_str(), extension) == 0)
				{
					DirectoryEntries.push_back(DirectoryEntry(entry->d_name, entry->d_type, entry->d_fileno));
				}
			}
			else
			{
				DirectoryEntries.push_back(DirectoryEntry(entry->d_name, entry->d_type, entry->d_fileno));
			}
		}

		if (SwapCreed)
			OrbisSystemWrapper::Jail();

		sceKernelClose(fd);
		free(buffer);
		return DirectoryEntries;
	}

	std::vector<DirectoryEntry> GetDirectoryEntries(const char* path, int PathType)
	{
		static std::vector<DirectoryEntry> DirectoryEntries;

		bool SwapCreed = (PathType == USB0 || PathType == USB1 || PathType == USB2 || PathType == USB3 || PathType == USB4 || PathType == USB5 || PathType == USB6 || PathType == USB7 || PathType == System);

		if (SwapCreed)
			OrbisSystemWrapper::Jailbreak();

		int fd = sceKernelOpen(path, O_RDONLY | O_DIRECTORY, 0000555);
		if (fd < 0)
		{
			if (SwapCreed)
				OrbisSystemWrapper::Jail();

			OrbisMessageHandler::KernelPrintOut("sceKernelOpen(%s) returned 0x%lx", path, fd);
			return DirectoryEntries;
		}

		struct stat dirstat;
		fstat(fd, &dirstat);

		char* buffer = (char*)malloc(dirstat.st_blksize);

		int usedbufferlen = 0;
		int curpos = dirstat.st_blksize;

		while (true)
		{
			if (curpos >= usedbufferlen)
			{
				int ret = sceKernelGetdents(fd, buffer, dirstat.st_blksize);
				if (ret <= 0) { break; }

				usedbufferlen = ret;
				curpos = 0;
			}

			dirent *entry = (dirent*)(buffer + curpos);
			curpos += entry->d_reclen;

			if (strcasecmp(entry->d_name, ".") == 0 || strcasecmp(entry->d_name, "..") == 0)
				continue;

			if (entry->d_type == DT_DIR)
				DirectoryEntries.push_back(DirectoryEntry(entry->d_name, DT_DIR, entry->d_fileno));
			
		}

		if (SwapCreed)
			OrbisSystemWrapper::Jail();

		sceKernelClose(fd);
		free(buffer);
		return DirectoryEntries;

	}

	bool exists(const char* path, bool DIR)
	{
		int fd = 0;

		if (DIR)
			fd = sceKernelOpen(path, O_RDONLY | O_DIRECTORY, 0000555);
		else
			fd = sceKernelOpen(path, O_RDONLY, 0000555);

		if (fd >= 0)
		{
			sceKernelClose(fd);
			return true;
		}
		else
		{
			return false;
		}
	}

	bool PathExists(int reltype, const char* relpath, bool IsDirectory)
	{
		int  USBID  = -1;
		bool FixUSB = false;
		bool ret	= false;
		char path[260]{ 0 };

		switch (reltype)
		{
			case App:
				sprintf(path, "/app0/%s", relpath);
				break;
			case Download:
				sprintf(path, "/download0/%s", relpath);
				break;
			case USB0:
				OrbisSystemWrapper::MountUSB(0);
				sprintf(path, "/usb_0/%s", relpath);
				USBID = 0;
				FixUSB = true;
				break;
			case USB1:
				OrbisSystemWrapper::MountUSB(1);
				sprintf(path, "/usb_1/%s", relpath);
				USBID = 1;
				FixUSB = true;
				break;
			case USB2:
				OrbisSystemWrapper::MountUSB(2);
				sprintf(path, "/usb_2/%s", relpath);
				USBID = 2;
				FixUSB = true;
				break;
			case USB3:
				OrbisSystemWrapper::MountUSB(3);
				sprintf(path, "/usb_3/%s", relpath);
				USBID = 3;
				FixUSB = true;
				break;
			case USB4:
				OrbisSystemWrapper::MountUSB(4);
				sprintf(path, "/usb_4/%s", relpath);
				USBID = 4;
				FixUSB = true;
				break;
			case USB5:
				OrbisSystemWrapper::MountUSB(5);
				sprintf(path, "/usb_5/%s", relpath);
				USBID = 5;
				FixUSB = true;
				break;
			case USB6:
				OrbisSystemWrapper::MountUSB(6);
				sprintf(path, "/usb_6/%s", relpath);
				USBID = 6;
				FixUSB = true;
				break;
			case USB7:
				OrbisSystemWrapper::MountUSB(7);
				sprintf(path, "/usb_7/%s", relpath);
				USBID = 7;
				FixUSB = true;
				break;
			case Data:
				sprintf(path, "/data/%s", relpath);
				break;
			case SaveData0:
				sprintf(path, "/savedata0/%s", relpath);
				break;
			case SaveData1:
				sprintf(path, "/savedata1/%s", relpath);
				break;
			case SaveData2:
				sprintf(path, "/savedata2/%s", relpath);
				break;
			case SaveData3:
				sprintf(path, "/savedata3/%s", relpath);
				break;
			case SaveData4:
				sprintf(path, "/savedata4/%s", relpath);
				break;
			case SaveData5:
				sprintf(path, "/savedata5/%s", relpath);
				break;
			case SaveData6:
				sprintf(path, "/savedata6/%s", relpath);
				break;
			case SaveData7:
				sprintf(path, "/savedata7/%s", relpath);
				break;
			case SaveData8:
				sprintf(path, "/savedata8/%s", relpath);
				break;
			case SaveData9:
				sprintf(path, "/savedata9/%s", relpath);
				break;
			case SaveData10:
				sprintf(path,"/savedata10/%s", relpath);
				break;
			case SaveData11:
				sprintf(path, "/savedata11/%s", relpath);
				break;
			case SaveData12:
				sprintf(path, "/savedata12/%s", relpath);
				break;
			case SaveData13:
				sprintf(path, "/savedata13/%s", relpath);
				break;
			case SaveData14:
				sprintf(path, "/savedata14/%s", relpath);
				break;
			case SaveData15:
				sprintf(path, "/savedata15/%s", relpath);
				break;
			case SaveData16:
				sprintf(path, "/savedata16/%s", relpath);
				break;
			case System:
				sprintf(path, "/%s", relpath);
				break;
			case Full:
				ret = exists(relpath, IsDirectory);
				break;
			case MIRA:
				sprintf(path, "/_mira/%s", relpath);
				break;
			default:
				sprintf(path, "/download0/%s", relpath);
				break;
		}
		
		if (reltype != Full)
			ret = exists(path, IsDirectory);


		if (FixUSB && USBID != -1)
			OrbisSystemWrapper::UnmountUSB(USBID);

		return ret;
	}

	bool PathExists2(char* path, int reltype, const char* relpath, bool IsDirectory, bool create)
	{
		int  USBID = -1;
		bool FixUSB = false;
		bool ret = false;

		switch (reltype)
		{
		case App:
			sprintf(path, "/app0/%s", relpath);
			break;
		case Download:
			sprintf(path, "/download0/%s", relpath);
			break;
		case USB0:
			OrbisSystemWrapper::MountUSB(0);
			sprintf(path, "/usb_0/%s", relpath);
			USBID = 0;
			FixUSB = true;
			break;
		case USB1:
			OrbisSystemWrapper::MountUSB(1);
			sprintf(path, "/usb_1/%s", relpath);
			USBID = 1;
			FixUSB = true;
			break;
		case USB2:
			OrbisSystemWrapper::MountUSB(2);
			sprintf(path, "/usb_2/%s", relpath);
			USBID = 2;
			FixUSB = true;
			break;
		case USB3:
			OrbisSystemWrapper::MountUSB(3);
			sprintf(path, "/usb_3/%s", relpath);
			USBID = 3;
			FixUSB = true;
			break;
		case USB4:
			OrbisSystemWrapper::MountUSB(4);
			sprintf(path, "/usb_4/%s", relpath);
			USBID = 4;
			FixUSB = true;
			break;
		case USB5:
			OrbisSystemWrapper::MountUSB(5);
			sprintf(path, "/usb_5/%s", relpath);
			USBID = 5;
			FixUSB = true;
			break;
		case USB6:
			OrbisSystemWrapper::MountUSB(6);
			sprintf(path, "/usb_6/%s", relpath);
			USBID = 6;
			FixUSB = true;
			break;
		case USB7:
			OrbisSystemWrapper::MountUSB(7);
			sprintf(path, "/usb_7/%s", relpath);
			USBID = 7;
			FixUSB = true;
			break;
		case Data:
			sprintf(path, "/data/%s", relpath);
			break;
		case SaveData0:
			sprintf(path, "/savedata0/%s", relpath);
			break;
		case SaveData1:
			sprintf(path, "/savedata1/%s", relpath);
			break;
		case SaveData2:
			sprintf(path, "/savedata2/%s", relpath);
			break;
		case SaveData3:
			sprintf(path, "/savedata3/%s", relpath);
			break;
		case SaveData4:
			sprintf(path, "/savedata4/%s", relpath);
			break;
		case SaveData5:
			sprintf(path, "/savedata5/%s", relpath);
			break;
		case SaveData6:
			sprintf(path, "/savedata6/%s", relpath);
			break;
		case SaveData7:
			sprintf(path, "/savedata7/%s", relpath);
			break;
		case SaveData8:
			sprintf(path, "/savedata8/%s", relpath);
			break;
		case SaveData9:
			sprintf(path, "/savedata9/%s", relpath);
			break;
		case SaveData10:
			sprintf(path, "/savedata10/%s", relpath);
			break;
		case SaveData11:
			sprintf(path, "/savedata11/%s", relpath);
			break;
		case SaveData12:
			sprintf(path, "/savedata12/%s", relpath);
			break;
		case SaveData13:
			sprintf(path, "/savedata13/%s", relpath);
			break;
		case SaveData14:
			sprintf(path, "/savedata14/%s", relpath);
			break;
		case SaveData15:
			sprintf(path, "/savedata15/%s", relpath);
			break;
		case SaveData16:
			sprintf(path, "/savedata16/%s", relpath);
			break;
		case System:
			sprintf(path, "/%s", relpath);
			break;
		case Full:
			ret = exists(relpath, IsDirectory);
			break;
		case MIRA:
			sprintf(path, "/_mira/%s", relpath);
			break;
		default:
			sprintf(path, "/download0/%s", relpath);
			break;
		}

		if (reltype != Full)
			ret = exists(path, IsDirectory);
		
		if (create)
			CreateDirectoryPath(path);

		if (FixUSB && USBID != -1)
			OrbisSystemWrapper::UnmountUSB(USBID);

		return ret;
	}
	
	bool CreateFullPath(char* buffer, int reltype, const char* Relpath, ...)
	{
		if (!buffer)
			return false;

		char tempbuffer[260];
		va_list args;
		va_start(args, Relpath);
		vsprintf(tempbuffer, Relpath, args);
		va_end(args);

		switch (reltype)
		{
			case App:
				sprintf(buffer, "%s%s", "/app0/", tempbuffer);
				break;
			case Download:
				sprintf(buffer, "%s%s", "/download0/", tempbuffer);
				break;
			case USB0:
				sprintf(buffer, "%s%s", "/usb_0/", tempbuffer);
				break;
			case USB1:
				sprintf(buffer, "%s%s", "/usb_1/", tempbuffer);
				break;
			case USB2:
				sprintf(buffer, "%s%s", "/usb_2/", tempbuffer);
				break;
			case USB3:
				sprintf(buffer, "%s%s", "/usb_3/", tempbuffer);
				break;
			case USB4:
				sprintf(buffer, "%s%s", "/usb_4/", tempbuffer);
				break;
			case USB5:
				sprintf(buffer, "%s%s", "/usb_5/", tempbuffer);
				break;
			case USB6:
				sprintf(buffer, "%s%s", "/usb_6/", tempbuffer);
				break;
			case USB7:
				sprintf(buffer, "%s%s", "/usb_7/", tempbuffer);
				break;
			case Data:
				sprintf(buffer, "%s%s", "/data/", tempbuffer);
				break;
			case SaveData0:
				sprintf(buffer, "%s%s", "/savedata0/", tempbuffer);
				break;
			case SaveData1:
				sprintf(buffer, "%s%s", "/savedata1/", tempbuffer);
				break;
			case SaveData2:
				sprintf(buffer, "%s%s", "/savedata2/", tempbuffer);
				break;
			case SaveData3:
				sprintf(buffer, "%s%s", "/savedata3/", tempbuffer);
				break;
			case SaveData4:
				sprintf(buffer, "%s%s", "/savedata4/", tempbuffer);
				break;
			case SaveData5:
				sprintf(buffer, "%s%s", "/savedata5/", tempbuffer);
				break;
			case SaveData6:
				sprintf(buffer, "%s%s", "/savedata6/", tempbuffer);
				break;
			case SaveData7:
				sprintf(buffer, "%s%s", "/savedata7/", tempbuffer);
				break;
			case SaveData8:
				sprintf(buffer, "%s%s", "/savedata8/", tempbuffer);
				break;
			case SaveData9:
				sprintf(buffer, "%s%s", "/savedata9/", tempbuffer);
				break;
			case SaveData10:
				sprintf(buffer, "%s%s", "/savedata10/", tempbuffer);
				break;
			case SaveData11:
				sprintf(buffer, "%s%s", "/savedata11/", tempbuffer);
				break;
			case SaveData12:
				sprintf(buffer, "%s%s", "/savedata12/", tempbuffer);
				break;
			case SaveData13:
				sprintf(buffer, "%s%s", "/savedata13/", tempbuffer);
				break;
			case SaveData14:
				sprintf(buffer, "%s%s", "/savedata14/", tempbuffer);
				break;
			case SaveData15:
				sprintf(buffer, "%s%s", "/savedata15/", tempbuffer);
				break;
			case SaveData16:
				sprintf(buffer, "%s%s", "/savedata16/", tempbuffer);
				break;
			case System:
				sprintf(buffer, "%s%s", "/", tempbuffer);
				break;
			case MIRA:
				sprintf(buffer, "%s%s", "/_mira/", tempbuffer);
				break;
			default:
				OrbisMessageHandler::KernelPrintOut("Invailed Path type, overwriting with download");
				sprintf(buffer, "%s%s", "/download0/", tempbuffer);
		}
		return true;
	}

	bool CreateDirectoryPath(char* path)
	{
		char buf[260]{ 0 };
		size_t strsize = strlen(path);
		
		for (int i = 0; i < strsize; i++)
		{
			if (path[i] == '/' || path[i] == '\\')
			{
				sceKernelUnlink(buf);
				sceKernelMkdir(buf, 0000777);
			}
			buf[i] = path[i];
		}

		return true;
	}

	size_t CreateString(char* buffer, const char* MessageFMT, ...)
	{
		va_list args;
		va_start(args, MessageFMT);
		size_t length = vsprintf(buffer, MessageFMT, args);
		va_end(args);
		return length;
	}

	// uses std::string
	bool GetFilenameFromPath(char* buffer, size_t buflen, char* string)
	{
		// confirm vaild memory
		if (!buffer || !string || buflen <= 0)
			return false;

		// convert char* into string for easy manipulation
		std::string newstring(string);
		auto size = newstring.size();
		auto pos = 0;

		// find the last file path splitter
		if ((pos = newstring.rfind('/')) == std::string::npos)
			return false;

		// increase the pos past the splitter as we don't want "/XXXXx" as our string
		pos += 1;
		
		// create the new stirng and get it's size
		newstring = newstring.substr(pos, size - pos);
		size = newstring.size();

		// confirm the string + term will fit into the buffer
		if (buflen < size || buflen < size + sizeof(char))
			return false;

		// get the string
		const char* ptr = newstring.c_str();

		// put said string into the buffer
		for (int i = 0; i < size; i++)
		{
			buffer[i] = ptr[i];

			// termanate it
			if (i == size)
			{
				buffer[i + 1] = '\0';
			}

		}

		return true;
	}

	bool ReplaceString(const char* path, char* buffer, size_t bufflen, char* replace, const char* find, const char* find2)
	{
		OrbisMessageHandler::KernelPrintOut("[DEBUG] Path [%s]", path);

		std::string paths(path);
		
		auto pos = paths.find(find);
		if (pos != std::string::npos)
		{
			paths = paths.replace(pos, strlen(find), replace);
			const char* str = paths.c_str();
			strncpy(buffer, str, paths.size());
			return buffer[paths.size()] == '/';
		}

		pos = paths.find(find2);
		if (pos != std::string::npos)
		{
			paths = paths.replace(pos, strlen(find2), replace);
			const char* str = paths.c_str();
			strncpy(buffer, str, paths.size());
			return buffer[paths.size()] == '/';
		}

		auto len = strlen(path);
		strncpy(buffer, path, len);
		return buffer[len] == '/';

		__asm__("int3");
	}


	/*
	void WriteData(void* data, size_t datasize, int reltype, const char* relpath)
	{
		char path[260];
		SceRtcDateTime now;
		FILE* fd;

		switch (reltype)
		{
		case App:
			sprintf(path, "%s%s", "/app0/", relpath);
			break;
		case Download:
			sprintf(path, "%s%s", "/download0/", relpath);
			break;
		case USB:
			if (sceRtcGetCurrentClockLocalTime(&now) != 0)
			{
				now.month = 0;
				now.day = 0;
				now.year = 0;
				now.hour = 0;
				now.minute = 0;
			}
			OrbisSystemWrapper::MountUSB();
			sprintf(path, "%s%d%d%d%d%d%s", "/usb_0/", now.month, now.day, now.year, now.hour, now.minute, ".dump");
			break;
		case SaveData:
			sprintf(path, "%s%s", "/savedata0/", relpath);
			break;
		default:
			sprintf(path, "%s%s", "/download0/", relpath);
			break;
		}

		if (!(fd = fopen(path, "wb"))) { return; }
		if (fwrite(data, sizeof(int8_t), datasize, fd) != datasize) { OrbisMessageHandler::KernelPrintOut("Failed to write buffer to file for \"%s\"", strerror(errno));  }

		fclose(fd);
	}
	*/

	void RDP(const char* path, void* pattern, size_t patternlength, int plusindex, uint64_t datalength, void* dst)
	{
		FILE* fdata = fopen(path, "rb");
		if (!fdata) { return; }

		fseek(fdata, 0, SEEK_END);
		long len = ftell(fdata);
		fseek(fdata, 0, SEEK_SET);

		while (ftell(fdata) < len)
		{
			void* buff = malloc(patternlength);
			fread(buff, sizeof(uint8_t), patternlength, fdata);
			if (memcmp(buff, pattern, patternlength) == 0)
			{
				fseek(fdata, plusindex, SEEK_CUR);
				fread(dst, sizeof(uint8_t), datalength, fdata);
				break;
			}
			free(buff);
		}
	}

	void RD(const char* path, uint64_t offset, uint64_t DataLength, void* Data) 
	{
		FILE* Fdata = fopen(path, "rb");
		if (!Fdata) { return; }

		fseek(Fdata, offset, SEEK_SET);
		if (fread(Data, sizeof(int8_t), DataLength, Fdata) != DataLength)
		{
			perror("Failed To Read Data Into buffer! Reason: ");
		}
		fclose(Fdata);
	}


	// void WD(const char* path, void* data, size_t datalen)
	// {
	// 	int fd = 0;
	// 
	// 	if ((fd = sceKernelOpen(path, 0, 0000777)) < 0)
	// 		return;
	// 
	// 	sceKernelWrite(fd, data, datalen);
	// 	sceKernelClose(fd);
	// 
	// }

	void WD(const char* path, void* data, size_t datalen) 
	{
		FILE* Fdata = fopen(path, "wb");
		if (!Fdata)
			return;

		fwrite(data, sizeof(int8_t), datalen, Fdata);
		fclose(Fdata);
	}

	// void Write(char* Path, int PathType, void* buf, size_t bufflen)
	// {
	// 	if (!Path || !buf)
	// 		return;
	// 
	// 	char buffer[260]{ 0 };
	// 	int fd = 0;
	// 	int ret = 0;
	// 	bool IsFull = (PathType == Full);
	// 
	// 	if (!IsFull)
	// 		OrbisFileSystem::CreateFullPath(buffer, PathType, Path);
	// 
	// 	
	// 	if ((fd = sceKernelOpen((IsFull ? Path : buffer), O_RDWR | O_CREAT | O_DIRECT, 0000777)) >= 0)
	// 	{
	// 		if ((ret = sceKernelWrite(fd, buf, bufflen)) != bufflen)
	// 			OrbisMessageHandler::KernelPrintOut("Failed to write buffer to file for 0x%lx", fd);
	// 		goto exit;
	// 	}
	// 	else
	// 	{
	// 		OrbisMessageHandler::KernelPrintOut("Failed to open %s for 0x%lx", fd);
	// 		goto exit;
	// 	}
	// 
	// exit:
	// 	sceKernelClose(fd);
	// }

	bool Mount(int rel)
	{
		bool ret = false;

		switch (rel)
		{
		case USB0:
			ret = OrbisSystemWrapper::MountUSB(0);
			break;
		case USB1:
			ret = OrbisSystemWrapper::MountUSB(1);
			break;
		case USB2:
			ret = OrbisSystemWrapper::MountUSB(2);
			break;
		case USB3:
			ret = OrbisSystemWrapper::MountUSB(3);
			break;
		case USB4:
			ret = OrbisSystemWrapper::MountUSB(4);
			break;
		case USB5:
			ret = OrbisSystemWrapper::MountUSB(5);
			break;
		case USB6:
			ret = OrbisSystemWrapper::MountUSB(6);
			break;
		case USB7:
			ret = OrbisSystemWrapper::MountUSB(7);
			break;
		default:
			break;
		}

		return ret;
	}

	bool Unmount(int rel)
	{
		bool ret = false;

		switch (rel)
		{
			case USB0:
				ret = OrbisSystemWrapper::UnmountUSB(0);
				break;
			case USB1:
				ret = OrbisSystemWrapper::UnmountUSB(1);
				break;
			case USB2:
				ret = OrbisSystemWrapper::UnmountUSB(2);
				break;
			case USB3:
				ret = OrbisSystemWrapper::UnmountUSB(3);
				break;
			case USB4:
				ret = OrbisSystemWrapper::UnmountUSB(4);
				break;
			case USB5:
				ret = OrbisSystemWrapper::UnmountUSB(5);
				break;
			case USB6:
				ret = OrbisSystemWrapper::UnmountUSB(6);
				break;
			case USB7:
				ret = OrbisSystemWrapper::UnmountUSB(7);
				break;
			default:
				break;
		}

		return ret;
	}

	void Copy2(char* src, char* dst, int reltype)
	{
		if (!src)
			return;

		if (!dst)
			return;

		Mount(reltype);

		if (!OrbisFileSystem::PathExists(OrbisFileSystem::Full, dst, false))
			OrbisFileSystem::CreateDirectoryPath(dst);

		int src_fd = 0;
		int dst_fd = 0;
		int ret = 0;
		struct stat stats;
		void* mem = NULL;

		if ((src_fd = sceKernelOpen(src, O_RDONLY, 0000555)) <= 0)
		{
			OrbisMessageHandler::KernelPrintOut("Failed to open [%s] for reading(reason 0x%lx)", src, src_fd);
			goto exit;
			return;
		}

		if ((dst_fd = sceKernelOpen(dst, O_WRONLY | O_CREAT | O_DIRECT, 0000777)) <= 0)
		{
			OrbisMessageHandler::KernelPrintOut("Failed to open [%s] for writing(reason 0x%lx)", dst, dst_fd);
			goto exit;
			return;
		}
		
		if (sceKernelFstat(src_fd, &stats) != 0)
		{
			stats.st_size = 0;
		}

		mem = (void*)malloc(stats.st_size);
		if ((ret = sceKernelRead(src_fd, mem, stats.st_size) <= 0))
		{
			goto exit;
		}
	
		if ((ret = sceKernelWrite(dst_fd, mem, stats.st_size) <= 0))
		{
			goto exit;
		}

		goto exit;

		exit:
		sceKernelClose(src_fd);
		sceKernelClose(dst_fd);
		Unmount(reltype);
		free(mem);
		return;
	}
}