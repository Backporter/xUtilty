#include <stdint.h>
#include <iostream>

#include "../include/MessageHandler.h"
#include "../include/SystemWrapper.h"
#include "../include/INIHandler.h"

#include <time.h>
namespace MessageHandler
{
	void LocalPrint(const char * FMT, ...)
	{
		char buffer[1024];
		va_list args;
		va_start(args, FMT);
		size_t length = SystemWrapper::vsprintf(buffer, FMT, args);
		va_end(args);
	}

	void Notify(const char* MessageFMT, ...)
	{
		NotifyBuffer Buffer;
		va_list args;
		va_start(args, MessageFMT);
		size_t length = SystemWrapper::vsprintf(Buffer.Message, MessageFMT, args);
		va_end(args);

		Buffer.Type = NotifyType::NotificationRequest;
		Buffer.unk3 = 0;
		Buffer.UseIconImageUri = 1;
		Buffer.TargetId = -1;
		
		auto* INI = OrbisINIHandler::OrbisINIHandler::GetSingleton();
		if (INI->GetINIOptions()->UseCustomIconURL)
		{
			strncpy(Buffer.Uri, OrbisINIHandler::OrbisINIHandler::GetSingleton()->GetINIOptions()->Icon, sizeof(Buffer.Uri));
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
#endif
	}

	void KernelPrintOut(const char* MessageFMT, ...)
	{
		int64_t s_time;
		char MessageBuf[1024];

		time(&s_time);
		auto time = localtime(&s_time);
		std::strftime(MessageBuf, 1024, "[%m/%d/%Y - %I:%M:%S%p] ", time);
		
		// stub the size.
		auto timelen = strlen(MessageBuf);
		
		va_list args;
		va_start(args, MessageFMT);
		size_t  length = SystemWrapper::vsprintf(&MessageBuf[timelen], MessageFMT, args);
		va_end(args);

		// copy buf


#if defined (__ORBIS__) || defined(__OPENORBIS__)
		SystemWrapper::sceKernelDebugOutText(0, MessageBuf);
		SystemWrapper::sceKernelDebugOutText(0, "\n");

#elif defined(__SWITCH__) || defined(PLATFORM_NX)
		svcOutputDebugString(MessageBuf, length);
#endif
	}
}