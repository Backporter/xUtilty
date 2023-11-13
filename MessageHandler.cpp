#include "../include/MessageHandler.h"

//
#include "../include/SystemWrapper.h"
#include "../include/INIHandler.h"

#include <stdint.h>
#include <iostream>
#include <cstdio>
#include <time.h>

namespace xUtilty
{
	void LocalPrint(const char* a_fmt, ...)
	{
		//
		char buffer[1024];


		va_list args;
		va_start(args, a_fmt);
		size_t length = SystemWrapper::vsprintf(buffer, a_fmt, args);
		va_end(args);
	}

	void Notify(const char* a_fmt, ...)
	{
		//
		NotifyBuffer Buffer;

		va_list args;
		va_start(args, a_fmt);
		size_t length = SystemWrapper::vsprintf(Buffer.Message, a_fmt, args);
		va_end(args);

		Buffer.Type = NotifyType::NotificationRequest;
		Buffer.unk3 = 0;
		Buffer.UseIconImageUri = 1;
		Buffer.TargetId = -1;

		auto* INI = INIHandler::INIHandler::GetSingleton();
		if (INI->GetINIOptions()->UseCustomIconURL)
		{
			strncpy(Buffer.Uri, INI->GetINIOptions()->Icon, sizeof(Buffer.Uri));
		}
		else
		{
			strncpy(Buffer.Uri, "https://www.akcpetinsurance.com/res/akc/images/icons/home/home_dog.png", sizeof(Buffer.Uri));
		}

#if defined (__ORBIS__) || defined(__OPENORBIS__)
		SystemWrapper::sceKernelSendNotificationRequest(0, (char*)&Buffer, 3120, 0);
		KernelPrintOut(Buffer.Message, "\n");

#elif defined(__SWITCH__) || defined(PLATFORM_NX)
		svcOutputDebugString(Buffer.Message, length);
#elif defined(_WIN32) || defined(_WIN64)
		printf(Buffer.Message);
#endif
	}

	void KernelPrintOut(const char* a_fmt, ...)
	{
		//
		int64_t s_time;
		char MessageBuf[1024];

		time(&s_time);
		auto time = localtime(&s_time);
		strftime(MessageBuf, 1024, "[%m/%d/%Y - %I:%M:%S%p] ", time);

		// stub the size.
		auto timelen = strlen(MessageBuf);

		va_list args;
		va_start(args, a_fmt);
		size_t length = SystemWrapper::vsprintf(&MessageBuf[timelen], a_fmt, args);
		va_end(args);

#if defined (__ORBIS__) || defined(__OPENORBIS__)
		SystemWrapper::sceKernelDebugOutText(0, MessageBuf);
		SystemWrapper::sceKernelDebugOutText(0, "\n");
#elif defined(__SWITCH__) || defined(PLATFORM_NX)
		svcOutputDebugString(MessageBuf, length);
#elif defined(_WIN32) || defined(_WIN64)
		printf(MessageBuf);
#endif
	}

	void CoredumpPrintOut(const char* a_fmt, ...)
	{
		//
		int64_t s_time;
		char MessageBuf[1024];

		time(&s_time);
		auto time = localtime(&s_time);
		strftime(MessageBuf, 1024, "[%m/%d/%Y - %I:%M:%S%p] ", time);

		// stub the size.
		auto timelen = strlen(MessageBuf);

		va_list args;
		va_start(args, a_fmt);
		size_t length = SystemWrapper::vsprintf(&MessageBuf[timelen], a_fmt, args);
		va_end(args);

#if defined (__ORBIS__) || defined(__OPENORBIS__)
		SystemWrapper::sceCoredumpDebugTextOut(MessageBuf, length);
		SystemWrapper::sceCoredumpDebugTextOut("\n", length);
#elif defined(__SWITCH__) || defined(PLATFORM_NX)
		svcOutputDebugString(MessageBuf, length);
#elif defined(_WIN32) || defined(_WIN64)
		printf(MessageBuf);
#endif
	}

	void PrintNeighborhood(const char* a_fmt, ...) 
	{
		for (const char* msg = a_fmt; *msg != '\0'; ++msg) {
			putchar(*msg);
		}
		fflush(stdout);
	}

}