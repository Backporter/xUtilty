#include "../include/SystemWrapper.h"

//
#include "../include/MessageHandler.h"
#include "../include/FileSystem.h"
#include "../include/Types.h"


#if defined(__ORBIS__)
#include "../include/Mira.h"
#include <kernel.h>
#include <rtc.h>
#include <gnm.h>
#elif __OPENORBIS__
typedef OrbisKernelModule SceKernelModule;
typedef OrbisKernelSema SceKernelSema;
typedef OrbisKernelSemaOptParam SceKernelSemaOptParam;
typedef OrbisKernelEventFlag SceKernelEventFlag;
typedef OrbisKernelEventFlagOptParam SceKernelEventFlagOptParam;
#include "../include/Mira.h"
#include <orbis/libkernel.h>
#include <orbis/Rtc.h>
#endif

#include <assert.h>
#include <map>
#include <stdint.h>
#include <string>
#include <iostream>
#include <fcntl.h>

#if _WIN32 || _WIN64
#include <io.h>
#endif

#ifndef USE_JBC
#if __ORBIS__
#define USE_JBC true
#elif __OPENORBIS__
#define USE_JBC false
#else
#define USE_JBC false
#endif
#endif

namespace xUtilty
{
	namespace SystemWrapper
	{
		static bool s_initialized{ false };
		static std::map<int32_t, OrbisKernelModuleInfo> g_moduleList;

		static int32_t libjbchandle{ -1 };
		static int32_t zlibhandle{ -1 };
		static int32_t libchandle{ -1 };
		static int32_t libkernelhandle{ -1 };
		static int32_t libkernelsyshandle{ -1 };
		static int32_t libSceNpTrophyhandle{ -1 };

		static OrbisKernelFirmwareInfo				Firmware;
		static int32_t								sysKernelGetLowerLimitUpdVersionarg{ 0 };
		static jbc_cred								BackupOrginalcred{ 0 };

		// volatile structs.
		static jbc_cred								Orginalcred{ 0 };
		static jbc_cred								RootCred{ 0 };

#pragma region
		int(*_ioctl)(int fd, unsigned long request, ...) { 0 };
		int(*_sysconf)(int num) { 0 };

		size_t(*_mbstowcs)(wchar_t_t* dst, const char* src, size_t len) { 0 };
		size_t(*_wcstombs)(char* dst, const wchar_t_t * src, size_t len) { 0 };
		int(*_vsprintf)(char*, const char*, va_list) { 0 };
		void*(*_malloc)(size_t) { 0 };
		void(*_free)(void*) { 0 };

		int(*_sceKernelGetModuleInfo)(uint32_t handle, OrbisKernelModuleInfo* info) { 0 };
		int(*_sceKernelGetSystemSwVersion)(OrbisKernelFirmwareInfo* data) { 0 };
		int(*_sysKernelGetUpdVersion)(int* unk) { 0 };
		int(*_sysKernelGetLowerLimitUpdVersion)(int* unk) { 0 };
		int64_t(*_sceKernelSendNotificationRequest)(int64_t unk1, char* Buffer, size_t size, int64_t unk2) { 0 };
		char*(*_sceKernelGetFsSandboxRandomWord)() { 0 };
		int(*_sceKernelDebugOutText)(int Channel, const char* text) { 0 };
		int(*_sceKernelInternalMemoryGetAvailableSize)(size_t*) { 0 };
		uint64_t(*_sceKernelGetCpuFrequency)() { 0 };
		int(*_sceKernelGetCpuUsage)(struct proc*, int*) { 0 };
		int(*_sceKernelGetThreadName)(unsigned int, char*) { 0 };


		int32_t(*_sceCoredumpRegisterCoredumpHandler)(int32_t(*func)(void*), size_t stackSize, void* pCommon);
		void(*_sceCoredumpDebugTextOut)(const char*, int) { 0 };

		int(*_jbc_get_cred)(jbc_cred *ans) { 0 };
		int(*_jbc_jailbreak_cred)(jbc_cred *ans) { 0 };
		int(*_jbc_set_cred)(const jbc_cred *ans) { 0 };
		void(*_jbc_run_as_root)(void(*fn)(void *arg), void *arg, int cwd_mode) { 0 };
		int(*_jbc_mount_in_sandbox)(const char *system_path, const char *mnt_name) { 0 };
		int(*_jbc_unmount_in_sandbox)(const char *mnt_name) { 0 };

		int(*_compress)(void*, unsigned long*, void*, unsigned long) { 0 };
		int(*_uncompress)(void*, unsigned long*, void*, unsigned long) { 0 };
#pragma endregion

#pragma region

		int		open(const char* path, int flags, uint16_t mode)
		{
#if __clang__
			return ::open(path, flags, mode);
#elif _WIN32 || _WIN64
			return ::_open(path, flags, mode);
#endif
		}

		int		creat(const char* path, uint16_t mode)
		{
#if __clang__
			return ::creat(path, mode);
#elif _WIN32 || _WIN64
			return ::_creat(path, mode);
#endif
		}

		int		close(int fd)
		{
#if __clang__
			return ::close(fd);
#elif _WIN32 || _WIN64
			return ::_close(fd);
#endif
		}

		int64_t					write(int fd, const void* data, size_t size)
		{
#if __clang__
			return ::write(fd, data, size);
#elif _WIN32 || _WIN64
			return ::_write(fd, data, size);
#endif
		}

		int64_t					read(int fd, void* data, size_t size)
		{
#if __clang__
			return ::read(fd, data, size);
#elif _WIN32 || _WIN64
			return ::_read(fd, data, size);
#endif
		}

		off_t	 lseek(int fd, off_t pos, int mode)
		{
#if __clang__
			return ::lseek(fd, pos, mode);
#elif _WIN32 || _WIN64
			return ::_lseek(fd, pos, mode);
#endif
		}

		int						fstat(int fd, struct stat* stat)
		{
#if __clang__
			return ::fstat(fd, stat);
#elif _WIN32 || _WIN64
			return ::fstat(fd, stat);
#endif
		}

		int						fsync(int fd)
		{
#if __clang__
			return ::fsync(fd);
#elif _WIN32 || _WIN64
			return ::_commit(fd);
#endif
		}

		int						ftruncate(int fd, off_t pos)
		{
#if __clang__
			return ::ftruncate(fd, pos);
#elif _WIN32 || _WIN64
			return ::_chsize_s(fd, pos);
#endif
		}

		// https://opensource.apple.com/source/xnu/xnu-4903.221.2/libsyscall/wrappers/ioctl.c.auto.html
		int		ioctl(int fd, unsigned long request, ...)
		{
			va_list	ap;
			void* arg;

			va_start(ap, request);
			arg = va_arg(ap, void*);
			va_end(ap);
			if (_ioctl)
				return _ioctl(fd, request, arg);
			else
				return -1;
		}

		int		sysconf(int num)
		{
			if (_sysconf)
				return _sysconf(num);
			else
				return -1;
		}

		size_t	mbstowcs(wchar_t_t* dst, const char* src, size_t len)
		{
			if (_mbstowcs)
			{
				return _mbstowcs(dst, src, len);
			}
			else
			{
				return ::mbstowcs(reinterpret_cast<wchar_t*>(dst), src, len);
			}
		}

		size_t	wcstombs(char* dst, const wchar_t_t* src, size_t max)
		{
			if (_wcstombs)
			{
				return _wcstombs(dst, src, max);
			}
			else
			{
				return ::wcstombs(dst, reinterpret_cast<const wchar_t*>(src), max);
			}
		}

		int		vsprintf(char* s, const char* format, va_list arg)
		{
			if (_vsprintf)
			{
				return _vsprintf(s, format, arg);
			}
			else
			{
				return ::vsprintf(s, format, arg);
			}
		}

		void*	malloc(size_t len)
		{
			if (_malloc)
			{
				return _malloc(len);
			}
			else
			{
				return ::malloc(len);
			}
		}

		void	free(void* ptr)
		{
			if (_free)
			{
				_free(ptr);
			}
			else
			{
				::free(ptr);
			}
		}

		int		sceKernelGetModuleInfo(uint32_t handle, OrbisKernelModuleInfo* info)
		{
			if (_sceKernelGetModuleInfo)
			{
				return _sceKernelGetModuleInfo(handle, info);
			}
			else
			{
				return -1;
			}
		}

		int		sceKernelGetSystemSwVersion(OrbisKernelFirmwareInfo * data)
		{
			if (_sceKernelGetSystemSwVersion)
			{
				return _sceKernelGetSystemSwVersion(data);
			}
			else
			{
				return -1;
			}
		}

		int		sysKernelGetUpdVersion(int* unk)
		{
			if (_sysKernelGetUpdVersion)
			{
				return _sysKernelGetUpdVersion(unk);
			}
			else
			{
				return -1;
			}
		}

		int		sysKernelGetLowerLimitUpdVersion(int* unk)
		{
			if (_sysKernelGetLowerLimitUpdVersion)
			{
				return _sysKernelGetLowerLimitUpdVersion(unk);
			}
			else
			{
				return -1;
			}
		}

		int64_t sceKernelSendNotificationRequest(int64_t unk1, char* Buffer, size_t size, int64_t unk2)
		{
			if (_sceKernelSendNotificationRequest)
			{
				return _sceKernelSendNotificationRequest(unk1, Buffer, size, unk2);
			}
			else
			{
				return -1;
			}
		}

		char*	sceKernelGetFsSandboxRandomWord()
		{
			if (_sceKernelGetFsSandboxRandomWord)
			{
				return _sceKernelGetFsSandboxRandomWord();
			}
			else
			{
				return nullptr;
			}
		}

		int		sceKernelDebugOutText(int Channel, const char* text)
		{
			if (_sceKernelDebugOutText)
			{
				return _sceKernelDebugOutText(Channel, text);
			}
			else
			{
				return -1;
			}
		}

		int		sceKernelInternalMemoryGetAvailableSize(size_t* a_unk)
		{
			if (_sceKernelInternalMemoryGetAvailableSize)
			{
				return _sceKernelInternalMemoryGetAvailableSize(a_unk);
			}
			else
			{
				return -1;
			}
		}

		int64_t sceKernelGetCpuFrequency()
		{
			if (_sceKernelGetCpuFrequency)
			{
				return _sceKernelGetCpuFrequency();
			}
			else
			{
				return -1;
			}
		}

		int		sceKernelGetCpuUsage(struct proc* a_proc, int* a_unk)
		{
			if (_sceKernelGetCpuUsage)
			{
				return _sceKernelGetCpuUsage(a_proc, a_unk);
			}
			else
			{
				return -1;
			}
		}

		int		sceKernelGetThreadName(unsigned int a_unk, char* a_unk2)
		{
			if (_sceKernelGetThreadName)
			{
				return _sceKernelGetThreadName(a_unk, a_unk2);
			}
			else
			{
				return -1;
			}
		}

		int32_t	sceCoredumpRegisterCoredumpHandler(int32_t(*func)(void*), size_t stackSize, void* pCommon)
		{
			if (_sceCoredumpRegisterCoredumpHandler)
			{
				return _sceCoredumpRegisterCoredumpHandler(func, stackSize, pCommon);
			}

			return -1;
		}

		void	sceCoredumpDebugTextOut(const char* str, int len)
		{
			if (_sceCoredumpDebugTextOut)
			{
				return _sceCoredumpDebugTextOut(str, len);
			}
		}

		int		jbc_get_cred(jbc_cred *ans)
		{
			if (_jbc_get_cred)
			{
				return _jbc_get_cred(ans);
			}
			else
			{
				return -1;
			}
		}

		int		jbc_jailbreak_cred(jbc_cred *ans)
		{
			if (_jbc_jailbreak_cred)
			{
				return _jbc_jailbreak_cred(ans);
			}
			else
			{
				return -1;
			}
		}

		int		jbc_set_cred(const jbc_cred *ans)
		{
			if (_jbc_set_cred)
			{
				return _jbc_set_cred(ans);
			}
			else
			{
				return -1;
			}
		}

		void	jbc_run_as_root(void fn(void*), void *arg, int cwd_mode)
		{
			if (_jbc_run_as_root)
			{
				return _jbc_run_as_root(fn, arg, cwd_mode);
			}
			else
			{
				return;
			}
		}

		int		jbc_mount_in_sandbox(const char* system_path, const char* mnt_name)
		{
			if (_jbc_mount_in_sandbox)
			{
				return _jbc_mount_in_sandbox(system_path, mnt_name);
			}
			else
			{
				return -1;
			}
		}

		int		jbc_unmount_in_sandbox(const char* mnt_name)
		{
			if (_jbc_unmount_in_sandbox)
			{
				return _jbc_unmount_in_sandbox(mnt_name);
			}
			else
			{
				return -1;
			}
		}

		int		compress(void* a_dst, unsigned long* a_dstden, void* a_src, unsigned long a_srclen)
		{
			if (_compress)
			{
				return _compress(a_dst, a_dstden, a_src, a_srclen);
			}
			else
			{
				KernelPrintOut("compress not dlsymed.");
				return 0xFFFFFFFF;
			}
		}

		int		uncompress(void* a_dst, unsigned long* a_dstden, void* a_src, unsigned long a_srclen)
		{
			if (_uncompress)
			{
				return _uncompress(a_dst, a_dstden, a_src, a_srclen);
			}
			else
			{
				KernelPrintOut("uncompress not dlsymed.");
				return 0xFFFFFFFF;
			}
		}
#pragma endregion

		bool initialized() { return s_initialized; }

		int32_t GetlibjbcHandle(int32_t* a_newhandle) { if (a_newhandle) { libjbchandle = *a_newhandle; } return libjbchandle; }
		int32_t GetLibcHandle(int32_t* a_newhandle) { if (a_newhandle) { libchandle = *a_newhandle; } return libchandle; }
		int32_t GetkernelHandle(int32_t* a_newhandle) { if (a_newhandle) { libkernelhandle = *a_newhandle; } return libkernelhandle; }
		int32_t GetkernelsysHandle(int32_t* a_newhandle) { if (a_newhandle) { libkernelsyshandle = *a_newhandle; } return libkernelsyshandle; }
		int32_t GetSceNpTrophyHandle(int32_t* a_newhandle) { if (a_newhandle) { libSceNpTrophyhandle = *a_newhandle; } return libSceNpTrophyhandle; }
		int		GetFirmwareVersion(int32_t* a_newfw) { if (a_newfw) { sysKernelGetLowerLimitUpdVersionarg = *a_newfw; } return sysKernelGetLowerLimitUpdVersionarg; }

		std::map<int32_t, OrbisKernelModuleInfo>&	GetModuleList() { return g_moduleList; }
		OrbisKernelFirmwareInfo*					GetFirmware() { return &Firmware; }
		jbc_cred*									GetBackupOrginalCred() { return &BackupOrginalcred; }
		jbc_cred*									GetOrginalcred() { return &Orginalcred; }
		jbc_cred*									GetRootCred() { return &RootCred; }

		int32_t	QueryMemoryProtection(void* addr, void** start, void** end, int* prot)
		{
#if __ORBIS__ || __OPENORBIS__
			return sceKernelQueryMemoryProtection(addr, start, end, prot);
#else 
			return 0xFF;
#endif
		}

		int32_t Mprotect(const void *addr, size_t len, int prot)
		{
#if __ORBIS__ || __OPENORBIS__
			return sceKernelMprotect(addr, len, prot);
#else
			return 0xFF;
#endif
		}

		int32_t CreateSemaphore(void* a_semaphore, const char* a_name, uint32_t a_attr, int32_t a_initCount, int32_t a_maxCount, const void* a_pParam)
		{
#if __ORBIS__ || __OPENORBIS__
			return sceKernelCreateSema(reinterpret_cast<SceKernelSema*>(a_semaphore), a_name, a_attr, a_initCount, a_maxCount, reinterpret_cast<const SceKernelSemaOptParam*>(a_pParam));
#else
			return 0;
#endif
		}

		int32_t CreateEvent(void* a_event, const char* a_name, uint32_t a_attr, uint64_t a_initPattern, const void* a_Param)
		{
#if __ORBIS__ || __OPENORBIS__
			return sceKernelCreateEventFlag(reinterpret_cast<SceKernelEventFlag*>(a_event), a_name, a_attr, a_initPattern, reinterpret_cast<const SceKernelEventFlagOptParam*>(a_Param));
#else
			return 0;
#endif
		}

		int32_t DeleteSemaphore(void* a_semaphore)
		{
#if __ORBIS__ || __OPENORBIS__
			return sceKernelDeleteSema(reinterpret_cast<SceKernelSema>(a_semaphore));
#else
			return 0;
#endif
		}

		int32_t DeleteEvent(void* a_event)
		{
#if __ORBIS__ || __OPENORBIS__
			return sceKernelDeleteEventFlag(reinterpret_cast<SceKernelEventFlag>(a_event));
#else
			return 0;
#endif
		}

		int32_t AllocateDirectMemory(off_t a_searchStart, off_t a_searchEnd, size_t a_size, size_t a_alignment, int a_memoryType, off_t& a_physicalAddresOut)
		{
#if __ORBIS__ || __OPENORBIS__
			return sceKernelAllocateDirectMemory(a_searchStart, a_searchEnd, a_size, a_alignment, a_memoryType, &a_physicalAddresOut);
#else
			return 0;
#endif
		}

		int32_t MapDirectMemory(void** a_address, size_t a_size, int a_prot, int a_flags, off_t a_directMemoryStart, size_t a_maxPageSize)
		{
#if __ORBIS__ || __OPENORBIS__
			return sceKernelMapDirectMemory(a_address, a_size, a_prot, a_flags, a_directMemoryStart, a_maxPageSize);
#else
			return 0;
#endif
		}

		int32_t ReleaseDirectMemory(off_t a_physicalAddress, size_t a_size)
		{
#if __ORBIS__ || __OPENORBIS__
			return sceKernelReleaseDirectMemory(a_physicalAddress, a_size);
#else
			return 0;
#endif
		}

		size_t GetDirectMemorySize()
		{
#if __ORBIS__ || __OPENORBIS__
			return sceKernelGetDirectMemorySize();
#else
			return 0;
#endif
		}

		uint64_t getGpuCoreClockFrequency()
		{
#if __ORBIS__
			return sce::Gnm::getGpuCoreClockFrequency();
#elif __OPENORBIS__
			return 0;
#else
			return 0;
#endif
		}

		int			strcasecmp(const char* a_lhs, const char* a_rhs)
		{
#if __clang__
			return ::strcasecmp(a_lhs, a_rhs);
#else
			return ::_stricmp(a_lhs, a_rhs);
#endif
		}

		const char*	strchr_s(const char* _Str, char _Ch)
		{
			const char* ret = strchr(_Str, _Ch);
			if (ret)
			{
				return ret + 1;
			}

			return _Str;
		}

		const char* strrchr_s(const char *_Str, char _Ch)
		{
			const char* ret = strrchr(_Str, _Ch);
			if (ret)
			{
				return ret + 1;
			}

			return _Str;
		}

		uintptr_t LoadModule(const char* a_path, size_t a_argc, const void* a_argv, uint32_t a_flags, const void* a_opt, int* a_res)
		{
#if __ORBIS__
			return sceKernelLoadStartModule(a_path, a_argc, a_argv, a_flags, (const SceKernelLoadModuleOpt*)a_opt, a_res);
#elif __OPENORBIS__
			return sceKernelLoadStartModule(a_path, a_argc, a_argv, a_flags, (void*)a_opt, (void*)a_res);
#else
			return -1;
#endif
		}

		int32_t ModuleDlsym(uintptr_t a_handle, const char* a_symbol, void** a_funcptr)
		{
#if __ORBIS__ || __OPENORBIS__
			return sceKernelDlsym(a_handle, a_symbol, a_funcptr);
#else
			return -1;
#endif
		}

		void	ImportSymbols()
		{
			int ret = 0;

#if defined (__ORBIS__) || defined(__OPENORBIS__)
			const char* libcpath = "/app0/sce_module/libc.prx";
			const char* libjbcpath = "/app0/modules/libjbc.prx";
			const char* zlibpath = "/app0/modules/zlib.prx";

			// libc inside the game? use it, otherwise use the system's libc.
			if (!FileSystem::PathExists(FileSystem::Full, libcpath, false))
			{
				libcpath = "libc.sprx";
			}

			if (!FileSystem::PathExists(FileSystem::Full, libjbcpath, false))
			{
				libjbcpath = "/data/modules/libjbc.prx";
			}

			if (!FileSystem::PathExists(FileSystem::Full, zlibpath, false))
			{
				zlibpath = "/data/modules/zlib.prx";
			}

			libkernelhandle = sceKernelLoadStartModule("libkernel.sprx", 0, 0, 0, 0, 0);
			if (libkernelhandle > 0)
			{
				if ((ret = sceKernelDlsym(libkernelhandle, "sceKernelSendNotificationRequest", (void**)&_sceKernelSendNotificationRequest)) != 0)
				{
					LocalPrint("Failed to get function address for sceKernelSendNotificationRequest");
				}

				if ((ret = sceKernelDlsym(libkernelhandle, "sceKernelDebugOutText", (void**)&_sceKernelDebugOutText)) != 0)
				{
					LocalPrint("Failed to get function address for sceKernelDebugOutText");
				}

				if ((ret = sceKernelDlsym(libkernelhandle, "sceKernelGetSystemSwVersion", (void**)&_sceKernelGetSystemSwVersion)) != 0)
				{
					KernelPrintOut("Failed to get function address for sceKernelGetSystemSwVersion");
				}

				if ((ret = sceKernelDlsym(libkernelhandle, "sysKernelGetUpdVersion", (void**)&_sysKernelGetUpdVersion)) != 0)
				{
					KernelPrintOut("Failed to get function address for sysKernelGetUpdVersion");
				}

				if ((ret = sceKernelDlsym(libkernelhandle, "sysKernelGetLowerLimitUpdVersion", (void**)&_sysKernelGetLowerLimitUpdVersion)) != 0)
				{
					KernelPrintOut("Failed to get function address for sysKernelGetLowerLimitUpdVersion");
				}

				if ((ret = sceKernelDlsym(libkernelhandle, "sysconf", (void**)&_sysconf)) != 0)
				{
					KernelPrintOut("Failed to get function address for sysconf");
				}

				if ((ret = sceKernelDlsym(libkernelhandle, "ioctl", (void**)&_ioctl)) != 0)
				{
					KernelPrintOut("Failed to get function address for ioctl");
				}

				if ((ret = sceKernelDlsym(libkernelhandle, "sceKernelGetFsSandboxRandomWord", (void**)&_sceKernelGetFsSandboxRandomWord)) != 0)
				{
					KernelPrintOut("Failed to get function address for sceKernelGetFsSandboxRandomWord");
				}

				if ((ret = sceKernelDlsym(libkernelhandle, "sceKernelInternalMemoryGetAvailableSize", (void**)&_sceKernelInternalMemoryGetAvailableSize)) != 0)
				{
					KernelPrintOut("Failed to get function address for sceKernelInternalMemoryGetAvailableSize");
				}

				if ((ret = sceKernelDlsym(libkernelhandle, "sceKernelGetCpuFrequency", (void**)&_sceKernelGetCpuFrequency)) != 0)
				{
					KernelPrintOut("Failed to get function address for sceKernelGetCpuFrequency");
				}

				if ((ret = sceKernelDlsym(libkernelhandle, "sceKernelGetCpuUsage", (void**)&_sceKernelGetCpuUsage)) != 0)
				{
					KernelPrintOut("Failed to get function address for sceKernelGetCpuUsage");
				}

				if ((ret = sceKernelDlsym(libkernelhandle, "sceKernelGetThreadName", (void**)&_sceKernelGetThreadName)) != 0)
				{
					KernelPrintOut("Failed to get function address for sceKernelGetCpuUsage");
				}

				if ((ret = sceKernelDlsym(libkernelhandle, "sceKernelGetModuleInfo", (void**)&_sceKernelGetModuleInfo)) != 0)
				{
					KernelPrintOut("Failed to get function address for sceKernelGetModuleInfo");
				}

				//
				if ((ret = sceKernelDlsym(libkernelhandle, "sceCoredumpRegisterCoredumpHandler", (void**)&_sceCoredumpRegisterCoredumpHandler)) != 0)
				{
					KernelPrintOut("Failed to get function address for sceCoredumpRegisterCoredumpHandler");
				}

				if ((ret = sceKernelDlsym(libkernelhandle, "sceCoredumpDebugTextOut", (void**)&_sceCoredumpDebugTextOut)) != 0)
				{
					KernelPrintOut("Failed to get function address for sceCoredumpDebugTextOut");
				}
			}
			else
			{
				KernelPrintOut("Failed To Load libkernel (libkernel.sprx %d", libkernelhandle);
			}

			libchandle = sceKernelLoadStartModule(libcpath, 0, 0, 0, 0, 0);
			if (libchandle > 0)
			{
				if ((ret = sceKernelDlsym(libchandle, "wcstombs", (void**)&_wcstombs)) != 0)
				{
					KernelPrintOut("Failed to get function address for wcstombs");
				}

				if ((ret = sceKernelDlsym(libchandle, "mbstowcs", (void**)&_mbstowcs)) != 0)
				{
					KernelPrintOut("Failed to get function address for mbstowcs");
				}

				if ((ret = sceKernelDlsym(libchandle, "vsprintf", (void**)&_vsprintf)) != 0)
				{
					KernelPrintOut("Failed to get function address for vsprintf");
				}

				if ((ret = sceKernelDlsym(libchandle, "malloc", (void**)&_malloc)) != 0)
				{
					KernelPrintOut("Failed to get function address for malloc");
				}

				if ((ret = sceKernelDlsym(libchandle, "free", (void**)&_free)) != 0)
				{
					KernelPrintOut("Failed to get function address for free");
				}

			}
			else
			{
				KernelPrintOut("Failed To Load libc (libc.sprx / %d)", libchandle);
			}

			libjbchandle = sceKernelLoadStartModule(libjbcpath, 0, 0, 0, 0, 0);
			if (libjbchandle > 0)
			{
				if ((ret = sceKernelDlsym(libjbchandle, "jbc_get_cred", (void**)&_jbc_get_cred)) != 0)
				{
					KernelPrintOut("Failed to get function address for jbc_get_cred");
				}

				if ((ret = sceKernelDlsym(libjbchandle, "jbc_jailbreak_cred", (void**)&_jbc_jailbreak_cred)) != 0)
				{
					KernelPrintOut("Failed to get function address for jbc_jailbreak_cred");
				}

				if ((ret = sceKernelDlsym(libjbchandle, "jbc_set_cred", (void**)&_jbc_set_cred)) != 0)
				{
					KernelPrintOut("Failed to get function address for jbc_set_cred");
				}

				if ((ret = sceKernelDlsym(libjbchandle, "jbc_run_as_root", (void**)&_jbc_run_as_root)) != 0)
				{
					KernelPrintOut("Failed to get function address for jbc_run_as_root");
				}

				if ((ret = sceKernelDlsym(libjbchandle, "jbc_mount_in_sandbox", (void**)&_jbc_mount_in_sandbox)) != 0)
				{
					KernelPrintOut("Failed to get function address for jbc_mount_in_sandbox");
				}

				if ((ret = sceKernelDlsym(libjbchandle, "jbc_unmount_in_sandbox", (void**)&_jbc_unmount_in_sandbox)) != 0)
				{
					KernelPrintOut("Failed to get function address for jbc_unmount_in_sandbox");
				}

			}
			else
			{
				KernelPrintOut("Failed To Load libjbc(%s / %d)", zlibpath, zlibhandle);
			}

			zlibhandle = sceKernelLoadStartModule(zlibpath, 0, 0, 0, 0, 0);
			if (zlibhandle > 0)
			{
				if ((ret = sceKernelDlsym(zlibhandle, "compress", (void**)&_compress)) != 0)
				{
					KernelPrintOut("Failed to get function address for compress");
				}

				if ((ret = sceKernelDlsym(zlibhandle, "uncompress", (void**)&_uncompress)) != 0)
				{
					KernelPrintOut("Failed to get function address for uncompress");
				}
			}
			else
			{
				KernelPrintOut("[Info] Failed To Load zlib(%s / %d)", zlibpath, zlibhandle);
			}
#endif
		}

		void	initialize()
		{
			if (s_initialized)
				return;

#if __ORBIS__ || __OPENORBIS__

#if USE_JBC
			// backup, nonvolatile.
			jbc_get_cred(&BackupOrginalcred);
#endif

			ImportSymbols();
			sceKernelGetSystemSwVersion(&Firmware);

#if USE_JBC
			if (libjbchandle > 0)
			{
				Jailbreak();
				sysKernelGetLowerLimitUpdVersion(&sysKernelGetLowerLimitUpdVersionarg);
				Jail();
			}
			else
			{
				sysKernelGetLowerLimitUpdVersionarg = Firmware.Version;
			}
#else
			sysKernelGetLowerLimitUpdVersionarg = Firmware.Version;
#endif

#elif __SWITCH__
#endif
			s_initialized = true;
		}

		int		ImportSymbol(int* a_handle, void** a_dst, const char* a_module, const char* a_symbolName)
		{
#if defined (__ORBIS__) || defined(__OPENORBIS__)
			if (!a_handle || !a_dst || !a_module || !a_symbolName)
				return -1;

			auto s_handle = *a_handle;
			if (s_handle < 0)
			{
				s_handle = sceKernelLoadStartModule(a_module, 0, 0, 0, 0, 0);
				if (s_handle < 0)
				{
					Notify("%s failed to load module %s", __FUNCTION__, a_module);
					return -1;
				}
				else
				{
					*a_handle = s_handle;
				}
			}

			//
			if (s_handle > 0)
			{
				return sceKernelDlsym(s_handle, a_symbolName, a_dst);
			}
			else
			{
				return -2;
			}
#endif
			return 0;
		}

		void	UpdateRTC(RtcDateTime* now)
		{
#if __ORBIS__ || __OPENORBIS__
			auto ret = sceRtcGetCurrentClockLocalTime(now);
#elif _WIN32 || _WIN64
			auto ret = 0;
			GetSystemTime(now);
#endif
			if (ret != 0)
			{
				memset(now, 0, sizeof(RtcDateTime));
			}
		}

		void	Jailbreak()
		{
#if defined (__ORBIS__) || defined(__OPENORBIS__)
			memset(&Orginalcred, 0, sizeof(Orginalcred));
			memset(&RootCred, 0, sizeof(RootCred));

			jbc_get_cred(&Orginalcred);
			jbc_jailbreak_cred(&RootCred);
			jbc_set_cred(&RootCred);
#endif
		}

		void	Jail()
		{
#if defined (__ORBIS__) || defined(__OPENORBIS__)
			jbc_set_cred(&Orginalcred);
#endif
		}

		bool	MountUSB(int index)
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
			if (FileSystem::PathExists(FileSystem::Full, sandbox, true))
				return false;

			// if the function pointer is valid, call it.
			jbc_mount_in_sandbox(fullpath, sandboxname);

			// confirm it mounted
			return FileSystem::PathExists(FileSystem::Full, sandbox, true);
#else
			return true;
#endif
		}

		bool	UnmountUSB(int index)
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
			if (!FileSystem::PathExists(FileSystem::Full, sandbox, true))
				return false;

			// if the function pointer is valid, call it.
			jbc_unmount_in_sandbox(sandboxname);

			return FileSystem::PathExists(FileSystem::Full, sandbox, true);
#else
			return true;
#endif
		}

		//
		bool	initializeModuleList()
		{
			if (g_moduleList.size() > 0)
				return true;

#if __ORBIS__ || __OPENORBIS__

			size_t s_moduleCount = 0;
			int ret = 0;
			int32_t handles[SCE_KERNEL_MAX_MODULES] { 0 };

			if ((ret = sceKernelGetModuleList((SceKernelModule*)handles, SCE_KERNEL_MAX_MODULES, &s_moduleCount)))
			{
				PRINT_FMT("sceKernelGetModuleList failed with 0x%lx", ret);
				return false;
			}

			for (size_t i = 0; i < s_moduleCount; i++)
			{
				OrbisKernelModuleInfo s_moduleInfo;
				s_moduleInfo.size = 0x160;

				if ((ret = SystemWrapper::sceKernelGetModuleInfo(handles[i], &s_moduleInfo)))
				{
					PRINT_FMT("sceKernelGetModuleInfo failed with 0x%lx", ret);
					break;
				}
				else
				{
					g_moduleList.insert({ handles[i], s_moduleInfo });
				}
			}

			return (g_moduleList.size() > 0);
#else
			return false;
#endif
		}

		int		GetModuleHandle(const char* a_name)
		{
			// no modules in the list?
			if (!g_moduleList.size())
			{
				// init list.
				if (!initializeModuleList())
				{
					// failed?, force close the game.
					assert(false);
				}
			}

			for (auto& pair : g_moduleList)
			{
				const char* moduleListName = strrchr_s(pair.second.name, '/');
				const char* paramModuleName = strrchr_s(a_name, '/');

				if (!strcasecmp(moduleListName, paramModuleName))
				{
					return pair.first;
				}
			}

			return -1;
		}

		bool	IsModuleLoaded(const char* a_name)
		{
			return GetModuleHandle(a_name) != -1;
		}

		OrbisKernelModuleInfo*	GetModuleInfo(const char* a_name)
		{
			// no modules in the list?
			if (g_moduleList.size() == 0)
			{
				// init list.
				if (!initializeModuleList())
				{
					// failed?, force close the game.
					assert(false);
				}
			}

			for (auto& pair : g_moduleList)
			{
				const char* moduleListName = strrchr_s(pair.second.name, '/');
				const char* paramModuleName = strrchr_s(a_name, '/');

				if (!strcasecmp(moduleListName, paramModuleName))
				{
					return &pair.second;
				}
			}

			return nullptr;
		}

		uintptr_t GetModuleAddress(const char* a_name)
		{
			// no modules in the list?
			if (!g_moduleList.size())
			{
				// init list.
				if (!initializeModuleList())
				{
					// failed?, force close the game.
					assert(false);
				}
			}

			for (auto& pair : g_moduleList)
			{
				const char* moduleListName = strrchr_s(pair.second.name, '/');
				const char* paramModuleName = strrchr_s(a_name, '/');

				if (!strcasecmp(moduleListName, paramModuleName))
				{
#if __ORBIS__
					return (uintptr_t)pair.second.segmentInfo[0].baseAddr;
#elif __OPENORBIS__
					return (uintptr_t)pair.second.segmentInfo[0].address;
#endif
				}
			}

			return 0;
		}

		void SetModuleArray(uintptr_t* a_pModuleBaseAddressArray, size_t a_arraySize)
		{
			if (!a_pModuleBaseAddressArray)
				return;

			// no modules in the list?
			if (!g_moduleList.size())
			{
				// init list.
				if (!initializeModuleList())
				{
					// failed?, force close the game.
					assert(false);
				}
			}

			size_t i = 0;
			for (auto& pair : g_moduleList)
			{
				if (i > a_arraySize)
					break;

#if __ORBIS__
				a_pModuleBaseAddressArray[i] = (uintptr_t)pair.second.segmentInfo[0].baseAddr;
#elif __OPENORBIS__
				a_pModuleBaseAddressArray[i] = (uintptr_t)pair.second.segmentInfo[0].address;
#else
#endif
				i++;
			}
		}
	}
}