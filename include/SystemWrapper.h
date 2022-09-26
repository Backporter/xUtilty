#pragma once

#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include <vector>

#include "SafeTypes.h"
#include "MessageHandler.h"

#if defined(__OPENORBIS__)
#include <orbis/libkernel.h>
#include <orbis/Rtc.h>
typedef TimeTable SceRtcDateTime;
#elif defined(__ORBIS__)
#include <kernel.h>
#include <rtc.h>
#elif defined(__SWITCH__) || defined(PLATFORM_NX)
#include <switch.h>
typedef TimeCalendarTime SceRtcDateTime;
#endif

namespace OrbisSystemWrapper
{
#if defined(__ORBIS__)
	typedef SceLibcMallocManagedSize OrbisLibcMallocManagedSize;

#elif defined(__OPENORBIS__)
	typedef struct OrbisLibcMallocManagedSize 
	{
		unsigned short size;
		unsigned short version;
		unsigned int reserved1;
		size_t maxSystemSize;
		size_t currentSystemSize;
		size_t maxInuseSize;
		size_t currentInuseSize;
	} OrbisLibcMallocManagedSize;
#endif

	extern int sysKernelGetLowerLimitUpdVersionret;
	extern int sysKernelGetLowerLimitUpdVersionarg;
	extern int sysKernelGetUpdVersionarg;
	extern int sysKernelGetUpdVersionret;

#if defined (__ORBIS__) || defined(__OPENORBIS__)

	typedef struct SceKernelModuleSegmentInfo 
	{
		void *baseAddr;
		uint32_t size;
		int32_t prot;
	} SceKernelModuleSegmentInfo;

	typedef struct SceKernelModuleInfo 
	{
		size_t size;
		char name[256];
		SceKernelModuleSegmentInfo segmentInfo[4];
		uint32_t numSegments;
		uint8_t fingerprint[20];

		/* given the system refers to this structure by the size member i should be able to add into this*/
		uint32_t handle;
	} SceKernelModuleInfo;

#ifndef SCE_KERNEL_MAX_MODULES
#define SCE_KERNEL_MAX_MODULES 256
#endif

	struct OrbisKernelFirmwareInfo
	{
		size_t Size;
		char VersionString[0x1C];
		uint32_t Version;
	};

	struct proc
	{
		int32_t	 lo_data;
		uint32_t td_tid;
		timespec user_cpu_usage_time;
		timespec system_cpu_usage_time;
	};

	struct jbc_cred
	{
		uid_t uid;
		uid_t ruid;
		uid_t svuid;
		gid_t rgid;
		gid_t svgid;
		uintptr_t prison;
		uintptr_t cdir;
		uintptr_t rdir;
		uintptr_t jdir;
		uint64_t sceProcType;
		uint64_t sonyCred;
		uint64_t sceProcCap;
	};

	enum
	{
		CWD_KEEP,
		CWD_ROOT,
		CWD_RESET,
	};

	extern std::vector<SceKernelModuleInfo> g_moduleList;

	extern jbc_cred Orginalcred;
	extern jbc_cred RootCred;
	extern OrbisKernelFirmwareInfo Firmware;

	extern int32_t libkernelhandle;
	extern int32_t libkernelsyshandle;
	extern int32_t libSceNpTrophyhandle;
	extern int32_t libjbchandle;
	extern int32_t libchandle;

	extern size_t(*mbstowcs)(wchar_t_t* dst, const char* src, size_t len);
	extern size_t(*wcstombs)(char*, const wchar_t_t *, size_t);
	extern    int(*vsprintf)(char*, const char*, va_list);
	extern  void*(*malloc)(size_t);
	extern  void(*free)(void*);
	typedef int32_t(*SceCoredumpHandler)(void* pCommon);

	extern int(*sceKernelGetModuleInfo)(uint32_t handle, SceKernelModuleInfo* info);
	extern int(*ioctl)(int fd, unsigned long request, ...);
	extern int(*sysconf)(int num);
	extern int(*sceKernelGetSystemSwVersion)(OrbisKernelFirmwareInfo * data);
	extern int(*sysKernelGetUpdVersion)(int* unk);
	extern int(*sysKernelGetLowerLimitUpdVersion)(int* unk);
	extern int64_t(*sceKernelSendNotificationRequest)(int64_t unk1, char* Buffer, size_t size, int64_t unk2);
	extern char*(*sceKernelGetFsSandboxRandomWord)();
	extern int(*sceKernelDebugOutText)(int Channel, const char* text);
	
	extern int(*get_page_table_stats)(int, int, int*, int*);
	extern int(*sceKernelInternalMemoryGetAvailableSize)(size_t*);
	
	extern uint64_t(*sceKernelGetCpuFrequency)();
	extern int(*sceKernelGetCpuUsage)(struct proc*, int*);
	extern int(*sceKernelGetThreadName)(unsigned int, char*);

	extern int(*jbc_get_cred)(jbc_cred *ans);
	extern int(*jbc_jailbreak_cred)(jbc_cred *ans);
	extern int(*jbc_set_cred)(const jbc_cred *ans);
	extern void(*jbc_run_as_root)(void(*fn)(void *arg), void *arg, int cwd_mode);
	extern int(*jbc_mount_in_sandbox)(const char *system_path, const char *mnt_name);
	extern int(*jbc_unmount_in_sandbox)(const char *mnt_name);

	typedef struct	pthread_mutex*		pthread_mutex_t_t;
	typedef struct	pthread_mutex_attr* pthread_mutexattr_t_t;

	extern int (*pthread_mutexattr_init)    (pthread_mutexattr_t_t*);
	extern int (*pthread_mutex_init)        (pthread_mutex_t_t*, const pthread_mutexattr_t_t*);
	extern int (*pthread_mutexattr_destroy) (pthread_mutexattr_t_t*);
	extern int (*pthread_mutex_destroy)     (pthread_mutex_t_t*);
	extern int (*pthread_mutex_trylock)     (pthread_mutex_t_t*);
	extern int (*pthread_mutex_lock)        (pthread_mutex_t_t*);
	extern int (*pthread_mutex_unlock)      (pthread_mutex_t_t*);
#endif

	// Set root cred
	void Jailbreak();

	// Restore Orginal(Jailed) cred
	void Jail();

	// Mounts "/mnt/usbX" to "/usb_X/" within the sandbox
	bool MountUSB(int index);

	// Unmounts "/usb_X/" from within the sandbox
	bool UnmountUSB(int index);

	// TimeCalendarTime - NX
	// SceRtcDateTime - SCEE
	void UpdateRTC(SceRtcDateTime* now);

	bool Dump(const char* TypeName, void* base, int size);

	extern bool Inited;
	void initialize();
	bool initializeModuleList();
	int GetModuleHandle(const char* a_name);

	// checks if a prx is loaded
	// a_name = "example_name.prx"
	// Mira = using mira?
	bool IsLoaded(const char* a_name, bool a_usemira);

	void HandleCreedSwapFromType(int PathType, bool jailbreak);
}