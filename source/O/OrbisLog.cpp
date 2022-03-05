#include "../O/OrbisLog.h"
#include "../O/OrbisFileSystem.h"
#include "../O/OrbisMessageHandler.h"

#include <stdarg.h>
#include <assert.h>

namespace OrbisLog
{
	OrbisLog::OrbisLog()
		:FD(0), LogPath(nullptr), UseNet(false)
	{
		// 
	}

	OrbisLog::~OrbisLog()
	{
		// 
	}

	bool OrbisLog::Open(const char* path)
	{
		if ((FD = sceKernelOpen(path, O_CREAT | O_RDWR | O_SYNC | O_TRUNC, 0000777)) < 0)
		{
			OrbisMessageHandler::KernelPrintOut("%s %s %d failed to open path(%s) for log", __FILE__, __FUNCTION__, __LINE__, path);
			sceKernelClose(FD);
			return false;
		}

		return true;
	}

	bool OrbisLog::OpenRelitive(int ID, const char* path)
	{
		if (ID == OrbisFileSystem::NET)
		{
			this->UseNet = true;
			return this->Connect(path);
		}

		// buffer to store the created path
		char buff[260];
		if (!OrbisFileSystem::CreateFullPath(buff, ID, path))
		{
			OrbisMessageHandler::KernelPrintOut("%s %s %d failed to create full path for log", __FILE__, __FUNCTION__, __LINE__);
			return false;
		}

		// confirm the directory path exists and create it
		if (!OrbisFileSystem::CreateDirectoryPath(buff))
		{
			OrbisMessageHandler::KernelPrintOut("%s %s %d failed to create full path for log", __FILE__, __FUNCTION__, __LINE__);
			return false;
		}

		// open said log path
		if ((FD = sceKernelOpen(buff, O_CREAT | O_RDWR | O_SYNC | O_TRUNC, 0000777)) < 0)
		{
			sceKernelClose(FD);
			OrbisMessageHandler::KernelPrintOut("%s %s %d failed to open path(%s) for log", __FILE__, __FUNCTION__, __LINE__, buff);
			return false;
		}

		// store path for ref in the future
		LogPath = strdup(path);
		return true;
	}
	
	bool OrbisLog::Connect(const char* IP)
	{
		if (!IP)
			return false;

		if ((this->FD = sceNetSocket("Network Logger", 2, 1, 0)) < 0)
		{
			// failed to create socket for log
			OrbisMessageHandler::KernelPrintOut("Failed to create network log, ret 0x%lx", this->FD);
			return false;
		}

		memset(&NetworkServer, 0, sizeof(NetworkServer));
		NetworkServer.sin_len = sizeof(NetworkServer);
		NetworkServer.sin_family = 2;
		NetworkServer.sin_port = sceNetHtons(8081);

		if ((ret = sceNetInetPton(2, IP, &NetworkServer.sin_addr)) <= 0)
		{
			OrbisMessageHandler::KernelPrintOut("sceNetInetPton failed, ret 0x%lx", ret);
			return false;
		}

		if ((ret = sceNetConnect(this->FD, (SceNetSockaddr *)&NetworkServer, sizeof(this->NetworkServer))) < 0)
		{
			OrbisMessageHandler::KernelPrintOut("sceNetConnect Failed 0x%lx", ret);
			return false;
		}

		if ((ret = sceNetSend(this->FD, "PS4 Connected!", 14, 128)) != 14)
		{
			OrbisMessageHandler::KernelPrintOut("Inital Message failed to send..");
			return false;
		}

		return true;
	}

	bool OrbisLog::Close()
	{
		if (this->UseNet)
			return this->Disconnect();

		if (sceKernelClose(FD) != 0)
		{
			OrbisMessageHandler::KernelPrintOut("%s %s %d failed to close log", __FILE__, __FUNCTION__, __LINE__);
			return false;
		}

		return true;
	}

	bool OrbisLog::Disconnect()
	{
		if ((ret = sceNetSocketClose(this->FD)) != 0)
		{
			OrbisMessageHandler::KernelPrintOut("Failed to close socket, ret 0x%lx", ret);
			return false;
		}

		return true;
	}

	bool OrbisLog::Write(const char* MessageFMT, ...)
	{
		char endl = '\n';
		char buf[8096];

		va_list args;
		va_start(args, MessageFMT);
		size_t len = vsprintf(buf, MessageFMT, args);
		va_end(args);
		assert(len > 0);

		if (this->UseNet)
			return this->Send(buf);

		if ((ret = sceKernelWrite(FD, buf, len)) != len)
		{ 
			OrbisMessageHandler::KernelPrintOut("%s %s %d failed to write message to log(%s) ret: 0x%lx", __FILE__, __FUNCTION__, __LINE__, LogPath, ret);
			return false;
		}

		// ugly, we need to fix this...
		if ((ret = sceKernelWrite(FD, &endl, 1)) != 1)
		{
			OrbisMessageHandler::KernelPrintOut("%s %s %d failed to write endl to log(%s) ret: 0x%lx", __FILE__, __FUNCTION__, __LINE__, LogPath, ret);
			return false;
		}


		return true;
	}

	bool OrbisLog::Send(const char* MessageFMT, ...)
	{
		if (this->FD < 0)
			return false;

		char endl = '\n';
		char buf[8096];

		va_list args;
		va_start(args, MessageFMT);
		size_t len = vsprintf(buf, MessageFMT, args);
		va_end(args);
		assert(len > 0);

		if ((ret = sceNetSend(this->FD, buf, len, 128)) != len)
		{
			OrbisMessageHandler::KernelPrintOut("%s %s %d failed to write message to log(%s) ret: 0x%lx", __FILE__, __FUNCTION__, __LINE__, LogPath, ret);
			return false;
		}

		// ugly, we need to fix this...
		if ((ret = sceNetSend(this->FD, &endl, len, 128)) != 1)
		{
			OrbisMessageHandler::KernelPrintOut("%s %s %d failed to write endl to log(%s) ret: 0x%lx", __FILE__, __FUNCTION__, __LINE__, LogPath, ret);
			return false;
		}


		return true;
	}
}