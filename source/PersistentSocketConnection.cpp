#include "../include/PersistentSocketConnection.h"

#if __clang__
#include <sys/socket.h>
#include <_fs.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <stddef.h>
#include <string.h>
#elif _WIN32 || _WIN64
#include <winsock2.h>
#endif

namespace xUtilty
{
	PersistentSocketConnection::PersistentSocketConnection()
	{}

	PersistentSocketConnection::~PersistentSocketConnection()
	{}

	int PersistentSocketConnection::Initialise()
	{
#if _WIN32 || _WIN64
		WSADATA wsa;

		KernelPrintOut("Initialising Winsock...\n");
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		{
			KernelPrintOut("Failed. Error Code : %d", WSAGetLastError());
			return 1;
		}
		KernelPrintOut("initialized Winsock...\n");
#endif

		return 0;
	}

	int	PersistentSocketConnection::Connect(char* a_ip, int port)
	{
		KernelPrintOut("Starting Socket on IP:PORT -> %s:%d\n", a_ip, port);

#if __clang__
		if (!a_ip)
		{
			// ..
			return -1;
		}

		socketHandle = socket(AF_INET, SOCK_STREAM, 0);
		if (socketHandle == -1)
		{
			// ..
			return -1;
		}


		memset(&server, 0, sizeof(server));
		server.sin_len = sizeof(server);
		server.sin_family = 2;
		server.sin_port = htons(port);

		if (inet_pton(2, a_ip, &server.sin_addr) <= 0)
		{
			return -1;
		}

		if (connect(socketHandle, (const sockaddr *)&server, sizeof(server)) < 0)
		{
			return -1;
		}
#elif _WIN32 || _WIN64
		int ret;
		int c;
		bool opt = TRUE;

		//
		if ((socketHandle[kServer] = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
		{
			KernelPrintOut("Could not create socket : %d\n", WSAGetLastError());
		}

		//
		ret = setsockopt(socketHandle[kServer], SOL_SOCKET, SO_KEEPALIVE, (char*)&opt, sizeof(bool));
		if (ret == SOCKET_ERROR)
		{
			exit(EXIT_FAILURE);
		}

		//
		server[kServer].sin_family = AF_INET;
		server[kServer].sin_addr.s_addr = INADDR_ANY;
		server[kServer].sin_port = htons(port);

		//
		if (bind(socketHandle[kServer], (struct sockaddr *)&server[kServer], sizeof(server[kServer])) == SOCKET_ERROR)
		{
			KernelPrintOut("Bind failed with error code : %d\n", WSAGetLastError());
		}

		//
		listen(socketHandle[kServer], 3);

		//
		KernelPrintOut("Waiting for incoming connections...\n");

		//
		c = sizeof(struct sockaddr_in);
		socketHandle[kClient] = accept(socketHandle[kServer], (struct sockaddr *)&server[kClient], &c);
		if (socketHandle[kClient] == INVALID_SOCKET)
		{
			printf("accept failed with error code : %d\n", WSAGetLastError());
		}

		KernelPrintOut("Connection accepted\n");
#endif

		return 0;
	}

	int	PersistentSocketConnection::Disconnect()
	{
#if __clang__
		if (socketHandle != -1)
		{
			shutdown(socketHandle, SHUT_RDWR);
			close(socketHandle);
		}

		socketHandle = -1;
		return 0;
#elif _WIN32 || _WIN64
		closesocket(socketHandle[kServer]);
		WSACleanup();
		return 0;
#endif
	}

	int	PersistentSocketConnection::Send(const void* a_data, size_t a_DataSize)
	{
#if __clang__
		return send(socketHandle, a_data, a_DataSize, 0);
#elif _WIN32 || _WIN64
		return send(socketHandle[kClient], (const char*)a_data, a_DataSize, 0);
#endif
	}
	
	int	PersistentSocketConnection::Recive(const void* a_data, size_t a_DataSize)
	{
#if __clang__
		return recv(socketHandle, (void*)a_data, a_DataSize, 0);
#elif _WIN32 || _WIN64
		return recv(socketHandle[kClient], (char*)a_data, a_DataSize, 0);
#endif
	}
}
