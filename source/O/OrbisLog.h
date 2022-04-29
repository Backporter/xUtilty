#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

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

namespace OrbisLog
{
	class OrbisLog
	{
	public:
		OrbisLog();
		virtual ~OrbisLog();

		// Open's the log with a full path, exactly like OpenRelitive minus the ID
		virtual bool Open(const char* path);

		// This will open the log with the Relitive ID + path, ex OpenRelitive(OrbisFileSystem::Download, "OCEL/log.txt"); = "/download0/OCEL/log.txt"
		// ID matches the ID in OrbisFileSystem::XXXX
		virtual bool OpenRelitive(int ID, const char* path);

		// network version of this->Open(), this network stuff, so it's cleaner to do it in a different method
		// BEWARE, blocking function.... 
		virtual bool Connect(const char* IP);

		// closes log
		virtual bool Close();

		// Confirms the file descriptor is still vaild
		virtual bool Vaild();

		// network version of this->Close(), network stuff is seperated for cleaner code
		// BEWARE, blocking function.... 
		virtual bool Disconnect();

		// writes a message to the log
		virtual bool Write(const char* fmt, ...);

		// writes a message to the log
		virtual bool WriteVA(uint32_t type, const char* fmt, va_list list);

		// network version of this->write(), network stuff is seperated for cleaner code... use this->Write as it works too.
		// BEWARE, blocking function.... 
		virtual bool Send(const char* MessageFMT, ...);

		// Singleton implementation for a single class
		static OrbisLog* GetSingleton()
		{
			static OrbisLog _OrbisLog;
			return &_OrbisLog;
		}

	private:
		// file descriptor when a local file is used, socket when the network is used
		int FD;
		int ret;
		const char* LogPath;

		bool UseNet;
		int opt;
		SceNetSockaddrIn NetworkServer;
	};
}

void PrintToLog(char* rdi);