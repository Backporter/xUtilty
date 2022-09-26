#include "../include/SystemWrapper.h"
#include "../include/FileSystem.h"
#include "../include/MessageHandler.h"

#if defined(__ORBIS__)
#include <kernel.h>
#include <rtc.h>
#include <sys/file.h>
#include <sys\dirent.h>
#elif defined(__OPENORBIS__)
#include <orbis/libkernel.h>
#include <dirent.h>
#endif

#include <stdint.h>
#include <iostream>
#include <vector>
#include <stdio.h>     
#include <stdarg.h>

#include <string>
#include <wchar.h>
#include <fcntl.h>
#include <unistd.h>

namespace OrbisFileSystem
{
	// https://github.com/KuromeSan/ps5-lib-il2cpp/blob/382f6daac6ea2da96d9e26ea44926df214b76d35/libil2cpp/os/PS5/Directory.cpp
	
	std::vector<DirectoryEntry> GetDirectoryFileEntries(const char* path, const char* extension, bool UseFilter, int PathType, bool combine)
	{
		std::vector<DirectoryEntry> DirectoryEntries;

#if defined (__ORBIS__) || defined(__OPENORBIS__)
		bool SwapCreed = (PathType == Data || PathType == USB0 || PathType == USB1 || PathType == USB2 || PathType == USB3 || PathType == USB4 || PathType == USB5 || PathType == USB6 || PathType == USB7 || PathType == System);

		if (SwapCreed)
			OrbisSystemWrapper::Jailbreak();

		int fd = open(path, O_RDONLY | O_DIRECTORY, 0000555);
		if (fd < 0) 
		{
			MessageHandler::Notify("open(%s) failed with 0x%lx", path, fd);
			if (SwapCreed)
				OrbisSystemWrapper::Jail();
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
				std::string filename;
				if (combine)
					filename += (char*)path;

				filename += entry->d_name;


				std::string ext = filename.substr(filename.size() - 4, 4);

				if (strcasecmp(ext.c_str(), extension) == 0)
				{
					DirectoryEntries.push_back(DirectoryEntry(combine ? std::string(path) + entry->d_name : entry->d_name, entry->d_type, entry->d_fileno));
				}
			}
			else
			{
				DirectoryEntries.push_back(DirectoryEntry(combine ? std::string(path) + entry->d_name : entry->d_name, entry->d_type, entry->d_fileno));
			}
		}

		if (SwapCreed)
			OrbisSystemWrapper::Jail();

		close(fd);
		free(buffer);
#endif
		return DirectoryEntries;
	}

	std::vector<DirectoryEntry> GetDirectoryEntries(const char* path, int PathType, bool combine)
	{
		std::vector<DirectoryEntry> DirectoryEntries;

#if defined (__ORBIS__) || defined(__OPENORBIS__)
		bool SwapCreed = (PathType == Data || PathType == USB0 || PathType == USB1 || PathType == USB2 || PathType == USB3 || PathType == USB4 || PathType == USB5 || PathType == USB6 || PathType == USB7 || PathType == System);

		if (SwapCreed)
			OrbisSystemWrapper::Jailbreak();

		int fd = open(path, O_RDONLY | O_DIRECTORY, 0000555);
		if (fd < 0)
		{
			if (SwapCreed)
				OrbisSystemWrapper::Jail();

			MessageHandler::KernelPrintOut("open(%s) returned 0x%lx", path, fd);
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
				DirectoryEntries.push_back(DirectoryEntry(combine ? std::string(path) + entry->d_name: entry->d_name, DT_DIR, entry->d_fileno));
			
		}

		if (SwapCreed)
			OrbisSystemWrapper::Jail();

		close(fd);
		free(buffer);
#endif
		return DirectoryEntries;

	}

	bool exists(const char* path, bool DIR)
	{
		int fd = 0;

		if (DIR)
			fd = open(path, 0 | O_DIRECTORY, 0);
		else
			fd = open(path, 0, 0);

		if (fd >= 0)
		{
			close(fd);
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
#if defined (__ORBIS__) || defined(__OPENORBIS__)
				OrbisSystemWrapper::MountUSB(0);
#endif
				sprintf(path, "/usb_0/%s", relpath);
				USBID = 0;
				FixUSB = true;
				break;
			case USB1:
#if defined (__ORBIS__) || defined(__OPENORBIS__)
				OrbisSystemWrapper::MountUSB(1);
#endif
				sprintf(path, "/usb_1/%s", relpath);
				USBID = 1;
				FixUSB = true;
				break;
			case USB2:
#if defined (__ORBIS__) || defined(__OPENORBIS__)
				OrbisSystemWrapper::MountUSB(2);
#endif
				sprintf(path, "/usb_2/%s", relpath);
				USBID = 2;
				FixUSB = true;
				break;
			case USB3:
#if defined (__ORBIS__) || defined(__OPENORBIS__)
				OrbisSystemWrapper::MountUSB(3);
#endif
				sprintf(path, "/usb_3/%s", relpath);
				USBID = 3;
				FixUSB = true;
				break;
			case USB4:
#if defined (__ORBIS__) || defined(__OPENORBIS__)
				OrbisSystemWrapper::MountUSB(4);
#endif
				sprintf(path, "/usb_4/%s", relpath);
				USBID = 4;
				FixUSB = true;
				break;
			case USB5:
#if defined (__ORBIS__) || defined(__OPENORBIS__)
				OrbisSystemWrapper::MountUSB(5);
#endif
				sprintf(path, "/usb_5/%s", relpath);
				USBID = 5;
				FixUSB = true;
				break;
			case USB6:
#if defined (__ORBIS__) || defined(__OPENORBIS__)
				OrbisSystemWrapper::MountUSB(6);
#endif
				sprintf(path, "/usb_6/%s", relpath);
				USBID = 6;
				FixUSB = true;
				break;
			case USB7:
#if defined (__ORBIS__) || defined(__OPENORBIS__)
				OrbisSystemWrapper::MountUSB(7);
#endif
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

#if defined (__ORBIS__) || defined(__OPENORBIS__)
		if (FixUSB && USBID != -1)
			OrbisSystemWrapper::UnmountUSB(USBID);
#endif
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
#if defined (__ORBIS__) || defined(__OPENORBIS__)
			OrbisSystemWrapper::MountUSB(0);
#endif
			sprintf(path, "/usb_0/%s", relpath);
			USBID = 0;
			FixUSB = true;
			break;
		case USB1:
#if defined (__ORBIS__) || defined(__OPENORBIS__)
			OrbisSystemWrapper::MountUSB(1);
#endif
			sprintf(path, "/usb_1/%s", relpath);
			USBID = 1;
			FixUSB = true;
			break;
		case USB2:
#if defined (__ORBIS__) || defined(__OPENORBIS__)
			OrbisSystemWrapper::MountUSB(2);
#endif
			sprintf(path, "/usb_2/%s", relpath);
			USBID = 2;
			FixUSB = true;
			break;
		case USB3:
#if defined (__ORBIS__) || defined(__OPENORBIS__)
			OrbisSystemWrapper::MountUSB(3);
#endif
			sprintf(path, "/usb_3/%s", relpath);
			USBID = 3;
			FixUSB = true;
			break;
		case USB4:
#if defined (__ORBIS__) || defined(__OPENORBIS__)
			OrbisSystemWrapper::MountUSB(4);
#endif
			sprintf(path, "/usb_4/%s", relpath);
			USBID = 4;
			FixUSB = true;
			break;
		case USB5:
#if defined (__ORBIS__) || defined(__OPENORBIS__)
			OrbisSystemWrapper::MountUSB(5);
#endif
			sprintf(path, "/usb_5/%s", relpath);
			USBID = 5;
			FixUSB = true;
			break;
		case USB6:
#if defined (__ORBIS__) || defined(__OPENORBIS__)
			OrbisSystemWrapper::MountUSB(6);
#endif
			sprintf(path, "/usb_6/%s", relpath);
			USBID = 6;
			FixUSB = true;
			break;
		case USB7:
#if defined (__ORBIS__) || defined(__OPENORBIS__)
			OrbisSystemWrapper::MountUSB(7);
#endif
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

#if defined (__ORBIS__) || defined(__OPENORBIS__)
		if (FixUSB && USBID != -1)
			OrbisSystemWrapper::UnmountUSB(USBID);
#endif
		return ret;
	}

	bool CreateFullPath(char* buffer, int reltype, const char* Relpath, ...)
	{
		if (!buffer)
			return false;

		char tempbuffer[260];
		va_list args;
		va_start(args, Relpath);
#if defined (__ORBIS__) || defined(__OPENORBIS__)
		if (OrbisSystemWrapper::vsprintf)
			OrbisSystemWrapper::vsprintf(tempbuffer, Relpath, args);
		else
#endif
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
			case None:
				sprintf(buffer, "%s", tempbuffer);
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
				MessageHandler::KernelPrintOut("Invailed Path type, overwriting with download");
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
				unlink(buf);
				mkdir(buf, 0000777);
			}

			buf[i] = path[i];
		}

		return true;
	}

	size_t CreateString(char* buffer, const char* MessageFMT, ...)
	{
		size_t length;
		va_list args;
		va_start(args, MessageFMT);

#if defined (__ORBIS__) || defined(__OPENORBIS__)
		if (OrbisSystemWrapper::vsprintf)
			length = OrbisSystemWrapper::vsprintf(buffer, MessageFMT, args);
		else
#endif
			length = vsprintf(buffer, MessageFMT, args);

		va_end(args);
		return length;
	}

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

	void WD(const char* path, void* data, size_t datalen)
	{
		FILE* Fdata = fopen(path, "wb");
		if (!Fdata)
		{
			MessageHandler::KernelPrintOut("Failed to open %s", path);
			return;
		}

		fwrite(data, sizeof(int8_t), datalen, Fdata);
		fclose(Fdata);
	}

	bool Mount(int rel)
	{
		bool ret = false;
#if defined (__ORBIS__) || defined(__OPENORBIS__)
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
#endif
		return ret;
	}

	bool Unmount(int rel)
	{
		bool ret = false;

#if defined (__ORBIS__) || defined(__OPENORBIS__)
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
#endif
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

		if ((src_fd = open(src, O_RDONLY, 0000555)) <= 0)
		{
			MessageHandler::KernelPrintOut("Failed to open [%s] for reading(reason 0x%lx)", src, src_fd);
			goto exit;
			return;
		}

		if ((dst_fd = open(dst, O_WRONLY | O_CREAT | O_DIRECT, 0000777)) <= 0)
		{
			MessageHandler::KernelPrintOut("Failed to open [%s] for writing(reason 0x%lx)", dst, dst_fd);
			goto exit;
			return;
		}

		if (fstat(src_fd, &stats) != 0)
		{
			stats.st_size = 0;
		}

		mem = (void*)malloc(stats.st_size);
		if ((ret = read(src_fd, mem, stats.st_size) <= 0))
		{
			goto exit;
		}

		if ((ret = write(dst_fd, mem, stats.st_size) <= 0))
		{
			goto exit;
		}

		goto exit;

	exit:
		close(src_fd);
		close(dst_fd);
		Unmount(reltype);
		free(mem);
		return;
	}

	void WriteFileToDisk(int reltype, const char* path, const void* data, size_t length)
	{
		if (!data || !length)
			return;

		int ret = 0;
		int fd = 0;

		char buffer[260];
		memset(buffer, 0, sizeof(buffer));

		Mount(reltype);

		if (reltype == Full)
		{
			OrbisFileSystem::CreateDirectoryPath((char*)path);
			fd = open(path, O_WRONLY | O_CREAT | O_DIRECT, 0000777);
		}
		else
		{
			CreateFullPath(buffer, reltype, path);
			OrbisFileSystem::CreateDirectoryPath(buffer);
			fd = open(buffer, O_WRONLY | O_CREAT | O_DIRECT, 0000777);
		}

		if (fd <= 0)
		{
			MessageHandler::KernelPrintOut("Failed to open [%s] for R/W", (reltype == Full ? path : buffer));
			return;
		}

		if ((ret = write(fd, data, length)) != length)
			MessageHandler::KernelPrintOut("Failed to write to [%s] fd[0x%lx] ret [0x%lx] length [0x%lx]", (reltype == Full ? path : buffer), fd, ret, length);

		close(fd);

		Unmount(reltype);
	}

	const char* GetFilenameFromPath(const char* path)
	{
		if (!path)
			return NULL;

		const char* newstr = strrchr(path, '/');
		if (!newstr)
			return NULL;

		newstr++;

		return newstr;
	}
}