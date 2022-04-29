#pragma once

#include "../O/OrbisMessageHandler.h"

#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>

namespace OrbisSystemWrapper
{
	struct OrbisKernelFirmwareInfo
	{
		size_t Size;				// 0x0
		char VersionString[0x1C];	// 0x8 (e.g. " 6.720.001" padded with leading zeros)
		uint32_t Version;			// 0x23
	};
	// size = 28

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

	extern int sysKernelGetLowerLimitUpdVersionret;
	extern int sysKernelGetLowerLimitUpdVersionarg;

	extern int sysKernelGetUpdVersionarg;
	extern int sysKernelGetUpdVersionret;
	
	extern bool Inited;

	extern jbc_cred Orginalcred;
	extern jbc_cred RootCred;
	
	extern int32_t libkernelhandle;
	extern int32_t libkernelsyshandle;
	extern int32_t libSceNpTrophyhandle;
	extern int32_t libjbchandle;
	extern int32_t libchandle;

	extern OrbisKernelFirmwareInfo Firmware;

	// libkernel
	extern int(*ioctl)(int fd, unsigned long request, ...);
	extern int(*sysconf)(int num);
	extern int(*sceKernelGetSystemSwVersion)(OrbisKernelFirmwareInfo * data);
	extern int(*sysKernelGetUpdVersion)(int* unk);
	extern int(*sysKernelGetLowerLimitUpdVersion)(int* unk);
	extern int64_t(*sceKernelSendNotificationRequest)(int64_t unk1, char* Buffer, size_t size, int64_t unk2);
	extern char*(*sceKernelGetFsSandboxRandomWord)();
	extern int(*sceKernelDebugOutText)(int Channel, const char* text);
	// end of libkernel

	// libjbc
	extern int(*jbc_get_cred)(jbc_cred *ans);
	extern int(*jbc_jailbreak_cred)(jbc_cred *ans);
	extern int(*jbc_set_cred)(const jbc_cred *ans);
	extern void(*jbc_run_as_root)(void(*fn)(void *arg), void *arg, int cwd_mode);
	extern int(*jbc_mount_in_sandbox)(const char *system_path, const char *mnt_name);
	extern int(*jbc_unmount_in_sandbox)(const char *mnt_name);
	// end of libjbc

	// loads libkernel and assigns function pointers and global vars thier require offsets/values
	void initialize();

	// Set root cred
	void Jailbreak();

	// Restore Orginal(Jailed) cred
	void Jail();

	// Mounts "/mnt/usbX" to "/usb_X/" within the sandbox
	bool MountUSB(int index);

	// Unmounts "/usb_X/" from within the sandbox
	bool UnmountUSB(int index);
}