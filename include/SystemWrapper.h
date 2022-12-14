#pragma once

#include "Types.h"
#include "MessageHandler.h"

#include <stdint.h>
#include <stdio.h>
#include <vector>

namespace SystemWrapper
{
	// wrapper functions:
	bool initialized();

	std::vector<OrbisKernelModuleInfoHandle>& GetModuleList();

	int32_t GetlibjbcHandle(int32_t* a_newhandle);
	int32_t GetLibcHandle(int32_t* a_newhandle);
	int32_t GetkernelHandle(int32_t* a_newhandle);
	int32_t GetkernelsysHandle(int32_t* a_newhandle);
	int32_t GetSceNpTrophyHandle(int32_t* a_newhandle);
	int32_t	GetFirmwareVersion(int32_t* a_newfw);

	OrbisKernelFirmwareInfo* GetFirmware();
	jbc_cred*				 GetBackupOrginalCred();
	jbc_cred*				 GetOrginalcred();
	jbc_cred*				 GetRootCred();

	// sys
	int		ioctl(int fd, unsigned long request, ...);
	int		sysconf(int num);

	// libc
	size_t	mbstowcs(wchar_t_t* dst, const char* src, size_t len);
	size_t	wcstombs(char*, const wchar_t_t *, size_t);
	int		vsprintf(char*, const char*, va_list);
	void*	malloc(size_t);
	void	free(void*);

	// kernel
	int		sceKernelGetModuleInfo(uint32_t handle, OrbisKernelModuleInfo* );
	int		sceKernelGetSystemSwVersion(OrbisKernelFirmwareInfo * );
	int		sysKernelGetUpdVersion(int* );
	int		sysKernelGetLowerLimitUpdVersion(int* );
	int64_t sceKernelSendNotificationRequest(int64_t , char* , size_t , int64_t );
	char*	sceKernelGetFsSandboxRandomWord();
	int		sceKernelDebugOutText(int , const char*);
	int		sceKernelInternalMemoryGetAvailableSize(size_t*);
	int64_t sceKernelGetCpuFrequency();
	int		sceKernelGetCpuUsage(struct proc*, int*);
	int		sceKernelGetThreadName(unsigned int, char*);

	// jbc
	int		jbc_get_cred(jbc_cred *ans);
	int		jbc_jailbreak_cred(jbc_cred *ans);
	int		jbc_set_cred(const jbc_cred *ans);
	void	jbc_run_as_root(void fn(void*), void* arg, int cwd_mode);
	int		jbc_mount_in_sandbox(const char* system_path, const char *mnt_name);
	int		jbc_unmount_in_sandbox(const char* mnt_name);

	// zlib

	// a_dst is the OUTPUT buffer that the compressed data will be placed in, a_dstlen is updated with the compressed size of the data, a_src is the source of the data to be compressed, a_srclen is the size of the data to be compressed
	int		compress(void* a_dst, unsigned long* a_dstlen, void* a_src, unsigned long a_srclen);
	
	// a_dst is the OUTPUT buffer that the uncompressed data will be placed in, a_dstlen is the size of the uncompressed data(must have been saved before), a_src is the compressed data, a_srclen is the size of the compressed data 
	int		uncompress(void* a_dst, unsigned long* a_dstlen, void* a_src, unsigned long a_srclen);

	// 
	void	ImportSymbols();
	void	initialize();
	int		ImportSymbol(int* a_handle, void** a_dst, const char* a_module, const char* a_symbolName);
	void	UpdateRTC(SceRtcDateTime* now);
	void	Jailbreak();
	void	Jail();
	bool	MountUSB(int index);
	bool	UnmountUSB(int index);
}