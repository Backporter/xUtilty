#pragma once

#include "SystemWrapper.h"
#include "MemoryHandler.h"
#include "MessageHandler.h"
#include "Mutex.h"

#include "../Third-Party/herumi/xbayk/6.00/xbyak.h"

#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <stddef.h>
#include <iostream>
#include <map>

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
	static_assert(sizeof(TrampolineABS) == 14);

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
	static_assert(sizeof(TrampolineHook5) == 5);

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
	static_assert(sizeof(TrampolineHook6) == 6);

#pragma endregion


	template <typename T>
	class OrbisTrampolineAllocatr
	{
	public:
		OrbisTrampolineAllocatr() { }
	public:
	};

	// based of the SKSE team/ianpatts trampoline class found within SKSE/F4SE, ill improve on this at some point....
	class Trampoline
	{
	public:
		Trampoline();
		~Trampoline();

		bool SystemAllocate(void* a_trampoline, const size_t a_capacity) 
		{
			SystemAllocatedMemory = a_trampoline;
			SystemAllocatedLength = a_capacity;
			AllocSysAllocLen	  = 0;
			return SystemAllocatedMemory != NULL;
		}

		bool SystemAllocate(size_t TrampolineSize = 1024 * 64, size_t alignment = 0);
		void SystemDeallocate();
		void SystemRestore(off_t src, size_t src_len = 1024 * 64);

		void* StartTake();
		void  RestoreAllocated(const void* allocated);
		void* Take(size_t size = 8);

		void* allocate(Xbyak::CodeGenerator&);
		size_t Remain() { return SystemAllocatedLength - AllocSysAllocLen; }

		template <size_t N>
		uintptr_t WriteCall(uintptr_t a_src, uintptr_t dest)
		{
			const auto disp = reinterpret_cast<int32_t*>(a_src + N - 4);
			const auto nextOp = a_src + N;
			const auto func = nextOp + *disp;

			if constexpr (N == 5)
			{
				TrampolineABS* ABS = (TrampolineABS*)Take(sizeof(TrampolineABS));
				if (ABS)
				{
					ABS->Init(dest);

					TrampolineHook5 Hook;

					uintptr_t	trampolineAddr = uintptr_t(ABS);
					uintptr_t	nextInstr = a_src + sizeof(Hook);
					ptrdiff_t	trampolineDispl = trampolineAddr - nextInstr;

					assert((trampolineDispl >= INT32_MIN) && (trampolineDispl <= INT32_MAX));

					Hook.Init(trampolineDispl, 0xE8);

					OrbisMemoryHandler::safe_write(a_src, &Hook, sizeof(Hook));
				}

			}
			else if constexpr (N == 6)
			{
				uintptr_t * trampoline = (uintptr_t *)Take();
				if (trampoline)
				{
					uintptr_t	trampolineAddr = (uintptr_t)trampoline;
					uintptr_t	nextInstr = a_src + 6;
					ptrdiff_t	trampolineDispl = trampolineAddr - nextInstr;

					if ((trampolineDispl >= INT32_MIN) && (trampolineDispl <= INT32_MAX))
					{
						TrampolineHook6 code;

						code.Init(0x15, (int32_t)trampolineDispl);
						OrbisMemoryHandler::safe_write(a_src, &code, sizeof(code));
						*trampoline = dest;
					}
				}
			}
			else
			{
				static_assert(N & 0, "Invalid Template Size");
			}

			return func;
		}

		template <size_t N>
		uintptr_t WriteJMP(uintptr_t a_src, uintptr_t dest)
		{
			const auto disp = reinterpret_cast<int32_t*>(a_src + N - 4);
			const auto nextOp = a_src + N;
			const auto func = nextOp + *disp;

			if constexpr (N == 5)
			{
				TrampolineABS* ABS = (TrampolineABS*)Take(sizeof(TrampolineABS));
				if (ABS)
				{
					ABS->Init(dest);

					TrampolineHook5 Hook;

					uintptr_t	trampolineAddr = uintptr_t(ABS);
					uintptr_t	nextInstr = a_src + sizeof(Hook);
					ptrdiff_t	trampolineDispl = trampolineAddr - nextInstr;

					assert((trampolineDispl >= INT32_MIN) && (trampolineDispl <= INT32_MAX));

					Hook.Init(trampolineDispl, 0xE9);

					OrbisMemoryHandler::safe_write(a_src, &Hook, sizeof(Hook));
				}

			}
			else if constexpr (N == 6)
			{
				uintptr_t * trampoline = (uintptr_t *)Take();
				if (trampoline)
				{
					uintptr_t	trampolineAddr = (uintptr_t)trampoline;
					uintptr_t	nextInstr = a_src + 6;
					ptrdiff_t	trampolineDispl = trampolineAddr - nextInstr;

					if ((trampolineDispl >= INT32_MIN) && (trampolineDispl <= INT32_MAX))
					{
						TrampolineHook6 code;

						code.Init(0x25, (int32_t)trampolineDispl);
						OrbisMemoryHandler::safe_write(a_src, &code, sizeof(code));
						*trampoline = dest;

					}
				}
			}
			else
			{
				static_assert(N & 0, "Invalid Template Size");
			}

			return func;
		}

		template <size_t N, typename T>
		uintptr_t WriteCall(uintptr_t source, T dest)	{ return WriteCall<N>(source, (uintptr_t)dest); }

		template <size_t N, typename T>
		uintptr_t WriteJMP(uintptr_t source, T dest)	{ return WriteJMP<N>(source, (uintptr_t)dest); }
	public:
		static Trampoline&				 get()		    { static Trampoline singleton;					return singleton; }
	public:
		void*	allocatePlugin(size_t a_handle, size_t a_size);
		void	restorePlugin(size_t a_handle, void* a_allocated, size_t a_size);
	private:
		void*		SystemAllocatedMemory;
		size_t		SystemAllocatedLength;
		off_t		SystemAllocatedOffset;
		size_t		AllocSysAllocLen;
		void*		AllocSysAllocAddr;
	protected:
		std::mutex							m_mapLock;
		std::unordered_map<size_t, size_t>	m_PluginMap;
	};

	extern Trampoline g_Trampoline;
}
#endif