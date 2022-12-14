#pragma once

#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <stddef.h>
#include <iostream>
#include <map>

#include "SystemWrapper.h"
#include "MemoryHandler.h"
#include "MessageHandler.h"
#include "Mutex.h"

#include "../Third-Party/herumi/xbayk/6.00/xbyak.h"

#ifndef TRAMPOLINE_H_
#define TRAMPOLINE_H_

namespace Trampoline
{
#pragma region RAW_OPCODE_X64
	struct TrampolineABS
	{
		uint8_t  opcode;		// = 0xFF
		uint8_t  modrm;			// = 0x25
		uint32_t RIPRG;			// = 00000000
		uint64_t ABSAddress;	// absolute address

		void Init(uintptr_t _ABSAddress)
		{
			opcode = 0xFF;
			modrm = 0x25;
			RIPRG = 0;
			ABSAddress = _ABSAddress;
		}

	} __attribute__((__packed__));
	static_assert(sizeof(TrampolineABS) == 14, "Size of TrampolineABS is incorrect");

	struct TrampolineHook5
	{
		uint8_t opcode;				// 0xE9 => jump | 0xE8 => call
		int32_t RelitiveAddress;	// 

		void Init(int32_t _RelitiveAddress, uint8_t _opcode)
		{
			opcode = _opcode;
			RelitiveAddress = _RelitiveAddress;
		}

	} __attribute__((__packed__));
	static_assert(sizeof(TrampolineHook5) == 5, "Size of TrampolineHook5 is incorrect");

	struct TrampolineHook6
	{
		uint8_t opcode;				// 0 - 0xFF
		uint8_t modrm;				// 1 - 0x25 = jump  | 0x15 => call
		int32_t RelitiveAddress;	// 

		void Init(uint8_t _modrm, int32_t _RelitiveAddress)
		{
			opcode = 0xFF;
			modrm = _modrm;
			RelitiveAddress = _RelitiveAddress;
		}

	} __attribute__((__packed__));
	static_assert(sizeof(TrampolineHook6) == 6, "Size of TrampolineHook6 is incorrect");

#pragma endregion

	// based of the SKSE team/ianpatts trampoline class found within SKSE/F4SE.
	class Trampoline
	{
	public:
		Trampoline();
		~Trampoline();

		bool SystemAllocate(size_t TrampolineSize = 1024 * 64, size_t alignment = 1024 * 64);
		void SystemDeallocate();
		void SystemRestore(off_t src, size_t src_len = 1024 * 64);

		void* StartTake();
		void  RestoreAllocated(const void* allocated);
		void* Take(size_t size = 8);

		void* allocate(Xbyak::CodeGenerator&);
		size_t Remain() { return SystemAllocatedLength - AllocSysAllocLen; }

		template <size_t N>
		void WriteCall(uintptr_t source, uintptr_t dest)
		{
			if constexpr (N == 5)
			{
				TrampolineABS* ABS = (TrampolineABS*)Take(sizeof(TrampolineABS));
				if (ABS)
				{
					ABS->Init(dest);

					TrampolineHook5 Hook;

					uintptr_t	trampolineAddr = uintptr_t(ABS);
					uintptr_t	nextInstr = source + sizeof(Hook);
					ptrdiff_t	trampolineDispl = trampolineAddr - nextInstr;

					assert((trampolineDispl >= INT32_MIN) && (trampolineDispl <= INT32_MAX));

					Hook.Init(trampolineDispl, 0xE8);

					OrbisMemoryHandler::WriteType(source, &Hook, sizeof(Hook));
				}

			}
			else if constexpr (N == 6)
			{
				uintptr_t * trampoline = (uintptr_t *)Take();
				if (trampoline)
				{
					uintptr_t	trampolineAddr = (uintptr_t)trampoline;
					uintptr_t	nextInstr = source + 6;
					ptrdiff_t	trampolineDispl = trampolineAddr - nextInstr;

					if ((trampolineDispl >= INT32_MIN) && (trampolineDispl <= INT32_MAX))
					{
						TrampolineHook6 code;

						code.Init(0x15, (int32_t)trampolineDispl);
						OrbisMemoryHandler::WriteType(source, &code, sizeof(code));
						*trampoline = dest;

					}
				}
			}
			else
			{
				static_assert(N & 0, "Invalid Template Size");
			}
		}

		template <size_t N>
		void WriteJMP(uintptr_t source, uintptr_t dest)
		{
			if constexpr (N == 5)
			{
				TrampolineABS* ABS = (TrampolineABS*)Take(sizeof(TrampolineABS));
				if (ABS)
				{
					ABS->Init(dest);

					TrampolineHook5 Hook;

					uintptr_t	trampolineAddr = uintptr_t(ABS);
					uintptr_t	nextInstr = source + sizeof(Hook);
					ptrdiff_t	trampolineDispl = trampolineAddr - nextInstr;

					assert((trampolineDispl >= INT32_MIN) && (trampolineDispl <= INT32_MAX));

					Hook.Init(trampolineDispl, 0xE9);

					OrbisMemoryHandler::WriteType(source, &Hook, sizeof(Hook));
				}

			}
			else if constexpr (N == 6)
			{
				uintptr_t * trampoline = (uintptr_t *)Take();
				if (trampoline)
				{
					uintptr_t	trampolineAddr = (uintptr_t)trampoline;
					uintptr_t	nextInstr = source + 6;
					ptrdiff_t	trampolineDispl = trampolineAddr - nextInstr;

					if ((trampolineDispl >= INT32_MIN) && (trampolineDispl <= INT32_MAX))
					{
						TrampolineHook6 code;

						code.Init(0x25, (int32_t)trampolineDispl);
						OrbisMemoryHandler::WriteType(source, &code, sizeof(code));
						*trampoline = dest;

					}
				}
			}
			else
			{
				static_assert(N & 0, "Invalid Template Size");
			}
		}

		template <size_t N>
		void WriteCall(uintptr_t source, Xbyak::CodeGenerator& xbayk) { return WriteCall<N>(source, uintptr_t(xbayk.getCode())); }

		template <size_t N>
		void WriteJMP(uintptr_t source, Xbyak::CodeGenerator& xbayk) { return WriteJMP<N>(source, uintptr_t(xbayk.getCode())); }
	public:
		static Mutex::Mutex&			 getallocMutex() { static Mutex::Mutex mutex;					return mutex;	  }
		static std::map<size_t, size_t>& getallocMap()   { static std::map<size_t, size_t> allocMap;	return allocMap;  }
		static Trampoline&				 get()		     { static Trampoline singleton;					return singleton; }
	public:
		void*							 allocatePlugin(size_t a_handle, size_t a_size);
	private:
		void*		SystemAllocatedMemory;
		size_t		SystemAllocatedLength;
		off_t		SystemAllocatedOffset;
		size_t		AllocSysAllocLen;
		void*		AllocSysAllocAddr;
	};

	
	extern Trampoline g_Trampoline;
}
#endif