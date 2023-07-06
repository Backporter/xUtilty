#include "../include/Logger.h"

//
#include "../include/FileSystem.h"
#include "../include/SystemWrapper.h"

#include <time.h>

namespace xUtilty
{
	Log::~Log()
	{
		if (Vaild())
		{
			Close();
		}

		if ((m_flags & kFlags::kManagedPath) == 0)
		{
			free((void*)m_path);
		}

		m_fd = -1;
		m_path = nullptr;
		m_flags = 0;
	}
	
	bool Log::Open(char* path)
	{
#if __clang__
		int flags = O_CREAT | O_RDWR | O_TRUNC | O_SYNC | O_NONBLOCK;
#elif _WIN32 || _WIN64
		int flags = O_CREAT | O_RDWR | O_TRUNC;
#endif
		if ((m_fd = SystemWrapper::open(path, flags, 0000777)) < 0)
		{
			SystemWrapper::close(m_fd);
			return false;
		}

		m_flags |= kFlags::kManagedPath;
		m_path = strdup(path);
		return true;
	}

	bool Log::Connect(char* IP, int port)
	{
#if __clang__
		int ret = 0;

		if (!IP)
		{
			return false;
		}

		if ((m_fd = socket(2, 1, 0)) < 0)
		{
			KernelPrintOut("Failed to open socket... returned 0x%lx", m_fd);
			return false;
		}

		/*
		// atempt to flag socket as non-blocking
		const int flags = fcntl(m_fd, 3, 0);
		if (flags < 0)
		{
			KernelPrintOut("Failed to get socket flags! [0x%lx]", flags);
			return false;
		}

		if ((ret = fcntl(m_fd, 4, flags | 0x0004)) < 0)
		{
			KernelPrintOut("Failed to get socket flags! [0x%lx]", ret);
			return false;
		}
		//
		*/

		memset(&NetworkServer, 0, sizeof(NetworkServer));
		NetworkServer.sin_len = sizeof(NetworkServer);
		NetworkServer.sin_family = 2;
		NetworkServer.sin_port = htons(port);

		if ((ret = inet_pton(2, IP, &NetworkServer.sin_addr)) <= 0)
		{
			return false;
		}

		if ((ret = connect(m_fd, (const sockaddr *)&NetworkServer, sizeof(NetworkServer))) < 0)
		{
			return false;
		}

		// opt = 1;
		// if ((ret = sceNetSetsockopt(m_fd, 0xffff, 0x1200, (char*)&opt, 4)) != 0)
		// {
		// 	// ???
		// 	// MessageHandler::KernelPrintOut("sceNetSetsockopt failed 0x%lx", ret);
		// }

		m_flags |= kFlags::kManagedPath;
		m_flags |= kFlags::kNetworkMode;

		m_path = strdup(IP);
#endif
		return true;
	}

	bool Log::OpenRelitive(int ID, char* path)
	{
		// prevent possable issues
		assert(ID != FileSystem::System);
		
		if (ID == FileSystem::Net)
			return Connect(path);

		// buffer to store the created path
		char buff[260];
		if (!FileSystem::CreateFullPath(buff, ID, path))
		{
			// MessageHandler::KernelPrintOut("%s %s %d failed to create full path for log", __FILE__, __FUNCTION__, __LINE__);
			return false;
		}

		// confirm the directory path exists and create it
		FileSystem::CreateDirectoryPath(buff);

		// open said log path
#if __clang__
		int flags = O_CREAT | O_RDWR | O_TRUNC  | O_SYNC | O_NONBLOCK;
#elif _WIN32 || _WIN64
		int flags = O_CREAT | O_RDWR | O_TRUNC;
#endif
		if ((m_fd = SystemWrapper::open(buff, flags, 0666)) < 0)
		{
			SystemWrapper::close(m_fd);
			PRINT_FMT("failed to open path(%s) for log", buff);
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
		{
			free((void*)m_path);
		}

		if (SystemWrapper::close(m_fd) != 0)
		{
			return false;
		}

		m_fd = -1;
		return true;
	}

	bool Log::Vaild()
	{
		if ((m_flags & kFlags::kNetworkMode) != 0)
		{
			return m_fd > 0;
		}
		else
		{
			return (m_fd > 0 && SystemWrapper::lseek(m_fd, 0, SEEK_CUR) >= 0);
		}
	}

	bool Log::Write(logLevel a_logLevel, const char* MessageFMT, ...)
	{
		//
		char buf[1024 * 2] { 0 };
		int64_t s_time;
		char newline = '\n';
		int ret = 0;

		if (!Vaild())
		{
			return false;
		}

		//
		ret = sprintf(buf, "%s", GetLevel(a_logLevel));


		time(&s_time);
		auto time = localtime(&s_time);
		ret += strftime(&buf[ret], sizeof(buf), "[%m/%d/%Y - %I:%M:%S%p] ", time);

		va_list args;
		va_start(args, MessageFMT);
		size_t len = ret + SystemWrapper::vsprintf(&buf[ret], MessageFMT, args);
		va_end(args);
		
		//
		assert(len > 0);
		
		if ((ret = SystemWrapper::write(m_fd, buf, len)) != len)
		{ 
			return false;
		}

		if ((ret = SystemWrapper::write(m_fd, &newline, 1)) != 1)
		{
			return false;
		}


		return true;
	}

	bool Log::WriteVA(uint32_t, logLevel a_logLevel, const char* MessageFMT, va_list list)
	{
		//
		char	buf[1024 * 2] { 0 };
		int64_t s_time;
		char	newline = '\n';
		int		ret = 0;

		if (!Vaild())
		{
			return false;
		}

		//
		ret = sprintf(buf, "%s", GetLevel(a_logLevel));

		time(&s_time);
		auto time = localtime(&s_time);
		ret += strftime(&buf[ret], sizeof(buf), "[%m/%d/%Y - %I:%M:%S%p] ", time);

		//
		size_t len = ret + SystemWrapper::vsprintf(&buf[ret], MessageFMT, list);
		
		//
		assert(len > 0);

		if ((ret = SystemWrapper::write(m_fd, buf, len)) != len)
		{
			return false;
		}

		if ((ret = SystemWrapper::write(m_fd, &newline, 1)) != 1)
		{
			return false;
		}

		return true;
	}

	bool Log::Send(logLevel a_logLevel, const char* MessageFMT, ...)
	{
#if __clang__
		char newline = '\n';
		char buf[1024 * 2] { 0 };
		int ret = 0;

		if (!Vaild())
		{
			return false;
		}

		//
		ret = sprintf(buf, "%s", GetLevel(a_logLevel));

		//
		va_list args;
		va_start(args, MessageFMT);
		size_t len = SystemWrapper::vsprintf(&buf[ret], MessageFMT, args);
		va_end(args);
		
		//
		assert(len > 0);

		if ((ret = send(m_fd, buf, len, 128)) != len)
		{
			return false;
		}

		if ((ret = send(m_fd, &newline, 1, 128)) != 1)
		{
			return false;
		}
#endif
		return true;
	}
}