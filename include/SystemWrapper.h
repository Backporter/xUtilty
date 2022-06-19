#pragma once

#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>

#include "MessageHandler.h"

#if defined(__OPENORBIS__)
#include <orbis/Rtc.h>
typedef TimeTable SceRtcDateTime;
#elif defined(__ORBIS__)
#include <rtc.h>
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

	extern jbc_cred Orginalcred;
	extern jbc_cred RootCred;
	extern OrbisKernelFirmwareInfo Firmware;

	extern int32_t libkernelhandle;
	extern int32_t libkernelsyshandle;
	extern int32_t libSceNpTrophyhandle;
	extern int32_t libjbchandle;
	extern int32_t libchandle;

	extern int(*ioctl)(int fd, unsigned long request, ...);
	extern int(*sysconf)(int num);
	extern int(*sceKernelGetSystemSwVersion)(OrbisKernelFirmwareInfo * data);
	extern int(*sysKernelGetUpdVersion)(int* unk);
	extern int(*sysKernelGetLowerLimitUpdVersion)(int* unk);
	extern int64_t(*sceKernelSendNotificationRequest)(int64_t unk1, char* Buffer, size_t size, int64_t unk2);
	extern char*(*sceKernelGetFsSandboxRandomWord)();
	extern int(*sceKernelDebugOutText)(int Channel, const char* text);
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

	// Set root cred
	void Jailbreak();

	// Restore Orginal(Jailed) cred
	void Jail();

	// Mounts "/mnt/usbX" to "/usb_X/" within the sandbox
	bool MountUSB(int index);

	// Unmounts "/usb_X/" from within the sandbox
	bool UnmountUSB(int index);

	void UpdateRTC(SceRtcDateTime* now);

	bool Dump(const char* TypeName, void* base, int size);

#endif

	extern bool Inited;
	void initialize();
}