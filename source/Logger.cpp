#include "../include/Logger.h"
#include "../include/FileSystem.h"
#include "../include/MessageHandler.h"
#include "../include/INIHandler.h"
#include "../include/SystemWrapper.h"

namespace Log
{
	Log::Log() : m_fd(-1), m_path(nullptr), m_flags(0) {}

	Log::~Log()
	{
		if (Vaild())
			Close();

		if ((m_flags & kFlags::kManagedPath) == 0)
			free((void*)m_path);

		m_fd = -1;
		m_path = nullptr;
		m_flags = 0;
	}
	
	bool Log::Open(char* path)
	{
		if ((m_fd = open(path, O_CREAT | O_RDWR | O_SYNC | O_TRUNC | O_NONBLOCK, 0666)) < 0)
		{
			close(m_fd);
			return false;
		}

		m_flags |= kFlags::kManagedPath;
		m_path = strdup(path);
		return true;
	}

	bool Log::Connect(char* IP)
	{
		int ret = 0;

		if (!IP)
			return false;

		if ((m_fd = socket(2, 1, 0)) < 0)
		{
			return false;
		}

		memset(&NetworkServer, 0, sizeof(NetworkServer));
		NetworkServer.sin_len = sizeof(NetworkServer);
		NetworkServer.sin_family = 2;
		NetworkServer.sin_port = htons(8081);

		if ((ret = inet_pton(2, IP, &NetworkServer.sin_addr)) <= 0)
		{
			return false;
		}

		if ((ret = connect(m_fd, (const sockaddr *)&NetworkServer, sizeof(NetworkServer))) < 0)
		{
			return false;
		}

		fcntl(m_fd, F_SETFL, O_NONBLOCK);

		// opt = 1;
		// if ((ret = sceNetSetsockopt(m_fd, 0xffff, 0x1200, (char*)&opt, 4)) != 0)
		// {
		// 	// ???
		// 	// MessageHandler::KernelPrintOut("sceNetSetsockopt failed 0x%lx", ret);
		// }

		if ((ret = send(m_fd, "PS4 Connected!", 14, 128)) != 14)
		{
			return false;
		}

		m_flags |= kFlags::kManagedPath;
		m_flags |= kFlags::kNetworkMode;
		m_path = strdup(IP);
		return true;
	}

	bool Log::OpenRelitive(int ID, char* path)
	{
		// prevent possable issues
		assert(ID != OrbisFileSystem::System);
		
		if (ID == OrbisFileSystem::Net)
			return Connect(path);

		// buffer to store the created path
		char buff[260];
		if (!OrbisFileSystem::CreateFullPath(buff, ID, path))
		{
			// MessageHandler::KernelPrintOut("%s %s %d failed to create full path for log", __FILE__, __FUNCTION__, __LINE__);
			return false;
		}

		// confirm the directory path exists and create it
		OrbisFileSystem::CreateDirectoryPath(buff);

		// open said log path
		if ((m_fd = open(buff, O_CREAT | O_RDWR | O_SYNC | O_TRUNC | O_NONBLOCK, 0666)) < 0)
		{
			close(m_fd);
			// MessageHandler::KernelPrintOut("%s %s %d failed to open path(%s) for log", __FILE__, __FUNCTION__, __LINE__, buff);
			return false;
		}

		// store path for ref in the future
		m_flags |= kFlags::kManagedPath;
		m_path = strdup(path);
		return true;
	}

	bool Log::Close()
	{
		if ((m_flags & kFlags::kManagedPath) != 0)
			free((void*)m_path);

		if (close(m_fd) != 0)
		{
			return false;
		}

		return true;
	}

	bool Log::Vaild()
	{
		if ((m_flags & kFlags::kNetworkMode) != 0)
			return m_fd > 0;
		else
			return (lseek(m_fd, 0, SEEK_CUR) >= 0 && m_fd > 0);
	}

	bool Log::Write(const char* MessageFMT, ...)
	{
		int ret = 0;
		if (!Vaild())
			return false;

		char endl = '\n';
		char buf[2048];

		va_list args;
		va_start(args, MessageFMT);
		size_t len = SystemWrapper::vsprintf(buf, MessageFMT, args);
		va_end(args);
		assert(len > 0);

		if ((m_flags & kFlags::kNetworkMode) != 0)
			return Send(buf);

		if ((ret = write(m_fd, buf, len)) != len)
		{ 
			// MessageHandler::KernelPrintOut("%s %s %d failed to write message to log(%s) ret: 0x%lx", __FILE__, __FUNCTION__, __LINE__, m_path, ret);
			return false;
		}

		// ugly, we need to fix this...
		if ((ret = write(m_fd, &endl, 1)) != 1)
		{
			// MessageHandler::KernelPrintOut("%s %s %d failed to write endl to log(%s) ret: 0x%lx", __FILE__, __FUNCTION__, __LINE__, m_path, ret);
			return false;
		}

		return true;
	}

	bool Log::WriteVA(uint32_t type, const char* MessageFMT, va_list list)
	{
		int ret = 0;
		if (!Vaild())
			return false;

		char endl = '\n';
		char buf[2048];

		size_t len = SystemWrapper::vsprintf(buf, MessageFMT, list);
		assert(len > 0);

		if ((ret = write(m_fd, buf, len)) != len)
		{
			// MessageHandler::KernelPrintOut("%s %s %d failed to write message to log(%s) ret: 0x%lx", __FILE__, __FUNCTION__, __LINE__, m_path, ret);
			return false;
		}

		// ugly, we need to fix this...
		if ((ret = write(m_fd, &endl, 1)) != 1)
		{
			// MessageHandler::KernelPrintOut("%s %s %d failed to write endl to log(%s) ret: 0x%lx", __FILE__, __FUNCTION__, __LINE__, m_path, ret);
			return false;
		}

		return true;
	}

	bool Log::WriteLevel(int a_logLevel, const char* a_fmt, ...)
	{
		return false;
	}

	bool Log::Send(const char* MessageFMT, ...)
	{
		int ret = 0;
		if (!Vaild())
			return false;

		char endl = '\n';
		char buf[2048];

		va_list args;
		va_start(args, MessageFMT);
		size_t len = SystemWrapper::vsprintf(buf, MessageFMT, args);
		va_end(args);
		assert(len > 0);

		if ((ret = send(m_fd, buf, len, 128)) != len)
		{
			return false;
		}

		if ((ret = send(m_fd, &endl, len, 128)) != 1)
		{
			return false;
		}
		return true;
	}
}