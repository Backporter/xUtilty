#pragma once

// xxxxxxxx_t
#include <stdint.h>

// inet_pton
#include <arpa/inet.h>

// socket, connect, bind, etc
#include <sys/socket.h>

// sockaddr_in
#include <arpa/inet.h>

// read, write, etc
#include <unistd.h>

// open
#include <fcntl.h>

// sockaddr_in
#include <netinet\in.h>

// assert
#include <assert.h>

#if __OPENORBIS__
typedef OrbisNetSockaddr SceNetSockaddr;
typedef sockaddr_in  SceNetSockaddrIn;
#endif

namespace xUtilty
{
	enum kFlags
	{
		kNone = 0,
		kManagedPath = 1 << 0,
		kNetworkMode = 1 << 1,
	};

	class Log
	{
	public:
		enum klogLevel { Normal, Warning, Error, Critical, Fatal };
		enum LoggerInstance { KModuleLog, kInputOutputLog, kUserInterfaceLog, kVirtualMachineLog, kMax };
	public:
		Log() = default;
		virtual ~Log();
		
		// add
		virtual bool Open(char* a_path);
		virtual bool Connect(char* a_ip);
		virtual bool OpenRelitive(int a_id, char* a_path);
		virtual bool Close();
		virtual bool Vaild();
		virtual bool Write(const char* fmt, ...);
		virtual bool WriteVA(uint32_t type, const char* fmt, va_list list);
		virtual bool WriteLevel(int a_logLevel, const char* a_fmt, ...);
		virtual bool Send(const char* MessageFMT, ...);
		
		static Log* GetSingleton(int i = LoggerInstance::KModuleLog)
		{
			static Log singleton[kMax];
			return &singleton[i];
		}
	private:
		const char*  m_path{ nullptr };
		int			 m_fd{ -1 };
		int			 m_flags{ 0 };
		int			 opt;
		sockaddr_in  NetworkServer;
	};
}