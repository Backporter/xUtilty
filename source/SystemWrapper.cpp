#include <stdint.h>
#include <string>
#include <iostream>

#include "../include/SystemWrapper.h"
#include "../include/MessageHandler.h"
#include "../include/FileSystem.h"
#include "../include/OrbisOffsertManger.h"
#include "../include/OrbisSFOHandler.h"
#include "../include/INIHandler.h"

#if defined(__ORBIS__)
#include <kernel.h>
#include <rtc.h>
#elif defined(__OPENORBIS__)
#include <orbis/libkernel.h>
#include <orbis/Rtc.h>
#endif

#include <assert.h>
namespace OrbisSystemWrapper
{
	int sysKernelGetLowerLimitUpdVersionret = 0;
	int sysKernelGetLowerLimitUpdVersionarg = 0;
	int sysKernelGetUpdVersionarg = 0;
	int sysKernelGetUpdVersionret = 0;

#if defined (__ORBIS__) || defined(__OPENORBIS__)
	jbc_cred Orginalcred;
	jbc_cred RootCred;
	OrbisKernelFirmwareInfo Firmware;

	int32_t libkernelhandle;
	int32_t libkernelsyshandle;
	int32_t libSceNpTrophyhandle;
	int32_t libjbchandle;
	int32_t libchandle;

	int(*ioctl)(int fd, unsigned long request, ...);
	int(*sysconf)(int num);
	int(*sceKernelGetSystemSwVersion)(OrbisKernelFirmwareInfo * data);
	int(*sysKernelGetUpdVersion)(int* unk);
	int(*sysKernelGetLowerLimitUpdVersion)(int* unk);
	int64_t(*sceKernelSendNotificationRequest)(int64_t unk1, char* Buffer, size_t size, int64_t unk2);
	char*(*sceKernelGetFsSandboxRandomWord)();
	int(*sceKernelDebugOutText)(int Channel, const char* text);
	int(*sceKernelInternalMemoryGetAvailableSize)(size_t*);
	uint64_t(*sceKernelGetCpuFrequency)();
	int(*sceKernelGetCpuUsage)(struct proc*, int*);
	int(*sceKernelGetThreadName)(unsigned int, char*);

	int(*jbc_get_cred)(jbc_cred *ans);
	int(*jbc_jailbreak_cred)(jbc_cred *ans);
	int(*jbc_set_cred)(const jbc_cred *ans);
	void(*jbc_run_as_root)(void(*fn)(void *arg), void *arg, int cwd_mode);
	int(*jbc_mount_in_sandbox)(const char *system_path, const char *mnt_name);
	int(*jbc_unmount_in_sandbox)(const char *mnt_name);

	void Jailbreak()
	{
		Orginalcred = jbc_cred();
		RootCred = jbc_cred();

		if (jbc_get_cred)
		{
			jbc_get_cred(&Orginalcred);
		}

		if (jbc_jailbreak_cred)
		{
			jbc_jailbreak_cred(&RootCred);
		}

		if (jbc_set_cred)
		{
			jbc_set_cred(&RootCred);
		}
	}

	void Jail()
	{
		if (jbc_set_cred)
		{
			jbc_set_cred(&Orginalcred);
		}
	}

	bool MountUSB(int index)
	{
		assert(index <= 7);

		char fullpath[11]{ 0 };
		char sandbox[8]{ 0 };
		char sandboxname[6]{ 0 };

		sprintf(fullpath, "/mnt/usb%d/", index);
		sprintf(sandbox, "/usb_%d/", index);
		sprintf(sandboxname, "usb_%d", index);

		// dir is already mounted
		if (OrbisFileSystem::PathExists(OrbisFileSystem::Full, sandbox, true))
			return false;

		// if the function pointer is valid, call it.
		if (OrbisSystemWrapper::jbc_mount_in_sandbox)
			OrbisSystemWrapper::jbc_mount_in_sandbox(fullpath, sandboxname);

		// confirm it mounted
		return OrbisFileSystem::PathExists(OrbisFileSystem::Full, sandbox, true);
	}

	bool UnmountUSB(int index)
	{
		assert(index <= 7);

		char fullpath[11]{ 0 };
		char sandbox[8]{ 0 };
		char sandboxname[6]{ 0 };

		sprintf(fullpath, "/mnt/usb%d/", index);
		sprintf(sandbox, "/usb_%d/", index);
		sprintf(sandboxname, "usb_%d", index);

		// dir is already unmounted
		if (!OrbisFileSystem::PathExists(OrbisFileSystem::Full, sandbox, true))
			return false;

		// if the function pointer is valid, call it.
		if (OrbisSystemWrapper::jbc_unmount_in_sandbox)
			OrbisSystemWrapper::jbc_unmount_in_sandbox(sandboxname);

		return OrbisFileSystem::PathExists(OrbisFileSystem::Full, sandbox, true);
	}

	void UpdateRTC(SceRtcDateTime* now)
	{
		if (sceRtcGetCurrentClockLocalTime(now) != 0)
			memset(now, 0, sizeof(SceRtcDateTime));
	}

	bool Dump(const char* TypeName, void* base, int size)
	{
		char path[260]{ 0 };
		SceRtcDateTime now;

		OrbisSystemWrapper::UpdateRTC(&now);
		sprintf(path, "DUMPS/%s%d%d%d%d%d.dump", TypeName, now.day, now.hour, now.minute, now.second, now.microsecond);

		if (OrbisFileSystem::PathExists(OrbisFileSystem::USB0, "DUMMY/dummy.txt", false))
			OrbisFileSystem::WriteFileToDisk(OrbisFileSystem::USB0, path, base, size);

		else if (OrbisFileSystem::PathExists(OrbisFileSystem::USB1, "DUMMY/dummy.txt", false))
			OrbisFileSystem::WriteFileToDisk(OrbisFileSystem::USB1, path, base, size);

		return true;
	}
#endif

	bool Inited = false;

	void initialize()
	{
#if defined (__ORBIS__) || defined(__OPENORBIS__)
		const char* libjbcpath = "/app0/modules/libjbc.prx";
		if (!OrbisFileSystem::PathExists(OrbisFileSystem::Full, libjbcpath, false))
		{
			libjbcpath = "/data/modules/libjbc.prx";
		}

		if (Inited) 
		{
			return;
		}

		int ret = 0;
		
		libkernelhandle = sceKernelLoadStartModule("libkernel.sprx", 0, 0, 0, 0, 0);
		if (libkernelhandle > 0)
		{
			if ((ret = sceKernelDlsym(libkernelhandle, "sceKernelSendNotificationRequest", (void**)&sceKernelSendNotificationRequest)) != 0)
			{
				MessageHandler::KernelPrintOut("Failed to get function address for sceKernelSendNotificationRequest");
			}

			if ((ret = sceKernelDlsym(libkernelhandle, "sceKernelDebugOutText", (void**)&sceKernelDebugOutText)) != 0)
			{
				MessageHandler::KernelPrintOut("Failed to get function address for sceKernelDebugOutText");
			}

			if ((ret = sceKernelDlsym(libkernelhandle, "sceKernelGetSystemSwVersion", (void**)&sceKernelGetSystemSwVersion)) != 0)
			{
				MessageHandler::KernelPrintOut("Failed to get function address for sceKernelGetSystemSwVersion");
			}

			if ((ret = sceKernelDlsym(libkernelhandle, "sysKernelGetUpdVersion", (void**)&sysKernelGetUpdVersion)) != 0)
			{
				MessageHandler::KernelPrintOut("Failed to get function address for sysKernelGetUpdVersion");
			}

			if ((ret = sceKernelDlsym(libkernelhandle, "sysKernelGetLowerLimitUpdVersion", (void**)&sysKernelGetLowerLimitUpdVersion)) != 0)
			{
				MessageHandler::KernelPrintOut("Failed to get function address for sysKernelGetLowerLimitUpdVersion");
			}

			if ((ret = sceKernelDlsym(libkernelhandle, "sysconf", (void**)&sysconf)) != 0)
			{
				MessageHandler::KernelPrintOut("Failed to get function address for sysconf");
			}

			if ((ret = sceKernelDlsym(libkernelhandle, "ioctl", (void**)&ioctl)) != 0)
			{
				MessageHandler::KernelPrintOut("Failed to get function address for ioctl");
			}

			if ((ret = sceKernelDlsym(libkernelhandle, "sceKernelGetFsSandboxRandomWord", (void**)&sceKernelGetFsSandboxRandomWord)) != 0)
			{
				MessageHandler::KernelPrintOut("Failed to get function address for sceKernelGetFsSandboxRandomWord");
			}

			if ((ret = sceKernelDlsym(libkernelhandle, "sceKernelInternalMemoryGetAvailableSize", (void**)&sceKernelInternalMemoryGetAvailableSize)) != 0)
			{
				MessageHandler::KernelPrintOut("Failed to get function address for sceKernelInternalMemoryGetAvailableSize");
			}

			if ((ret = sceKernelDlsym(libkernelhandle, "sceKernelGetCpuFrequency", (void**)&sceKernelGetCpuFrequency)) != 0)
			{
				MessageHandler::KernelPrintOut("Failed to get function address for sceKernelGetCpuFrequency");
			}

			if ((ret = sceKernelDlsym(libkernelhandle, "sceKernelGetCpuUsage", (void**)&sceKernelGetCpuUsage)) != 0)
			{
				MessageHandler::KernelPrintOut("Failed to get function address for sceKernelGetCpuUsage");
			}

			if ((ret = sceKernelDlsym(libkernelhandle, "sceKernelGetThreadName", (void**)&sceKernelGetThreadName)) != 0)
			{
				MessageHandler::KernelPrintOut("Failed to get function address for sceKernelGetCpuUsage");
			}
		}
		else 
		{
			MessageHandler::KernelPrintOut("Failed To Load libkernel.sprx");
		}

		libjbchandle = sceKernelLoadStartModule(libjbcpath, 0, 0, 0, 0, 0);
		if (libjbchandle > 0)
		{
			if ((ret = sceKernelDlsym(libjbchandle, "jbc_get_cred", (void**)&jbc_get_cred)) != 0)
			{
				MessageHandler::KernelPrintOut("Failed to get function address for sceKernelGetFsSandboxRandomWord");
			}

			if ((ret = sceKernelDlsym(libjbchandle, "jbc_jailbreak_cred", (void**)&jbc_jailbreak_cred)) != 0)
			{
				MessageHandler::KernelPrintOut("Failed to get function address for sceKernelGetFsSandboxRandomWord");
			}

			if ((ret = sceKernelDlsym(libjbchandle, "jbc_set_cred", (void**)&jbc_set_cred)) != 0)
			{
				MessageHandler::KernelPrintOut("Failed to get function address for sceKernelGetFsSandboxRandomWord");
			}

			if ((ret = sceKernelDlsym(libjbchandle, "jbc_run_as_root", (void**)&jbc_run_as_root)) != 0)
			{
				MessageHandler::KernelPrintOut("Failed to get function address for sceKernelGetFsSandboxRandomWord");
			}

			if ((ret = sceKernelDlsym(libjbchandle, "jbc_mount_in_sandbox", (void**)&jbc_mount_in_sandbox)) != 0)
			{
				MessageHandler::KernelPrintOut("Failed to get function address for sceKernelGetFsSandboxRandomWord");
			}

			if ((ret = sceKernelDlsym(libjbchandle, "jbc_unmount_in_sandbox", (void**)&jbc_unmount_in_sandbox)) != 0)
			{
				MessageHandler::KernelPrintOut("Failed to get function address for sceKernelGetFsSandboxRandomWord");
			}

		}
		else
		{
			MessageHandler::KernelPrintOut("Failed To Load libjbc");
		}

		if (sceKernelGetSystemSwVersion)
		{
			Firmware = OrbisKernelFirmwareInfo();
			Firmware.Size = sizeof(OrbisKernelFirmwareInfo);
			sceKernelGetSystemSwVersion(&Firmware);
		}

		if (jbc_set_cred)
		{
			Jailbreak();

			if (sysKernelGetLowerLimitUpdVersion)
			{
				sysKernelGetLowerLimitUpdVersionret = sysKernelGetLowerLimitUpdVersion(&sysKernelGetLowerLimitUpdVersionarg);
			}

			if (sysKernelGetUpdVersion)
			{
				sysKernelGetUpdVersionret = sysKernelGetUpdVersion(&sysKernelGetUpdVersionarg);
			}

			Jail();
		}
		else
		{
			sysKernelGetLowerLimitUpdVersionarg = Firmware.Version;
		}
#endif
		Inited = true;
#if defined (__ORBIS__) || defined(__OPENORBIS__)
		OrbisSFOHandler::sfo_parser::GetSingleton()->ParseCurrentProcessSFO();
#endif
		OrbisOffsetManger::OffsetManger::GetSingleton()->Initialize();
		OrbisINIHandler::OrbisINIHandler::GetSingleton()->ParseCSELINI();
	}


	//bool MountUSB0(int index)
	//{
	//		// dir is already mounted
	//		if (OrbisFileSystem::DirectoryExists(OrbisFileSystem::Full, "/usb_0/"))
	//			return false;
	//
	//	// if the function pointer is valid, call it.
	//	if (OrbisSystemWrapper::jbc_mount_in_sandbox)
	//		OrbisSystemWrapper::jbc_mount_in_sandbox("/mnt/usb0/", "usb_0");
	//
	//	// confirm it mounted
	//	return OrbisFileSystem::DirectoryExists(OrbisFileSystem::Full, "/usb_0/");
	//}

	//bool UnmountUSB(int)
	//{
	//	// dir is already unmounted
	//	if (!OrbisFileSystem::DirectoryExists(OrbisFileSystem::Full, "/usb_0/"))
	//		return false;
	//
	//	// if the function pointer is valid, call it.
	//	if (OrbisSystemWrapper::jbc_unmount_in_sandbox)
	//		OrbisSystemWrapper::jbc_unmount_in_sandbox("usb_0");
	//
	//	return OrbisFileSystem::DirectoryExists(OrbisFileSystem::Full, "/usb_0/");
	//}
}