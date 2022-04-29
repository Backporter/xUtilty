#pragma once

#include <vector>
#include <stdint.h>

// mira defs
#ifndef MIRA_IOCTL_BASE
#define MIRA_IOCTL_BASE			'M'
#endif

#ifndef	IOCPARM_SHIFT
#define IOCPARM_SHIFT         13
#endif

#ifndef IOC_VOID
#define IOC_VOID		      0x20000000
#endif

#ifndef IOC_OUT
#define IOC_OUT               0x40000000
#endif

#ifndef IOC_IN
#define IOC_IN                0x80000000
#endif

#ifndef MIRA_PROCESS_MODULE_LIST
#define MIRA_PROCESS_MODULE_LIST 0xC0204D17
#endif

#ifndef IOCPARM_MASK
#define IOCPARM_MASK    ((1 << IOCPARM_SHIFT) - 1)
#endif

#ifndef IOC_INOUT
#define IOC_INOUT	   (IOC_IN|IOC_OUT)
#endif

#ifndef IOCPARM_MAX
#define IOCPARM_MAX     (1 << IOCPARM_SHIFT)
#endif

#ifndef _IOC
#define _IOC(inout,group,num,len)    ((unsigned long) ((inout) | (((len) & IOCPARM_MASK) << 16) | ((group) << 8) | (num)))
#endif

#ifndef MIRA_PROCESS_PROTECT_MEMORY
#define MIRA_PROCESS_PROTECT_MEMORY _IOC(IOC_IN, MIRA_IOCTL_BASE,   (12),   sizeof(DriverIoctl))
#endif

#ifndef MIRA_PROCESS_READ_MEMORY
#define MIRA_PROCESS_READ_MEMORY    _IOC(IOC_IN, MIRA_IOCTL_BASE,   (10),   sizeof(DriverIoctl))
#endif

#ifndef MIRA_PROCESS_WRITE_MEMORY
#define MIRA_PROCESS_WRITE_MEMORY   _IOC(IOC_IN, MIRA_IOCTL_BASE,   (11),   sizeof(DriverIoctl))
#endif

namespace OrbisMiraHandler
{
	// mira structs
	typedef struct _ProcessModuleList
	{
		typedef struct _Module
		{
			uint64_t Handle;
			void* MapBase;
			uint64_t MapSize;
			uint64_t TextSize;
			void* DataBase;
			uint64_t DataSize;
			void* VirtualAddressBase;
			void* ReallocBase;
			void* Entry;
			void* PltGot;
			int32_t Status;
			int32_t Flags;
			char Path[260];

		} Module;

		Module* Modules;
		uint64_t ModuleCount;
	} ProcessModuleList;
	static_assert(sizeof(_ProcessModuleList) != 0, "Mira's Structure Mis-match");

	typedef struct _DriverIoctl
	{
		// Pointer to the input data
		void* Input;

		// Input data size
		uint64_t InputSize;

		// Pointer to the output data
		void* Output;

		// Output data size
		uint64_t OutputSize;
	} DriverIoctl;
	static_assert(sizeof(_DriverIoctl) != 0, "Mira's Structure Mis-match");

	typedef struct _ProcessProtectMemory
	{
		// <= 0 for calling process
		int32_t ProcessId;

		// Protection to set
		int32_t Protection;

		// Size to protect
		uint64_t Size;

		// Address to protect
		void* Address;
	} ProcessProtectMemory;
	static_assert(sizeof(_ProcessProtectMemory) != 0, "Mira's Structure Mis-match");

	typedef struct _ProcessReadMemory
	{
		// <= 0 for calling process
		int32_t ProcessId;

		// Address to read from process
		void* Address;

		// Size of data to read
		uint32_t DataSize;
	} ProcessReadMemory;
	static_assert(sizeof(_ProcessReadMemory) != 0, "Mira's Structure Mis-match");

	typedef struct _ProcessWriteMemory
	{
		// <= 0 for calling process
		int32_t ProcessId;

		// Address to write to process
		void* Address;

		// Size of the data to write
		uint32_t DataSize;

		// Data to write to Address
		void* Data;
	} ProcessWriteMemory;
	static_assert(sizeof(_ProcessWriteMemory) != 0, "Mira's Structure Mis-match");

	// main function, this will get all modules/exec loaded within our process
	std::vector<ProcessModuleList::Module> ProcessModules(int m_DriverHandle);

	// mira specific tools, un-used and un-tested for full use
	bool ReadMemory(void* p_Address, void* p_OutData, uint32_t p_Size);
	bool WriteMemory(void* p_Address, void* p_InData, uint32_t p_Size);
	bool ProtectMemory(void* p_Address, uint32_t p_Size, int32_t p_Protection);

	// atempts to open the mira device and returns if its dectected or not
	bool IsMira();

	// calls ProcessModules and iltterates through its list to get the speciifc module, this should *only* be called once
	uintptr_t GetBaseAddress(const char* path = "/app0/eboot.bin");

	// global list that is set by GetBaseAddress, this makes it so we only need to call GetBaseAddress once
	extern std::vector<ProcessModuleList::Module> g_processmodulelist;
}