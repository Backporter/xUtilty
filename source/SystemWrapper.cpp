#include <stdint.h>
#include <string>
#include <iostream>

#include "../include/SafeTypes.h"
#include "../include/SystemWrapper.h"
#include "../include/MessageHandler.h"
#include "../include/FileSystem.h"

#if defined(__ORBIS__)
#include "../include/Mira.h"
#include <kernel.h>
#include <rtc.h>
#elif defined(__OPENORBIS__)
#include "../include/Mira.h"
#include <orbis/libkernel.h>
#include <orbis/Rtc.h>
#endif


#include <assert.h>

namespace SystemWrapper
{
	static bool s_initialized{ false };
	static std::vector<OrbisKernelModuleInfoHandle>	g_moduleList;

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
	int		 (*_ioctl)(int fd, unsigned long request, ...){0};
	int		 (*_sysconf)(int num) { 0 };

	size_t   (*_mbstowcs)(wchar_t_t* dst, const char* src, size_t len) { 0 };
	size_t   (*_wcstombs)(char* dst, const wchar_t_t * src, size_t len) { 0 };
	int	     (*_vsprintf)(char*, const char* , va_list) { 0 };
	void*    (*_malloc)(size_t) { 0 };
	void     (*_free)(void*) { 0 };
	
	int      (*_sceKernelGetModuleInfo)(uint32_t handle, OrbisKernelModuleInfo* info) { 0 };
	int      (*_sceKernelGetSystemSwVersion)(OrbisKernelFirmwareInfo* data) { 0 };
	int      (*_sysKernelGetUpdVersion)(int* unk) { 0 };
	int      (*_sysKernelGetLowerLimitUpdVersion)(int* unk) { 0 };
	int64_t  (*_sceKernelSendNotificationRequest)(int64_t unk1, char* Buffer, size_t size, int64_t unk2) { 0 };
	char*    (*_sceKernelGetFsSandboxRandomWord)() { 0 };
	int      (*_sceKernelDebugOutText)(int Channel, const char* text) { 0 };
	int      (*_sceKernelInternalMemoryGetAvailableSize)(size_t*) { 0 };
	uint64_t (*_sceKernelGetCpuFrequency)() { 0 };
	int      (*_sceKernelGetCpuUsage)(struct proc*, int*) { 0 };
	int      (*_sceKernelGetThreadName)(unsigned int, char*) { 0 };

	int      (*_jbc_get_cred)(jbc_cred *ans) { 0 };
	int      (*_jbc_jailbreak_cred)(jbc_cred *ans) { 0 };
	int      (*_jbc_set_cred)(const jbc_cred *ans) { 0 };
	void     (*_jbc_run_as_root)(void(*fn)(void *arg), void *arg, int cwd_mode) { 0 };
	int      (*_jbc_mount_in_sandbox)(const char *system_path, const char *mnt_name) { 0 };
	int      (*_jbc_unmount_in_sandbox)(const char *mnt_name) { 0 };

	int		(*_compress)(void*, unsigned long*, void*, unsigned long) { 0 };
	int		(*_uncompress)(void*, unsigned long*, void*, unsigned long) { 0 };
#pragma endregion

#pragma region

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
			MessageHandler::Notify(":( M");
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
			MessageHandler::Notify(":( F");
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


	int		compress(void* a_dst, unsigned long* a_dstlen, void* a_src, unsigned long a_srclen)
	{
		if (_compress)
		{
			return _compress(a_dst, a_dstlen, a_src, a_srclen);
		}
		else
		{
			return -1;
		}
	}

	int		uncompress(void* a_dst, unsigned long* a_dstlen, void* a_src, unsigned long a_srclen)
	{
		if (_uncompress)
		{
			return _uncompress(a_dst, a_dstlen, a_src, a_srclen);
		}
		else
		{
			return -1;
		}
	}
#pragma endregion

	bool initialized() { return s_initialized; }
	std::vector<OrbisKernelModuleInfoHandle>& GetModuleList() { return g_moduleList; }

	int32_t GetlibjbcHandle(int32_t* a_newhandle)	   { if (a_newhandle) { libjbchandle						= *a_newhandle; } return libjbchandle;						  }
	int32_t GetLibcHandle(int32_t* a_newhandle)        { if (a_newhandle) { libchandle							= *a_newhandle; } return libchandle;						  }
	int32_t GetkernelHandle(int32_t* a_newhandle)      { if (a_newhandle) { libkernelhandle						= *a_newhandle; } return libkernelhandle;					  }
	int32_t GetkernelsysHandle(int32_t* a_newhandle)   { if (a_newhandle) { libkernelsyshandle					= *a_newhandle; } return libkernelsyshandle;				  }
	int32_t GetSceNpTrophyHandle(int32_t* a_newhandle) { if (a_newhandle) { libSceNpTrophyhandle				= *a_newhandle; } return libSceNpTrophyhandle;				  }
	int		GetFirmwareVersion(int32_t* a_newfw)	   { if (a_newfw)     { sysKernelGetLowerLimitUpdVersionarg = *a_newfw;		} return sysKernelGetLowerLimitUpdVersionarg; }
	
	OrbisKernelFirmwareInfo* GetFirmware()				{ return &Firmware; }
	jbc_cred*				 GetBackupOrginalCred()     { return &BackupOrginalcred; }
	jbc_cred*				 GetOrginalcred()		    { return &Orginalcred; }
	jbc_cred*				 GetRootCred()			    { return &RootCred; }
	
	void	ImportSymbols()
	{
		int ret = 0;

#if defined (__ORBIS__) || defined(__OPENORBIS__)
		const char* libjbcpath = "/app0/modules/libjbc.prx";
		const char* zlibpath = "/app0/modules/zlib.prx";

		if (!OrbisFileSystem::PathExists(OrbisFileSystem::Full, libjbcpath, false))
		{
			libjbcpath = "/data/modules/libjbc.prx";
		}

		if (!OrbisFileSystem::PathExists(OrbisFileSystem::Full, zlibpath, false))
		{
			zlibpath = "/data/modules/zlib.prx";
		}


		libkernelhandle = sceKernelLoadStartModule("libkernel.sprx", 0, 0, 0, 0, 0);
		if (libkernelhandle > 0)
		{
			if ((ret = sceKernelDlsym(libkernelhandle, "sceKernelSendNotificationRequest", (void**)&_sceKernelSendNotificationRequest)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for sceKernelSendNotificationRequest");
			}

			if ((ret = sceKernelDlsym(libkernelhandle, "sceKernelDebugOutText", (void**)&_sceKernelDebugOutText)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for sceKernelDebugOutText");
			}

			if ((ret = sceKernelDlsym(libkernelhandle, "sceKernelGetSystemSwVersion", (void**)&_sceKernelGetSystemSwVersion)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for sceKernelGetSystemSwVersion");
			}

			if ((ret = sceKernelDlsym(libkernelhandle, "sysKernelGetUpdVersion", (void**)&_sysKernelGetUpdVersion)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for sysKernelGetUpdVersion");
			}

			if ((ret = sceKernelDlsym(libkernelhandle, "sysKernelGetLowerLimitUpdVersion", (void**)&_sysKernelGetLowerLimitUpdVersion)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for sysKernelGetLowerLimitUpdVersion");
			}

			if ((ret = sceKernelDlsym(libkernelhandle, "sysconf", (void**)&_sysconf)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for sysconf");
			}

			if ((ret = sceKernelDlsym(libkernelhandle, "ioctl", (void**)&_ioctl)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for ioctl");
			}

			if ((ret = sceKernelDlsym(libkernelhandle, "sceKernelGetFsSandboxRandomWord", (void**)&_sceKernelGetFsSandboxRandomWord)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for sceKernelGetFsSandboxRandomWord");
			}

			if ((ret = sceKernelDlsym(libkernelhandle, "sceKernelInternalMemoryGetAvailableSize", (void**)&_sceKernelInternalMemoryGetAvailableSize)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for sceKernelInternalMemoryGetAvailableSize");
			}

			if ((ret = sceKernelDlsym(libkernelhandle, "sceKernelGetCpuFrequency", (void**)&_sceKernelGetCpuFrequency)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for sceKernelGetCpuFrequency");
			}

			if ((ret = sceKernelDlsym(libkernelhandle, "sceKernelGetCpuUsage", (void**)&_sceKernelGetCpuUsage)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for sceKernelGetCpuUsage");
			}

			if ((ret = sceKernelDlsym(libkernelhandle, "sceKernelGetThreadName", (void**)&_sceKernelGetThreadName)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for sceKernelGetCpuUsage");
			}

			if ((ret = sceKernelDlsym(libkernelhandle, "sceKernelGetModuleInfo", (void**)&_sceKernelGetModuleInfo)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for sceKernelGetModuleInfo");
			}
		}
		else
		{
			MessageHandler::Notify("Failed To Load libkernel (libkernel.sprx %d", libkernelhandle);
		}

		libchandle = sceKernelLoadStartModule("libc.sprx", 0, 0, 0, 0, 0);
		if (libchandle > 0)
		{
			if ((ret = sceKernelDlsym(libchandle, "wcstombs", (void**)&_wcstombs)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for wcstombs");
			}

			if ((ret = sceKernelDlsym(libchandle, "mbstowcs", (void**)&_mbstowcs)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for mbstowcs");
			}

			if ((ret = sceKernelDlsym(libchandle, "vsprintf", (void**)&_vsprintf)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for vsprintf");
			}

			if ((ret = sceKernelDlsym(libchandle, "malloc", (void**)&_malloc)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for malloc");
			}

			if ((ret = sceKernelDlsym(libchandle, "free", (void**)&_free)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for free");
			}

		}
		else
		{
			MessageHandler::Notify("Failed To Load libc (libc.sprx / %d)", libchandle);
		}

		libjbchandle = sceKernelLoadStartModule(libjbcpath, 0, 0, 0, 0, 0);
		if (libjbchandle > 0)
		{
			if ((ret = sceKernelDlsym(libjbchandle, "jbc_get_cred", (void**)&_jbc_get_cred)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for jbc_get_cred");
			}

			if ((ret = sceKernelDlsym(libjbchandle, "jbc_jailbreak_cred", (void**)&_jbc_jailbreak_cred)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for jbc_jailbreak_cred");
			}

			if ((ret = sceKernelDlsym(libjbchandle, "jbc_set_cred", (void**)&_jbc_set_cred)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for jbc_set_cred");
			}

			if ((ret = sceKernelDlsym(libjbchandle, "jbc_run_as_root", (void**)&_jbc_run_as_root)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for jbc_run_as_root");
			}

			if ((ret = sceKernelDlsym(libjbchandle, "jbc_mount_in_sandbox", (void**)&_jbc_mount_in_sandbox)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for jbc_mount_in_sandbox");
			}

			if ((ret = sceKernelDlsym(libjbchandle, "jbc_unmount_in_sandbox", (void**)&_jbc_unmount_in_sandbox)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for jbc_unmount_in_sandbox");
			}

		}
		else
		{
			MessageHandler::Notify("Failed To Load libjbc(%s / %d)", zlibpath, zlibhandle);
		}

		zlibhandle = sceKernelLoadStartModule(zlibpath, 0, 0, 0, 0, 0);
		if (zlibhandle > 0)
		{
			if ((ret = sceKernelDlsym(zlibhandle, "compress", (void**)&_compress)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for compress");
			}

			if ((ret = sceKernelDlsym(zlibhandle, "uncompress", (void**)&_uncompress)) != 0)
			{
				MessageHandler::Notify("Failed to get function address for uncompress");
			}
		}
		else
		{
			MessageHandler::Notify("Failed To Load zlib(%s / %d)", zlibpath, zlibhandle);
		}
#endif
	}

	void	initialize()
	{
		if (s_initialized)
			return;

#if defined (__ORBIS__) || defined(__OPENORBIS__)
		ImportSymbols();
		sceKernelGetSystemSwVersion(&Firmware);

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

		// backup, nonvolatile.
		jbc_get_cred(&BackupOrginalcred);

#elif defined(__SWITCH__)
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
				MessageHandler::Notify("%s failed to load module %s", __FUNCTION__, a_module);
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

	void	UpdateRTC(SceRtcDateTime* now)
	{
		if (sceRtcGetCurrentClockLocalTime(now) != 0)
			memset(now, 0, sizeof(SceRtcDateTime));
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
		if (OrbisFileSystem::PathExists(OrbisFileSystem::Full, sandbox, true))
			return false;

		// if the function pointer is valid, call it.
		jbc_mount_in_sandbox(fullpath, sandboxname);

		// confirm it mounted
		return OrbisFileSystem::PathExists(OrbisFileSystem::Full, sandbox, true);
#elif defined(__SWITCH__) || defined(PLATFORM_NX)
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
		if (!OrbisFileSystem::PathExists(OrbisFileSystem::Full, sandbox, true))
			return false;

		// if the function pointer is valid, call it.
		jbc_unmount_in_sandbox(sandboxname);

		return OrbisFileSystem::PathExists(OrbisFileSystem::Full, sandbox, true);
#elif defined(__SWITCH__) || defined(PLATFORM_NX)
		return true;
#endif
	}
}