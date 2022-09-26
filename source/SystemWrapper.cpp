#include <stdint.h>
#include <string>
#include <iostream>

#include "../include/SafeTypes.h"
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

#if defined (__ORBIS__) || defined(__OPENORBIS__)
#include "../include/Mira.h"
#endif

#include <assert.h>

namespace OrbisSystemWrapper
{
	int sysKernelGetLowerLimitUpdVersionret = 0;
	int sysKernelGetLowerLimitUpdVersionarg = 0;
	int sysKernelGetUpdVersionarg = 0;
	int sysKernelGetUpdVersionret = 0;

#if defined (__ORBIS__) || defined(__OPENORBIS__)
	std::vector<SceKernelModuleInfo> g_moduleList;

	jbc_cred Orginalcred;
	jbc_cred RootCred;
	OrbisKernelFirmwareInfo Firmware;

	int32_t libchandle;
	int32_t libkernelhandle;
	int32_t libkernelsyshandle;
	int32_t libSceNpTrophyhandle;
	int32_t libjbchandle;
	
	// because OOSDK has issues with wchar_t we need to manually import the functions from the system and use them(not ideal!!! but it's the best work-around i've seen without needing to change OOSDK types)
	size_t   (*mbstowcs)(wchar_t_t* dst, const char* src, size_t len);
	size_t   (*wcstombs)(char* dst, const wchar_t_t * src, size_t len);
	int	     (*vsprintf)(char*, const char* , va_list);
	void*    (*malloc)(size_t);
	void     (*free)(void*);
	int      (*ioctl)(int fd, unsigned long request, ...);
	int      (*sceKernelGetModuleInfo)(uint32_t handle, SceKernelModuleInfo* info);
	int      (*sysconf)(int num);
	int      (*sceKernelGetSystemSwVersion)(OrbisKernelFirmwareInfo* data);
	int      (*sysKernelGetUpdVersion)(int* unk);
	int      (*sysKernelGetLowerLimitUpdVersion)(int* unk);
	int64_t  (*sceKernelSendNotificationRequest)(int64_t unk1, char* Buffer, size_t size, int64_t unk2);
	char*    (*sceKernelGetFsSandboxRandomWord)();
	int      (*sceKernelDebugOutText)(int Channel, const char* text);
	int      (*get_page_table_stats)(int, int, int*, int*);
	int      (*sceKernelInternalMemoryGetAvailableSize)(size_t*);
	uint64_t (*sceKernelGetCpuFrequency)();
	int      (*sceKernelGetCpuUsage)(struct proc*, int*);
	int      (*sceKernelGetThreadName)(unsigned int, char*);
	int      (*jbc_get_cred)(jbc_cred *ans);
	int      (*jbc_jailbreak_cred)(jbc_cred *ans);
	int      (*jbc_set_cred)(const jbc_cred *ans);
	void     (*jbc_run_as_root)(void(*fn)(void *arg), void *arg, int cwd_mode);
	int      (*jbc_mount_in_sandbox)(const char *system_path, const char *mnt_name);
	int      (*jbc_unmount_in_sandbox)(const char *mnt_name);

	int      (*pthread_mutexattr_init)    (pthread_mutexattr_t_t*);
	int      (*pthread_mutex_init)        (pthread_mutex_t_t*, const pthread_mutexattr_t_t*);
	int      (*pthread_mutexattr_destroy) (pthread_mutexattr_t_t*);
	int      (*pthread_mutex_destroy)     (pthread_mutex_t_t*);
	int      (*pthread_mutex_trylock)     (pthread_mutex_t_t*);
	int      (*pthread_mutex_lock)        (pthread_mutex_t_t*);
	int      (*pthread_mutex_unlock)      (pthread_mutex_t_t*);
#endif

	void Jailbreak()
	{
#if defined (__ORBIS__) || defined(__OPENORBIS__)
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
#endif
	}

	void Jail()
	{
#if defined (__ORBIS__) || defined(__OPENORBIS__)
		if (jbc_set_cred)
		{
			jbc_set_cred(&Orginalcred);
		}
#endif
	}

	bool MountUSB(int index)
	{
#if defined (__ORBIS__) || defined(__OPENORBIS__)
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
#elif defined(__SWITCH__) || defined(PLATFORM_NX)
		return true;
#endif
	}

	bool UnmountUSB(int index)
	{
#if defined (__ORBIS__) || defined(__OPENORBIS__)
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
#elif defined(__SWITCH__) || defined(PLATFORM_NX)
		return true;
#endif
	}

	void UpdateRTC(SceRtcDateTime* now)
	{
#if defined (__ORBIS__) || defined(__OPENORBIS__)
		if (sceRtcGetCurrentClockLocalTime(now) != 0)
			memset(now, 0, sizeof(SceRtcDateTime));
#elif defined(__SWITCH__) || defined(PLATFORM_NX)
		memset(now, 0, sizeof(SceRtcDateTime));
#endif
	}

	bool Dump(const char* TypeName, void* base, int size)
	{
#if defined (__ORBIS__) || defined(__OPENORBIS__)
		char path[260]{ 0 };
		SceRtcDateTime now;

		OrbisSystemWrapper::UpdateRTC(&now);
		sprintf(path, "DUMPS/%s%d%d%d%d%d.dump", TypeName, now.day, now.hour, now.minute, now.second, now.microsecond);

		if (OrbisFileSystem::PathExists(OrbisFileSystem::USB0, "DUMMY/dummy.txt", false))
			OrbisFileSystem::WriteFileToDisk(OrbisFileSystem::USB0, path, base, size);

		else if (OrbisFileSystem::PathExists(OrbisFileSystem::USB1, "DUMMY/dummy.txt", false))
			OrbisFileSystem::WriteFileToDisk(OrbisFileSystem::USB1, path, base, size);

		return true;
#elif defined(__SWITCH__) || defined(PLATFORM_NX)
		return true;
#endif
	}

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
		
		libchandle = sceKernelLoadStartModule("libc.sprx", 0, 0, 0, 0, 0);
		if (libchandle > 0)
		{
			if ((ret = sceKernelDlsym(libchandle, "wcstombs", (void**)&wcstombs)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for wcstombs");
			}

			if ((ret = sceKernelDlsym(libchandle, "mbstowcs", (void**)&mbstowcs)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for mbstowcs");
			}

			if ((ret = sceKernelDlsym(libchandle, "vsprintf", (void**)&vsprintf)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for vsprintf");
			}

			if ((ret = sceKernelDlsym(libchandle, "malloc", (void**)&malloc)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for malloc");
			}

			if ((ret = sceKernelDlsym(libchandle, "free", (void**)&free)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for free");
			}

		}
		else
		{
			MessageHandler::Notify("Failed To Load libc.sprx");
		}

		libkernelhandle = sceKernelLoadStartModule("libkernel.sprx", 0, 0, 0, 0, 0);
		if (libkernelhandle > 0)
		{
			if ((ret = sceKernelDlsym(libkernelhandle, "sceKernelSendNotificationRequest", (void**)&sceKernelSendNotificationRequest)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for sceKernelSendNotificationRequest");
			}

			if ((ret = sceKernelDlsym(libkernelhandle, "sceKernelDebugOutText", (void**)&sceKernelDebugOutText)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for sceKernelDebugOutText");
			}

			if ((ret = sceKernelDlsym(libkernelhandle, "sceKernelGetSystemSwVersion", (void**)&sceKernelGetSystemSwVersion)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for sceKernelGetSystemSwVersion");
			}

			if ((ret = sceKernelDlsym(libkernelhandle, "sysKernelGetUpdVersion", (void**)&sysKernelGetUpdVersion)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for sysKernelGetUpdVersion");
			}

			if ((ret = sceKernelDlsym(libkernelhandle, "sysKernelGetLowerLimitUpdVersion", (void**)&sysKernelGetLowerLimitUpdVersion)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for sysKernelGetLowerLimitUpdVersion");
			}

			if ((ret = sceKernelDlsym(libkernelhandle, "sysconf", (void**)&sysconf)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for sysconf");
			}

			if ((ret = sceKernelDlsym(libkernelhandle, "ioctl", (void**)&ioctl)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for ioctl");
			}

			if ((ret = sceKernelDlsym(libkernelhandle, "sceKernelGetFsSandboxRandomWord", (void**)&sceKernelGetFsSandboxRandomWord)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for sceKernelGetFsSandboxRandomWord");
			}

			if ((ret = sceKernelDlsym(libkernelhandle, "get_page_table_stats", (void**)&get_page_table_stats)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for get_page_table_stats");
			}

			if ((ret = sceKernelDlsym(libkernelhandle, "sceKernelInternalMemoryGetAvailableSize", (void**)&sceKernelInternalMemoryGetAvailableSize)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for sceKernelInternalMemoryGetAvailableSize");
			}

			if ((ret = sceKernelDlsym(libkernelhandle, "sceKernelGetCpuFrequency", (void**)&sceKernelGetCpuFrequency)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for sceKernelGetCpuFrequency");
			}

			if ((ret = sceKernelDlsym(libkernelhandle, "sceKernelGetCpuUsage", (void**)&sceKernelGetCpuUsage)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for sceKernelGetCpuUsage");
			}

			if ((ret = sceKernelDlsym(libkernelhandle, "sceKernelGetThreadName", (void**)&sceKernelGetThreadName)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for sceKernelGetCpuUsage");
			}

			if ((ret = sceKernelDlsym(libkernelhandle, "sceKernelGetModuleInfo", (void**)&sceKernelGetModuleInfo)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for sceKernelGetModuleInfo");
			}

			//
			if ((ret = sceKernelDlsym(libkernelhandle, "pthread_mutexattr_init", (void**)&pthread_mutexattr_init)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for pthread_mutexattr_init");
				OrbisSystemWrapper::pthread_mutexattr_init = (decltype(OrbisSystemWrapper::pthread_mutexattr_init))::pthread_mutexattr_init;
			}

			if ((ret = sceKernelDlsym(libkernelhandle, "pthread_mutex_init", (void**)&pthread_mutex_init)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for pthread_mutex_init");
				OrbisSystemWrapper::pthread_mutex_init = (decltype(OrbisSystemWrapper::pthread_mutex_init))::pthread_mutex_init;
			}

			if ((ret = sceKernelDlsym(libkernelhandle, "pthread_mutexattr_destroy", (void**)&pthread_mutexattr_destroy)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for pthread_mutexattr_destroy");
				OrbisSystemWrapper::pthread_mutexattr_destroy = (decltype(OrbisSystemWrapper::pthread_mutexattr_destroy))::pthread_mutexattr_destroy;
			}

			if ((ret = sceKernelDlsym(libkernelhandle, "pthread_mutex_destroy", (void**)&pthread_mutex_destroy)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for pthread_mutex_destroy");
				OrbisSystemWrapper::pthread_mutex_destroy = (decltype(OrbisSystemWrapper::pthread_mutex_destroy))::pthread_mutex_destroy;
			}

			if ((ret = sceKernelDlsym(libkernelhandle, "pthread_mutex_trylock", (void**)&pthread_mutex_trylock)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for pthread_mutex_trylock");
				OrbisSystemWrapper::pthread_mutex_trylock = (decltype(OrbisSystemWrapper::pthread_mutex_trylock))::pthread_mutex_trylock;
			}

			if ((ret = sceKernelDlsym(libkernelhandle, "pthread_mutex_lock", (void**)&pthread_mutex_lock)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for pthread_mutex_lock");
				OrbisSystemWrapper::pthread_mutex_lock = (decltype(OrbisSystemWrapper::pthread_mutex_lock))::pthread_mutex_lock;
			}

			if ((ret = sceKernelDlsym(libkernelhandle, "pthread_mutex_unlock", (void**)&pthread_mutex_unlock)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for pthread_mutex_unlock");
				OrbisSystemWrapper::pthread_mutex_unlock = (decltype(OrbisSystemWrapper::pthread_mutex_unlock))::pthread_mutex_unlock;
			}
		}
		else 
		{
			MessageHandler::Notify("Failed To Load libkernel.sprx");
		}

		libjbchandle = sceKernelLoadStartModule(libjbcpath, 0, 0, 0, 0, 0);
		if (libjbchandle > 0)
		{
			if ((ret = sceKernelDlsym(libjbchandle, "jbc_get_cred", (void**)&jbc_get_cred)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for sceKernelGetFsSandboxRandomWord");
			}

			if ((ret = sceKernelDlsym(libjbchandle, "jbc_jailbreak_cred", (void**)&jbc_jailbreak_cred)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for sceKernelGetFsSandboxRandomWord");
			}

			if ((ret = sceKernelDlsym(libjbchandle, "jbc_set_cred", (void**)&jbc_set_cred)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for sceKernelGetFsSandboxRandomWord");
			}

			if ((ret = sceKernelDlsym(libjbchandle, "jbc_run_as_root", (void**)&jbc_run_as_root)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for sceKernelGetFsSandboxRandomWord");
			}

			if ((ret = sceKernelDlsym(libjbchandle, "jbc_mount_in_sandbox", (void**)&jbc_mount_in_sandbox)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for sceKernelGetFsSandboxRandomWord");
			}

			if ((ret = sceKernelDlsym(libjbchandle, "jbc_unmount_in_sandbox", (void**)&jbc_unmount_in_sandbox)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for sceKernelGetFsSandboxRandomWord");
			}

		}
		else
		{
			MessageHandler::Notify("Failed To Load libjbc");
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
#elif defined(__SWITCH__)
#endif
		Inited = true;
#if defined (__ORBIS__) || defined(__OPENORBIS__)
		OrbisSFOHandler::sfo_parser::GetSingleton()->ParseCurrentProcessSFO();
#endif

		OrbisOffsetManger::OffsetManger::GetSingleton()->Initialize();
		OrbisINIHandler::OrbisINIHandler::GetSingleton()->ParseINI();
	}

	bool initializeModuleList()
	{
#if defined (__ORBIS__) || defined(__OPENORBIS__)
		size_t ModuleCount;
		int ret = 0;
		int32_t handles[SCE_KERNEL_MAX_MODULES] { 0 };
		SceKernelModuleInfo ModuleInfo{ 0 };
		
#if defined(__ORBIS__)
		if ((ret = sceKernelGetModuleList((int32_t*)handles, SCE_KERNEL_MAX_MODULES, &ModuleCount)))
#elif defined(__OPENORBIS__)
		if ((ret = sceKernelGetModuleList((uint32_t*)handles, SCE_KERNEL_MAX_MODULES, &ModuleCount)))
#endif
		{
			MessageHandler::KernelPrintOut("%s %d | Ret 0x%lx", __FUNCTION__, __LINE__, ret);
			return false;
		}
		
		for (size_t i = 0; i < ModuleCount; i++)
		{
			ModuleInfo.size = 0x160;
			MessageHandler::KernelPrintOut("%s %d | i = %d ", __FUNCTION__, __LINE__, i);

			if ((ret = sceKernelGetModuleInfo(handles[i], &ModuleInfo)))
			{
				MessageHandler::KernelPrintOut("%s %d | i = %d - handles 0x%lx - Ret 0x%lx", __FUNCTION__, __LINE__, i, handles[i], ret);
				break;
			}
			else
			{
				MessageHandler::KernelPrintOut("%s %d | i = %d - handles 0x%lx - Ret 0x%lx", __FUNCTION__, __LINE__, i, handles[i], ret);
				ModuleInfo.handle = handles[i];
				g_moduleList.push_back(ModuleInfo);
			}
		}

		return (g_moduleList.size() > 0);
#else
		return true;
#endif
	}

	int  GetModuleHandle(const char* a_name)
	{
#if defined (__ORBIS__) || defined(__OPENORBIS__)
		bool a_usemira = OrbisMiraHandler::IsMira();
		if (a_usemira)
		{
			if (OrbisMiraHandler::g_processmodulelist.size() <= 0)
				OrbisMiraHandler::InitializeModueList();

			for (int i = 0; i < OrbisMiraHandler::g_processmodulelist.size(); i++)
			{
				auto& module = OrbisMiraHandler::g_processmodulelist[i];
				char* path = strrchr(module.Path, '/');
				if (!path)
					return -1;

				path++;

				if (!strcasecmp(a_name, path))
					return module.Handle;
			}
			return -1;
		}
		else
		{
			if (g_moduleList.size() <= 0)
			{
				if (!initializeModuleList())
				{
					MessageHandler::KernelPrintOut("Failed to initialize Module List! [%s,%d]", __FUNCTION__, __LINE__);
					return -1;
				}
			}

// #if true
// 			int i = 0;
// 			for (auto& modules : g_moduleList)
// 			{
// 				MessageHandler::KernelPrintOut
// 				(
// 					"Module [%d]\n"
// 					"Module Name - %s\n"
// 					"Module Segment Count - %d\n"
// 
// 					"Module Segment[%d] Address - %p\n"
// 					"Module Segment[%d] prot - %d\n"
// 					"Module Segment[%d] size - %d\n"
// 
// 					"Module Segment[%d] Address - %p\n"
// 					"Module Segment[%d] prot - %d\n"
// 					"Module Segment[%d] size - %d\n"
// 
// 					"Module Segment[%d] Address - %p\n"
// 					"Module Segment[%d] prot - %d\n"
// 					"Module Segment[%d] size - %d\n"
// 
// 					"Module Segment[%d] Address - %p\n"
// 					"Module Segment[%d] prot - %d\n"
// 					"Module Segment[%d] size - %d",
// 
// 					i,
// 					modules.name,
// 					modules.numSegments,
// 
// 					0,
// 					modules.segmentInfo[0].baseAddr,
// 					0,
// 					modules.segmentInfo[0].prot,
// 					0,
// 					modules.segmentInfo[0].size,
// 
// 					1,
// 					modules.segmentInfo[1].baseAddr,
// 					1,
// 					modules.segmentInfo[1].prot,
// 					1,
// 					modules.segmentInfo[1].size,
// 
// 					2,
// 					modules.segmentInfo[2].baseAddr,
// 					2,
// 					modules.segmentInfo[2].prot,
// 					2,
// 					modules.segmentInfo[2].size,
// 
// 					3,
// 					modules.segmentInfo[3].baseAddr,
// 					3,
// 					modules.segmentInfo[3].prot,
// 					3,
// 					modules.segmentInfo[3].size);
// 				i++;
// 			}
// #endif
			for (auto& modules : g_moduleList)
			{
				if (!strcasecmp(modules.name, a_name))
				{
					return modules.handle;
				}
			}

			return -1;
		}
#else
	return -1;
#endif
	}

	bool IsLoaded(const char* a_name, bool a_usemira)
	{
#if defined (__ORBIS__) || defined(__OPENORBIS__)
		if (a_usemira)
		{
			if (OrbisMiraHandler::g_processmodulelist.size() <= 0)
				OrbisMiraHandler::InitializeModueList();

			for (int i = 0; i < OrbisMiraHandler::g_processmodulelist.size(); i++)
			{
				auto& module = OrbisMiraHandler::g_processmodulelist[i];
				char* path = strrchr(module.Path, '/');
				if (!path)
					return false;

				path++;

				if (!strcasecmp(a_name, path))
					return true;
			}

			return false;
		}
		else
		{
			if (g_moduleList.size() <= 0)
			{
				if (!initializeModuleList())
				{
					MessageHandler::KernelPrintOut("Failed to initialize Module List! [%s,%d]", __FUNCTION__, __LINE__);
					return false;
				}
			}

//#if true
//			int i = 0;
//			for (auto& modules : g_moduleList)
//			{
//				MessageHandler::KernelPrintOut
//				(
//					"Module [%d]\n" 
//					"Module Name - %s\n" 
//					"Module Segment Count - %d\n"
//
//					"Module Segment[%d] Address - %p\n"
//					"Module Segment[%d] prot - %d\n"
//					"Module Segment[%d] size - %d\n"
//
//					"Module Segment[%d] Address - %p\n"
//					"Module Segment[%d] prot - %d\n"
//					"Module Segment[%d] size - %d\n"
//
//					"Module Segment[%d] Address - %p\n"
//					"Module Segment[%d] prot - %d\n"
//					"Module Segment[%d] size - %d\n"
//
//					"Module Segment[%d] Address - %p\n"
//					"Module Segment[%d] prot - %d\n"
//					"Module Segment[%d] size - %d", 
//					
//					i, 
//					modules.name, 
//					modules.numSegments, 
//
//					0,
//					modules.segmentInfo[0].baseAddr,
//					0,
//					modules.segmentInfo[0].prot,
//					0,
//					modules.segmentInfo[0].size,
//					
//					1,
//					modules.segmentInfo[1].baseAddr,
//					1,
//					modules.segmentInfo[1].prot,
//					1,
//					modules.segmentInfo[1].size,
//
//					2,
//					modules.segmentInfo[2].baseAddr,
//					2,
//					modules.segmentInfo[2].prot,
//					2,
//					modules.segmentInfo[2].size,
//
//					3,
//					modules.segmentInfo[3].baseAddr,
//					3,
//					modules.segmentInfo[3].prot,
//					3,
//					modules.segmentInfo[3].size);
//				i++;
//			}
//#endif
			for (auto& modules : g_moduleList)
			{
				if (!strcasecmp(modules.name, a_name))
				{
					MessageHandler::KernelPrintOut("%s %d", __FUNCTION__, __LINE__);
					g_moduleList.clear();
					return true;
				}
			}

			g_moduleList.clear();
			return false;
		}
#else
		return false;
#endif	
	}

	void HandleCreedSwapFromType(int PathType, bool jailbreak)
	{
#if defined (__ORBIS__) || defined(__OPENORBIS__)
		bool SwapCreed = (PathType == OrbisFileSystem::Data || PathType == OrbisFileSystem::USB0 || PathType == OrbisFileSystem::USB1 || PathType == OrbisFileSystem::USB2 || PathType == OrbisFileSystem::USB3 || PathType == OrbisFileSystem::USB4 || PathType == OrbisFileSystem::USB5 || PathType == OrbisFileSystem::USB6 || PathType == OrbisFileSystem::USB7 || PathType == OrbisFileSystem::System);
		if (SwapCreed && jailbreak)
		{
			OrbisSystemWrapper::Jailbreak();
		}
		else if (SwapCreed && !jailbreak)
		{
			OrbisSystemWrapper::Jail();
		}
#endif
	}
}