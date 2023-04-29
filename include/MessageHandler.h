#pragma once

// THINKING
#include "../include/Macro.h"

#include <stdint.h>
#include <cstring>
#include <stdio.h>
#include <stdarg.h>

// print the current code pos into the kernel log(debug)
#ifndef PRINT_POS
#define PRINT_POS xUtilty::KernelPrintOut("%s %s %d", __FUNCTION__, __FILE__, __LINE__);
#define PRINT_POS_N xUtilty::Notify("%s %s %d", __FUNCTION__, __FILE__, __LINE__);
#endif

// print the current code pos into the kernel log and return(debug)
#ifndef PRINT_POS_RETURN 
#define PRINT_POS_RETURN xUtilty::KernelPrintOut("%s %s %d %s", __FUNCTION__, __FILE__, __LINE__, THINKING); return;
#define PRINT_POS_RETURN_N xUtilty::Notify("%s %s %d %s", __FUNCTION__, __FILE__, __LINE__, THINKING); return;

#endif

// print the current code pos + custom fmt + string into the kernel log 
#ifndef PRINT_FMT
#define PRINT_FMT(fmt, ...) xUtilty::KernelPrintOut("%s %s %d %s " fmt, __FUNCTION__, __FILE__, __LINE__, THINKING, __VA_ARGS__);
#define PRINT_FMT_N(fmt, ...) xUtilty::Notify("%s %s %d %s " fmt, __FUNCTION__, __FILE__, __LINE__, THINKING, __VA_ARGS__);
#endif

// print the current code pos + custom fmt + string into the kernel log and return T
#ifndef PRINT_FMT_RETURN_T
#define PRINT_FMT_RETURN_T(T, fmt, ...) xUtilty::KernelPrintOut("%s %s %d %s "  fmt, __FUNCTION__,__FILE__, __LINE__, THINKING, __VA_ARGS__); return T;
#define PRINT_FMT_RETURN_T_N(T, fmt, ...) xUtilty::Notify("%s %s %d %s "  fmt, __FUNCTION__,__FILE__, __LINE__, THINKING, __VA_ARGS__); return T;
#endif

// print the current code pos + custom fmt + string into the kernel log and return(debug)
#ifndef PRINT_RETURN
#define PRINT_RETURN(fmt, ...) xUtilty::KernelPrintOut("%s %s %d %s "  fmt, __FUNCTION__, __FILE__, __LINE__, THINKING, __VA_ARGS__); return;
#define PRINT_RETURN_N(fmt, ...) xUtilty::Notify("%s %s %d %s "  fmt, __FUNCTION__, __FILE__, __LINE__, THINKING, __VA_ARGS__); return;

#endif

// print the current code pos + custom string into the kernel log and return(debug)
#ifndef PRINT_STRING_RETURN 
#define PRINT_STRING_RETURN(string)  xUtilty::KernelPrintOut("%s %s %d %s", __FUNCTION__, __FILE__, __LINE__, string); return;
#define PRINT_STRING_RETURN_N(string)  xUtilty::Notify("%s %s %d %s", __FUNCTION__, __FILE__, __LINE__, string); return;
#endif

// print the current code pos + custom string into the kernel log(debug)
#ifndef PRINT_STRING
#define PRINT_STRING(string)  xUtilty::KernelPrintOut("%s %s %d %s", __FUNCTION__,__FILE__, __LINE__, string);
#define PRINT_STRING_N(string)  xUtilty::Notify("%s %s %d %s", __FUNCTION__,__FILE__, __LINE__, string);
#endif

namespace xUtilty
{
	enum MessageType
	{
		Local,
		Kernel,
		Visual
	};

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