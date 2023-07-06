#pragma once

#if __OPENORBIS__
#include <orbis/libkernel.h>
#include <orbis/Rtc.h>
#include <sys/types.h>
#include <orbis/_types/Np.h>
#elif __ORBIS__
#include <np.h>
#include <kernel.h>
#include <rtc.h>
#include <sys/types.h>
#endif

#include <stdint.h>
#include <time.h>

#ifndef SCE_KERNEL_MAX_MODULES
#define SCE_KERNEL_MAX_MODULES 256
#endif

#if __ORBIS__
using NpParentalControlInfo = SceNpParentalControlInfo;
using NpAuthorizationCode = SceNpAuthorizationCode;
using NpCountryCode = SceNpCountryCode;
using NpOnlineId = SceNpOnlineId;
using wchar_t_t = wchar_t;
using RtcDateTime = SceRtcDateTime;
#elif __OPENORBIS__
using NpParentalControlInfo = OrbisNpParentalControlInfo;
using NpAuthorizationCode = OrbisNpAuthorizationCode;
using NpCountryCode = OrbisNpCountryCode;
using NpOnlineId = OrbisNpOnlineId;
using wchar_t_t = uint16_t;
using RtcDateTime = TimeTable;
#else
typedef wchar_t wchar_t_t;
typedef struct _SYSTEMTIME RtcDateTime;
#endif


#if __clang__
typedef struct _FILETIME
{
public:
	uint32_t dwLowDateTime;
	uint32_t dwHighDateTime;
} FILETIME, *PFILETIME, *LPFILETIME;

typedef struct _WIN32_FIND_DATAA
{
public:
	uint32_t	  dwFileAttributes;
	FILETIME      ftCreationTime;
	FILETIME      ftLastAccessTime;
	FILETIME      ftLastWriteTime;
	uint32_t	  nFileSizeHigh;
	uint32_t	  nFileSizeLow;
	uint32_t	  dwReserved0;
	uint32_t	  dwReserved1;
	char          cFileName[260];
	char          cAlternateFileName[14];
} WIN32_FIND_DATAA, *PWIN32_FIND_DATAA, *LPWIN32_FIND_DATAA;
#endif

typedef struct OrbisRtcDateTime
{
	unsigned short year;
	unsigned short month;
	unsigned short day;
	unsigned short hour;
	unsigned short minute;
	unsigned short second;
	unsigned int   microsecond;
} OrbisRtcDateTime;

typedef struct OrbisKernelFirmwareInfo
{
	size_t	 Size{ 0x28 };
	char	 VersionString[0x1C]{ 0 };
	uint32_t Version{ 0 };
} OrbisKernelFirmwareInfo;

typedef struct proc
{
	int32_t	 lo_data;
	uint32_t td_tid;
	timespec user_cpu_usage_time;
	timespec system_cpu_usage_time;
} proc;

typedef struct jbc_cred
{
	uint32_t uid;
	uint32_t ruid;
	uint32_t svuid;
	uint32_t rgid;
	uint32_t svgid;
	uintptr_t prison;
	uintptr_t cdir;
	uintptr_t rdir;
	uintptr_t jdir;
	uint64_t sceProcType;
	uint64_t sonyCred;
	uint64_t sceProcCap;
} jbc_cred;

#if __ORBIS__ || _WIN32 || _WIN64
typedef struct OrbisLibcMallocManagedSize
{
	unsigned short	size;
	unsigned short	version;
	unsigned int	reserved1;
	size_t			maxSystemSize;
	size_t			currentSystemSize;
	size_t			maxInuseSize;
	size_t			currentInuseSize;
} OrbisLibcMallocManagedSize;

typedef struct OrbisKernelModuleSegmentInfo
{
	void*	 baseAddr;
	uint32_t size;
	int32_t  prot;
} OrbisKernelModuleSegmentInfo;

typedef struct OrbisKernelModuleInfo
{
	size_t size;
	char name[256];
	OrbisKernelModuleSegmentInfo segmentInfo[4];
	uint32_t numSegments;
	uint8_t fingerprint[20];
} OrbisKernelModuleInfo;
#elif __OPENORBIS__
using SceRtcDateTime = TimeTable;
using SceCoredumpHandler = int32_t(void*);
#elif __OPENORBIS__
#endif

typedef struct OrbisKernelModuleInfoHandle
{
	OrbisKernelModuleInfo m_moduleInfo{ 0 };
	uint32_t m_moduleHandle{ 0 };
} OrbisKernelModuleInfoHandle;
