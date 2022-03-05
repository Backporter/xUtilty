#pragma once

#if defined(__ORBIS__) && !defined(__OPENORBIS__)
#include <kernel.h>
#include <net.h>
#else
#include <orbis/net.h>
#include <orbis/libkernel.h>
typedef sockaddr_in SceNetSockaddrIn;
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
		virtual bool Connect(const char* IP);

		// closes log
		virtual bool Close();

		// network version of this->Close(), network stuff is seperated for cleaner code
		virtual bool Disconnect();

		// writes a message to the log
		virtual bool Write(const char* fmt, ...);

		// network version of this->write(), network stuff is seperated for cleaner code
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
		SceNetSockaddrIn NetworkServer;
	};
}