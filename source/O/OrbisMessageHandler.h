#pragma once

#include <stdint.h>
#include "../E/EMOJI.h"

namespace OrbisMessageHandler
{
	enum NotifyType
	{
		NotificationRequest = 0,
		SystemNotification = 1,
		SystemNotificationWithUserId = 2,
		SystemNotificationWithDeviceId = 3,
		SystemNotificationWithDeviceIdRelatedToUser = 4,
		SystemNotificationWithText = 5,
		SystemNotificationWithTextRelatedToUser = 6,
		SystemNotificationWithErrorCode = 7,
		SystemNotificationWithAppId = 8,
		SystemNotificationWithAppName = 9,
		SystemNotificationWithAppInfo = 9,
		SystemNotificationWithAppNameRelatedToUser = 10,
		SystemNotificationWithParams = 11,
		SendSystemNotificationWithUserName = 12,
		SystemNotificationWithUserNameInfo = 13,
		SendAddressingSystemNotification = 14,
		AddressingSystemNotificationWithDeviceId = 15,
		AddressingSystemNotificationWithUserName = 16,
		AddressingSystemNotificationWithUserId = 17,
		UNK_1 = 100,
		TrcCheckNotificationRequest = 101,
		NpDebugNotificationRequest = 102,
		UNK_2 = 102,
	};

	struct NotifyBuffer
	{
		NotifyType Type;
		int ReqId;
		int Priority;
		int MsgId;
		int TargetId;
		int UserId;
		int unk1;
		int unk2;
		int AppId;
		int ErrorNum;
		int unk3;
		char UseIconImageUri;
		char Message[1024];
		char Uri[1024];
		char unkstr[1024];
	};

	// self contained vsprinf, if patches are applied this will do the same thing KernelPrintOut does.
	void LocalPrint(const char * FMT, ...);

	// Pushes a Notifcation and writes to the kernel log
	void Notify(const char* MessageFMT, ...);
	
	// writes to the kernel log
	void KernelPrintOut(const char* MessageFMT, ...);
}