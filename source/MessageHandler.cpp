#include <stdint.h>
#include <iostream>

#include "../include/MessageHandler.h"
#include "../include/SystemWrapper.h"
#include "../include/INIHandler.h"

namespace MessageHandler
{
	void LocalPrint(const char * FMT, ...)
	{
		char buffer[1024];
		va_list args;
		va_start(args, FMT);
		size_t length = vsprintf(buffer, FMT, args);
		va_end(args);
	}

	void Notify(const char* MessageFMT, ...)
	{
		NotifyBuffer Buffer;
		va_list args;
		va_start(args, MessageFMT);
		size_t length = vsprintf(Buffer.Message, MessageFMT, args);
		va_end(args);

		Buffer.Type = NotifyType::NotificationRequest;
		Buffer.unk3 = 0;
		Buffer.UseIconImageUri = 1;
		Buffer.TargetId = -1;
		strncpy(Buffer.Uri, OrbisINIHandler::OrbisINIHandler::GetSingleton()->GetINIOptions()->Icon, sizeof(Buffer.Uri));

#if defined (__ORBIS__) || defined(__OPENORBIS__)

		if (!OrbisSystemWrapper::sceKernelSendNotificationRequest)
		{
			LocalPrint("OrbisSystemWrapper::sceKernelSendNotificationRequest Is not dlysm'ed redirecting call to prevent crash");
			return;
		}

		OrbisSystemWrapper::sceKernelSendNotificationRequest(0, (char*)&Buffer, 3120, 0);
		KernelPrintOut(Buffer.Message, "\n");

#elif defined(__SWITCH__) || defined(PLATFORM_NX)
		svcOutputDebugString(Buffer.Message, length);
#endif
	}

	void KernelPrintOut(const char* MessageFMT, ...)
	{
		char MessageBuf[1024];
		va_list args;
		va_start(args, MessageFMT);
		size_t length = vsprintf(MessageBuf, MessageFMT, args);
		va_end(args);

#if defined (__ORBIS__) || defined(__OPENORBIS__)
		if (!OrbisSystemWrapper::sceKernelDebugOutText)
		{
			LocalPrint("OrbisSystemWrapper::sceKernelDebugOutText Is not dlysm'ed redirecting call to prevent crash");
			return;
		}

		OrbisSystemWrapper::sceKernelDebugOutText(0, MessageBuf);
		OrbisSystemWrapper::sceKernelDebugOutText(0, "\n");

#elif defined(__SWITCH__) || defined(PLATFORM_NX)
		svcOutputDebugString(MessageBuf, length);
#endif
	}
}