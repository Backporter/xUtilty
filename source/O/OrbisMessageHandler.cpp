#include <stdint.h>
#include <iostream>

#include "../O/OrbisMessageHandler.h"
#include "../O/OrbisSystemWrapper.h"

namespace OrbisMessageHandler
{
	void LocalPrint(const char * FMT, ...)
	{
		char buffer[1024 * 8];
		va_list args;
		va_start(args, FMT);
		vsprintf(buffer, FMT, args);
		va_end(args);
	}

	void Notify(const char* MessageFMT, ...)
	{
		NotifyBuffer Buffer;

		va_list args;
		va_start(args, MessageFMT);
		vsprintf(Buffer.Message, MessageFMT, args);
		va_end(args);

		Buffer.Type = NotifyType::NotificationRequest;
		Buffer.unk3 = 0;
		Buffer.UseIconImageUri = 1;
		Buffer.TargetId = -1;
		strcpy(Buffer.Uri, "https://www.akcpetinsurance.com/res/akc/images/icons/home/home_dog.png");

		if (!OrbisSystemWrapper::sceKernelSendNotificationRequest)
		{
			LocalPrint("OrbisSystemWrapper::sceKernelSendNotificationRequest Is not dlysm'ed redirecting call to prevent crash");
			return;
		}

		OrbisSystemWrapper::sceKernelSendNotificationRequest(0, (char*)&Buffer, 3120, 0);
		KernelPrintOut(Buffer.Message, "\n");
	}

	void KernelPrintOut(const char* MessageFMT, ...)
	{
		char MessageBuf[1024*8];
		va_list args;
		va_start(args, MessageFMT);
		vsprintf(MessageBuf, MessageFMT, args);
		va_end(args);

		if (!OrbisSystemWrapper::sceKernelDebugOutText)
		{
			LocalPrint("OrbisSystemWrapper::sceKernelDebugOutText Is not dlysm'ed redirecting call to prevent crash");
			return;
		}

		OrbisSystemWrapper::sceKernelDebugOutText(0, MessageBuf);
		OrbisSystemWrapper::sceKernelDebugOutText(0, "\n");
	}
}