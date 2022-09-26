#pragma once

#if defined(__ORBIS__)
#include <kernel.h>
#include <net.h>
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

#ifndef _MESSAGE
#define _MESSAGE(fmt) Log::Log::GetSingleton()->Write(fmt);
#endif

#ifndef _MESSAGE_VA
#define _MESSAGE_VA(fmt, ...) Log::Log::GetSingleton()->Write(fmt, __VA_ARGS__);
#endif

#ifndef _MESSAGELAST
#define _MESSAGELAST(fmt) Log::Log::GetSingleton()->Write(fmt, __VA_ARGS__); \
Log::Log::GetSingleton()->Write(fmt);
#endif

#ifndef _MESSAGELAST_VA
#define _MESSAGELAST_VA(fmt, ...) Log::Log::GetSingleton()->Write(fmt, __VA_ARGS__); \
Log::Log::GetSingleton()->Write(fmt, __VA_ARGS__);
#endif

namespace Log
{
	class Log
	{
	public:
		Log();
		virtual ~Log();

		// Open's the log with a full path, exactly like OpenRelitive minus the ID
		virtual bool Open(const char* path);

		// This will open the log with the Relitive ID + path, ex OpenRelitive(OrbisFileSystem::Download, "OCEL/log.txt"); = "/download0/OCEL/log.txt"
		// ID matches the ID in OrbisFileSystem::XXXX
		virtual bool OpenRelitive(int ID, const char* path);

		// closes log
		virtual bool Close();

		// Confirms the file descriptor is still vaild
		virtual bool Vaild();

		// writes a message to the log
		virtual bool Write(const char* fmt, ...);

		// writes a message to the log
		virtual bool WriteVA(uint32_t type, const char* fmt, va_list list);

#if defined (__ORBIS__) || defined(__OPENORBIS__)
		// network version of this->Open(), this network stuff, so it's cleaner to do it in a different method
		// BEWARE, blocking function.... 
		virtual bool Connect(const char* IP);

		// network version of this->Close(), network stuff is seperated for cleaner code
		// BEWARE, blocking function.... 
		virtual bool Disconnect();

		// network version of this->write(), network stuff is seperated for cleaner code... use this->Write as it works too.
		// BEWARE, blocking function.... 
		virtual bool Send(const char* MessageFMT, ...);
#endif

		// Singleton implementation for a single class
		static Log* GetSingleton()
		{
			static Log _OrbisLog;
			return &_OrbisLog;
		}

		// debug stuff
		// Singleton implementation for a single class
		static Log* GetMiraLog()
		{
			static Log _OrbisLog;
			return &_OrbisLog;
		}

	private:
		// file descriptor when a local file is used, socket when the network is used
		int			FD;
		uint64_t	ret;
		const char* LogPath;

		bool		UseNet;
		int			opt;

#if defined (__ORBIS__) || defined(__OPENORBIS__)
		SceNetSockaddrIn NetworkServer;
#endif
	};
}