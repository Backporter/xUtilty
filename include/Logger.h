#pragma once

// THINKING
#include "../include/Macro.h"

// xxxxxxxx_t
#include <stdint.h>

#if __clang__
// inet_pton
#include <arpa/inet.h>

// socket, connect, bind, etc
#include <sys/socket.h>

// sockaddr_in
#include <arpa/inet.h>

// read, write, etc
#include <unistd.h>

// sockaddr_in
#include <netinet\in.h>
#elif _WIN32
#endif

// open
#include <fcntl.h>

// assert
#include <assert.h>

// va_args
#include <stdarg.h>

#if __OPENORBIS__

// OrbisNetSockaddr
#include <orbis/Net.h>

typedef OrbisNetSockaddr SceNetSockaddr;
typedef sockaddr_in  SceNetSockaddrIn;
#endif

// print the current code pos into the kernel log(debug)
#ifndef L_PRINT_POS
#define L_PRINT_POS xUtilty::Log::GetSingleton()->Write(xUtilty::Log::logLevel::kNone,"%s %s %d", __FUNCTION__, __FILE__, __LINE__);
#endif

// print the current code pos into the kernel log and return(debug)
#ifndef L_PRINT_POS_RETURN 
#define L_PRINT_POS_RETURN xUtilty::Log::GetSingleton()->Write(xUtilty::Log::logLevel::kNone,"%s %s %d %s", __FUNCTION__, __FILE__, __LINE__, THINKING); return;
#endif

// print the current code pos + custom fmt + string into the kernel log 
#ifndef L_PRINT_FMT
#define L_PRINT_FMT(fmt, ...) xUtilty::Log::GetSingleton()->Write(xUtilty::Log::logLevel::kNone, "%s %s %d %s " fmt, __FUNCTION__, __FILE__, __LINE__, THINKING, __VA_ARGS__);
#endif

// print the current code pos + custom fmt + string into the kernel log and return T
#ifndef L_PRINT_FMT_RETURN_T
#define L_PRINT_FMT_RETURN_T(T, fmt, ...) xUtilty::Log::GetSingleton()->Write(xUtilty::Log::logLevel::kNone,"%s %s %d %s "  fmt, __FUNCTION__,__FILE__, __LINE__, THINKING, __VA_ARGS__); return T;
#endif

// print the current code pos + custom fmt + string into the kernel log and return(debug)
#ifndef L_PRINT_RETURN
#define L_PRINT_RETURN(fmt, ...) xUtilty::Log::GetSingleton()->Write(xUtilty::Log::logLevel::kNone,"%s %s %d %s "  fmt, __FUNCTION__, __FILE__, __LINE__, THINKING, __VA_ARGS__); return;
#endif

// print the current code pos + custom string into the kernel log and return(debug)
#ifndef L_PRINT_STRING_RETURN 
#define L_PRINT_STRING_RETURN(string)  xUtilty::Log::GetSingleton()->Write(xUtilty::Log::logLevel::kNone,"%s %s %d %s", __FUNCTION__, __FILE__, __LINE__, string); return;
#endif

// print the current code pos + custom string into the kernel log(debug)
#ifndef L_PRINT_STRING
#define L_PRINT_STRING(string)  xUtilty::Log::GetSingleton()->Write(xUtilty::Log::logLevel::kNone,"%s %s %d %s", __FUNCTION__,__FILE__, __LINE__, string);
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
#if __clang__
		inline
#elif _WIN32 || _WIN64
#endif
		static constexpr const char* const LogLevelName[] = { "", "[Info] ", "[Warning] ", "[Error] ", "[Critical] ", "[Fatal] ", "[Debug] ", "[Trace] " };

		enum logLevel 
		{
			kNone,
			kInfo,
			kWarning, 
			kError, 
			kCritical, 
			kFatal,
			kDebug,
			kTrace,
		};

		enum LoggerInstance 
		{
			// 
			kModuleLog, 
			
			//
			kInputOutputLog, 
			
			//
			kUserInterfaceLog, 
			
			//
			kVirtualMachineLog, 
			
			//
			kNetwork, 
			
			//
			kMax
		};
	public:
		Log() = default;
		virtual ~Log();
		
		// add

		// Open a log with the full path directly.
		virtual bool Open(char* a_path);

		// Connect to a socket.
		// WARNING: BLOCKING!.
		virtual bool Connect(char* a_ip, int port = 8081);
		
		// Open with a relitive path ID from xUtilty::FileSystem.
		virtual bool OpenRelitive(int a_id, char* a_path);

		// Close the Log/Disconnect from the socket.
		virtual bool Close();

		// Confirm the logs file handle is still vaild.
		virtual bool Vaild();

		// Write a string to the log + '\n'
		virtual bool Write(logLevel a_logLevel, const char* fmt, ...);

		// Write a format with a predefined va_list.
		// First argument is unsued.
		virtual bool WriteVA(uint32_t, logLevel a_logLevel, const char* fmt, va_list list);

		// Send a string over to the socket + '\n'
		virtual bool Send(logLevel a_logLevel, const char* MessageFMT, ...);
		
		// singleton inside a getter.
		static Log* GetSingleton(int i = 0)
		{
			static Log singleton[kMax];
			return &singleton[i];
		}

		inline const char* GetLevel(int level)
		{
			return LogLevelName[level];
		}
	private:
		const char*  m_path{ nullptr };
		int			 m_fd{ -1 };
		int			 m_flags{ 0 };
		int			 opt;
#if __clang__
		sockaddr_in  NetworkServer;
#endif
	};
}