#pragma once

#include "Types.h"
#include "MessageHandler.h"

#include <stdint.h>
#include <stdio.h>
#include <vector>
#include <map>
#include <unordered_map>

#if __clang__
#elif _WIN32 || _WIN64
#include <windows.h>
#include <fileapi.h>
#endif

namespace xUtilty
{
	namespace SystemWrapper
	{
		enum { CWD_KEEP, CWD_ROOT, CWD_RESET };

		// wrapper functions:
		bool initialized();

		int32_t GetlibjbcHandle(int32_t* a_newhandle);
		int32_t GetLibcHandle(int32_t* a_newhandle);
		int32_t GetkernelHandle(int32_t* a_newhandle);
		int32_t GetkernelsysHandle(int32_t* a_newhandle);
		int32_t GetSceNpTrophyHandle(int32_t* a_newhandle);
		int32_t	GetFirmwareVersion(int32_t* a_newfw);

		std::map<int32_t, OrbisKernelModuleInfo>&	GetModuleList();
		OrbisKernelFirmwareInfo*					GetFirmware();
		jbc_cred*									GetBackupOrginalCred();
		jbc_cred*									GetOrginalcred();
		jbc_cred*									GetRootCred();

		// wrapper function that is if-defed for and usess whatever the native system inpl is.
		int32_t					QueryMemoryProtection(void* addr, void** start, void** end, int* prot);

		// wrapper function that is if-defed for and usess whatever the native system inpl is.
		int32_t					Mprotect(const void *addr, size_t len, int prot);

		// wrapper function that is if-defed for and usess whatever the native system inpl is.
		int32_t					CreateSemaphore(void* a_semaphore, const char* a_name, uint32_t a_attr, int32_t a_initCount, int32_t a_maxCount, const void* a_Param);

		// wrapper function that is if-defed for and usess whatever the native system inpl is.
		int32_t					CreateEvent(void* a_event, const char* a_name, uint32_t a_attr, uint64_t a_initPattern, const void* a_Param);

		// wrapper function that is if-defed for and usess whatever the native system inpl is.
		int32_t					DeleteSemaphore(void* a_semaphore);

		// wrapper function that is if-defed for and usess whatever the native system inpl is.
		int32_t					DeleteEvent(void* a_event);

		// wrapper function that is if-defed for and usess whatever the native system inpl is.
		int32_t					AllocateDirectMemory(off_t a_searchStart, off_t a_searchEnd, size_t a_size, size_t a_alignment, int a_memoryType, off_t& a_physicalAddresOut);

		// wrapper function that is if-defed for and usess whatever the native system inpl is.
		int32_t					MapDirectMemory(void** a_address, size_t a_size, int a_prot, int a_flags, off_t a_directMemoryStart, size_t a_maxPageSize);

		// wrapper function that is if-defed for and usess whatever the native system inpl is.
		int32_t					ReleaseDirectMemory(off_t a_physicalAddress, size_t a_size);

		// wrapper function that is if-defed for and usess whatever the native system inpl is.
		size_t					GetDirectMemorySize();

		// wrapper function that is if-defed for and usess whatever the native system inpl is.
		uint64_t				getGpuCoreClockFrequency();

		// wrapper function that is if-defed for and usess whatever the native system inpl is.
		int						strcasecmp(const char* a_lhs, const char* a_rhs);

		// wrapper for strchr that if strchr is null it returns _Str else strchr + 1
		const char*				strchr_s(const char* _Str, char _Ch);

		// wrapper for strrchr that if strrchr is null it returns _Str else strrchr + 1
		const char*				strrchr_s(const char* _Str, char _Ch);

		// orbis takes alll params, windows takes only the path
		uintptr_t				LoadModule(const char* a_path, size_t a_argc, const void* a_argv, uint32_t a_flags, const void* a_opt, int* a_res);

		int32_t					ModuleDlsym(uintptr_t a_handle, const char* a_symbol, void** a_funcptr);

		// sys
		int						open(const char* path, int flags, uint16_t mode);
		int						creat(const char* path, uint16_t mode);
		int						close(int);
		int64_t					write(int fd, const void* data, size_t size);
		int64_t					read(int fd, void* data, size_t size);
		off_t					lseek(int, off_t, int);
		int						fstat(int, struct stat*);
		int						fsync(int);
		int						ftruncate(int, off_t);

		int						ioctl(int fd, unsigned long request, ...);
		int						sysconf(int num);

		// libc
		size_t					mbstowcs(wchar_t_t* dst, const char* src, size_t len);
		size_t					wcstombs(char*, const wchar_t_t *, size_t);
		int						vsprintf(char*, const char*, va_list);
		void*					malloc(size_t);
		void					free(void*);

		// kernel
		int						sceKernelGetModuleInfo(uint32_t handle, OrbisKernelModuleInfo*);
		int						sceKernelGetSystemSwVersion(OrbisKernelFirmwareInfo*);
		int						sysKernelGetUpdVersion(int*);
		int						sysKernelGetLowerLimitUpdVersion(int*);
		int64_t					sceKernelSendNotificationRequest(int64_t, char*, size_t, int64_t);
		char*					sceKernelGetFsSandboxRandomWord();
		int						sceKernelDebugOutText(int, const char*);
		int						sceKernelInternalMemoryGetAvailableSize(size_t*);
		int64_t					sceKernelGetCpuFrequency();
		int						sceKernelGetCpuUsage(struct proc*, int*);
		int						sceKernelGetThreadName(unsigned int, char*);


		// Coredump
		int32_t					sceCoredumpRegisterCoredumpHandler(int32_t(*func)(void*), size_t, void*);
		void					sceCoredumpDebugTextOut(const char *str, int len);
		


		// jbc
		int						jbc_get_cred(jbc_cred *ans);
		int						jbc_jailbreak_cred(jbc_cred *ans);
		int						jbc_set_cred(const jbc_cred *ans);
		void					jbc_run_as_root(void fn(void*), void* arg, int cwd_mode);
		int						jbc_mount_in_sandbox(const char* system_path, const char *mnt_name);
		int						jbc_unmount_in_sandbox(const char* mnt_name);

		// a_dst is the OUTPUT buffer that the compressed data will be placed in, a_dstden is updated with the compressed size of the data, a_src is the source of the data to be compressed, a_srclen is the size of the data to be compressed
		int						compress(void* a_dst, unsigned long* a_dstden, void* a_src, unsigned long a_srclen);

		// a_dst is the OUTPUT buffer that the uncompressed data will be placed in, a_dstden is the size of the uncompressed data(must have been saved before), a_src is the compressed data, a_srclen is the size of the compressed data 
		int						uncompress(void* a_dst, unsigned long* a_dstden, void* a_src, unsigned long a_srclen);

		// 
		bool					initializeModuleList();
		int						GetModuleHandle(const char* a_name);
		bool					IsModuleLoaded(const char* a_name);
		OrbisKernelModuleInfo*	GetModuleInfo(const char* a_name);
		uintptr_t				GetModuleAddress(const char* a_name);
		void					SetModuleArray(uintptr_t* a_pModuleBaseAddressArray, size_t a_arraySize = 256);

		void					ImportSymbols();
		void					initialize();
		int						ImportSymbol(int* a_handle, void** a_dst, const char* a_module, const char* a_symbolName);
		void					UpdateRTC(RtcDateTime* now);
		void					Jailbreak();
		void					Jail();
		bool					MountUSB(int index);
		bool					UnmountUSB(int index);
	}
}