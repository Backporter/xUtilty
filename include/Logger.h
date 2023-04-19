#pragma once

#if defined(__ORBIS__)
#include <kernel.h>
#include <net.h>
#include <libnetctl.h>
#elif defined(__OPENORBIS__)
#include <orbis/Net.h>
#include <orbis/libkernel.h>
#include <sys/socket.h>
#include <netinet/in.h>
typedef OrbisNetSockaddr SceNetSockaddr;
typedef sockaddr_in  SceNetSockaddrIn;
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <assert.h>
#include <memory>
#include <stddef.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>
#include <string.h>
#include <stdarg.h>
#include <inttypes.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>


namespace Log
{
	enum LoggerInstance : int
	{
		KModuleLog,
		kInputOutputLog,
		kUserInterfaceLog,
		kVirtualMachineLog,
		kMax,
	};

	// kManagedPath means we cloned the string.
	enum kFlags
	{
		kNone = 0,
		kManagedPath = 1 << 0,
		kNetworkMode = 1 << 1,
	};

	enum klogLevel
	{
		Normal,
		Warning,
		Error,
		Critical,
		Fatal,
	};

	class Log
	{
	public:
		Log();
		virtual ~Log();
	public:
		virtual bool Open(char* a_path);
		virtual bool Connect(char* a_ip);
		virtual bool OpenRelitive(int a_id, char* a_path);
		virtual bool Close();
		virtual bool Vaild();
		virtual bool Write(const char* fmt, ...);
		virtual bool WriteVA(uint32_t type, const char* fmt, va_list list);
		virtual bool WriteLevel(int a_logLevel, const char* a_fmt, ...);
		virtual bool Send(const char* MessageFMT, ...);
	public:
		static Log* GetSingleton(LoggerInstance i = LoggerInstance::KModuleLog)
		{ 
			static Log singleton[kMax]; 
			return &singleton[i];
		}
	private:
		const char*  m_path;
		int			 m_fd;
		int			 m_flags;
		int			 opt;
		sockaddr_in  NetworkServer;
	};
}