#pragma once

#include "MessageHandler.h"

#if __clang__
#include <netinet/in.h>
#include <arpa\inet.h>
#include <stddef.h>
#elif _WIN32 || _WIN64
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")
#endif

namespace xUtilty
{
	class PersistentSocketConnection
	{
	public:
		enum
		{
			kClient = 0,
			kServer = 1,
		};
	public:
		PersistentSocketConnection();
		~PersistentSocketConnection();

		// add
		int		Initialise();
		int		Connect(char* ip, int port = 8082);
		int		Disconnect();

		int		Send(const void*, size_t);
		int		Recive(const void*, size_t);

		template <typename T>
		int		SendT(const T* data)
		{
			auto ret = Send((const void*)data, sizeof(T));
			return ret;
		}

		template <typename T>
		int ReciveT(const T* data)
		{
			auto ret = Recive((const void*)data, sizeof(T));
			return ret;
		}

		int SendString(const char* a_string)
		{
			return Send((const void*)a_string, strlen(a_string) + 1);
		}

		static PersistentSocketConnection& get()
		{
			static PersistentSocketConnection singleton;
			return singleton;
		}
	private:
#if __clang__
		int			 socketHandle { -1 };
		sockaddr_in  server;
#elif _WIN32 || _WIN64
		SOCKET		 socketHandle[2];
		sockaddr_in  server[2];
#endif
	};
}