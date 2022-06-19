#include "../include/Logger.h"
#include "../include/FileSystem.h"
#include "../include/MessageHandler.h"

namespace Log
{
	Log::Log() :
		FD(-1), LogPath(nullptr), UseNet(false) {}

	Log::~Log()
	{
		if (this->Vaild())
			this->Close();

		free((void*)LogPath);

		this->FD = -1;
		this->LogPath = nullptr;
		this->UseNet = false;
	}

	bool Log::Open(const char* path)
	{
		if ((FD = open(path, O_CREAT | O_RDWR | O_SYNC | O_TRUNC, 0666)) < 0)
		{
			// MessageHandler::KernelPrintOut("%s %s %d failed to open path(%s) for log", __FILE__, __FUNCTION__, __LINE__, path);
			close(FD);
			return false;
		}

		// store path for ref in the future
		LogPath = strdup(path);
		return true;
	}

	bool Log::OpenRelitive(int ID, const char* path)
	{
		// prevent possable issues
		assert(ID != OrbisFileSystem::System);
		
		// buffer to store the created path
		char buff[260];
		if (!OrbisFileSystem::CreateFullPath(buff, ID, path))
		{
			// MessageHandler::KernelPrintOut("%s %s %d failed to create full path for log", __FILE__, __FUNCTION__, __LINE__);
			return false;
		}
		
		// confirm the directory path exists and create it
		if (!OrbisFileSystem::CreateDirectoryPath(buff))
		{
			// MessageHandler::KernelPrintOut("%s %s %d failed to create full path for log", __FILE__, __FUNCTION__, __LINE__);
			return false;
		}
		
		// open said log path
		if ((FD = open(buff, O_CREAT | O_RDWR | O_SYNC | O_TRUNC, 0666)) < 0)
		{
			close(FD);
			// MessageHandler::KernelPrintOut("%s %s %d failed to open path(%s) for log", __FILE__, __FUNCTION__, __LINE__, buff);
			return false;
		}
		
		// store path for ref in the future
		LogPath = strdup(path);
		return true;
	}

	bool Log::Close()
	{
		if (close(FD) != 0)
		{
			// MessageHandler::KernelPrintOut("%s %s %d failed to close log", __FILE__, __FUNCTION__, __LINE__);
			return false;
		}

		return true;
	}

	bool Log::Vaild()
	{
		return (lseek(this->FD, 0, SEEK_CUR) >= 0 && this->FD > 0);
	}

	bool Log::Write(const char* MessageFMT, ...)
	{
		if (!this->Vaild())
		{
			// MessageHandler::KernelPrintOut("this->Vaild = false");
			return false;
		}

		char endl = '\n';
		char buf[8096];

		va_list args;
		va_start(args, MessageFMT);
		size_t len = vsprintf(buf, MessageFMT, args);
		va_end(args);
		assert(len > 0);


		if ((ret = write(FD, buf, len)) != len)
		{ 
			// MessageHandler::KernelPrintOut("%s %s %d failed to write message to log(%s) ret: 0x%lx", __FILE__, __FUNCTION__, __LINE__, LogPath, ret);
			return false;
		}

		// ugly, we need to fix this...
		if ((ret = write(FD, &endl, 1)) != 1)
		{
			// MessageHandler::KernelPrintOut("%s %s %d failed to write endl to log(%s) ret: 0x%lx", __FILE__, __FUNCTION__, __LINE__, LogPath, ret);
			return false;
		}

		return true;
	}

	bool Log::WriteVA(uint32_t type, const char* MessageFMT, va_list list)
	{
		if (!this->Vaild())
		{
			// MessageHandler::KernelPrintOut("this->Vaild = false");
			return false;
		}

		char endl = '\n';
		char buf[2048];

		size_t len = vsprintf(buf, MessageFMT, list);
		assert(len > 0);

		if ((ret = write(FD, buf, len)) != len)
		{
			// MessageHandler::KernelPrintOut("%s %s %d failed to write message to log(%s) ret: 0x%lx", __FILE__, __FUNCTION__, __LINE__, LogPath, ret);
			return false;
		}

		// ugly, we need to fix this...
		if ((ret = write(FD, &endl, 1)) != 1)
		{
			// MessageHandler::KernelPrintOut("%s %s %d failed to write endl to log(%s) ret: 0x%lx", __FILE__, __FUNCTION__, __LINE__, LogPath, ret);
			return false;
		}

		return true;
	}

#if defined (__ORBIS__) || defined(__OPENORBIS__)
	bool Log::Connect(const char* IP)
	{
		if (!IP)
			return false;

		if ((this->FD = sceNetSocket("Network Logger", 2, 1, 0)) < 0)
		{
			// failed to create socket for log
			// MessageHandler::KernelPrintOut("Failed to create network log, ret 0x%lx", this->FD);
			return false;
		}

		memset(&NetworkServer, 0, sizeof(NetworkServer));
		NetworkServer.sin_len = sizeof(NetworkServer);
		NetworkServer.sin_family = 2;
		NetworkServer.sin_port = sceNetHtons(8081);

		if ((ret = sceNetInetPton(2, IP, &NetworkServer.sin_addr)) <= 0)
		{
			// MessageHandler::KernelPrintOut("sceNetInetPton failed, ret 0x%lx", ret);
			return false;
		}

		if ((ret = sceNetConnect(this->FD, (const SceNetSockaddr *)&NetworkServer, sizeof(this->NetworkServer))) < 0)
		{
			// MessageHandler::KernelPrintOut("sceNetConnect Failed 0x%lx", ret);
			return false;
		}

		sceKernelFcntl(this->FD, F_SETFL, O_NONBLOCK);

		// this->opt = 1;
		// if ((ret = sceNetSetsockopt(this->FD, 0xffff, 0x1200, (char*)&this->opt, 4)) != 0)
		// {
		// 	// ???
		// 	// MessageHandler::KernelPrintOut("sceNetSetsockopt failed 0x%lx", ret);
		// }

		if ((ret = sceNetSend(this->FD, "PS4 Connected!", 14, 128)) != 14)
		{
			// MessageHandler::KernelPrintOut("Inital Message failed to send..");
			return false;
		}

		LogPath = strdup(IP);
		return true;
	}

	bool Log::Disconnect()
	{
		if ((ret = sceNetSocketClose(this->FD)) != 0)
		{
			// MessageHandler::KernelPrintOut("Failed to close socket, ret 0x%lx", ret);
			return false;
		}

		return true;
	}

	bool Log::Send(const char* MessageFMT, ...)
	{
		if (!this->Vaild())
		{
			// MessageHandler::KernelPrintOut("this->Vaild = false");
			return false;
		}

		char endl = '\n';
		char buf[8096];

		va_list args;
		va_start(args, MessageFMT);
		size_t len = vsprintf(buf, MessageFMT, args);
		va_end(args);
		assert(len > 0);

		if ((ret = sceNetSend(this->FD, buf, len, 128)) != len)
		{
			// MessageHandler::KernelPrintOut("%s %s %d failed to write message to log(%s) ret: 0x%lx", __FILE__, __FUNCTION__, __LINE__, LogPath, ret);
			return false;
		}

		// ugly, we need to fix this...
		if ((ret = sceNetSend(this->FD, &endl, len, 128)) != 1)
		{
			// MessageHandler::KernelPrintOut("%s %s %d failed to write endl to log(%s) ret: 0x%lx", __FILE__, __FUNCTION__, __LINE__, LogPath, ret);
			return false;
		}

		return true;
	}
#endif
}