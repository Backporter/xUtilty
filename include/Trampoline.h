#pragma once

#include "../include/stl.h"
#include "../include/MemoryHandler.h"

#if defined(__ORBIS__) || defined(__OPENORBIS__) || defined(__x86_64__) || defined(_M_X64)
#include "../Third-Party/herumi/xbayk/6.00/xbyak.h"
#define INCLUDE_TRAPOLINE true
#elif defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)
#define INCLUDE_TRAPOLINE false
#endif

#include <stdint.h>
#include <mutex>
#include <map>
#include <utility>

#if INCLUDE_TRAPOLINE
namespace xUtilty
{
	// based of the SKSE team/ianpatts trampoline class found within SKSE/F4SE with a few addons and code changes.
	class Trampoline
	{
	public:
		using deallocator_t = void(void*, size_t, int64_t);

		enum
		{ 
			// Main executable
			kBranch,

			// Main PRX
			kLocal,

			// Sub modules(unused)
			kModule,
			
			// 
			kTotal = 3
		};
	public:
		Trampoline() = default;
		~Trampoline();
		
		static Trampoline& get(int a_instance)
		{
			static Trampoline singleton[kTotal];
			return singleton[a_instance];
		}
		
		// allocate memory from the system for the pool.
		std::pair<void*, int64_t> allocate_sys(size_t a_size, size_t a_alignment, void* a_pModuleBase);

		// Create a memory pool for the trampoline to use, if a_pModuleBase is NOT a nullptr it's used as a refrence for where it should look for free memory, should it be null it defaults to using the game's base.
		bool create(size_t a_size = 1024 * 64, size_t a_alignment = 0, void* a_pModuleBase = nullptr);

		// 
		void Setup(void* a_data, int64_t a_physicalAddress, size_t a_size) { Setup(a_data, a_physicalAddress, a_size, {}); }

		void Setup(void* a_data, int64_t a_physicalAddress, size_t a_size, std::function<void(void*, size_t, int64_t)> a_deallocater)
		{
			auto trapoline = static_cast<uint8_t*>(a_data);
			if (trapoline)
			{
				constexpr auto INT3 = static_cast<int>(0xCC);
				memset(trapoline, INT3, a_size);
			}

			release();

			m_deallocator = std::move(a_deallocater);
			m_data = trapoline;
			m_physicalAddress = a_physicalAddress;
			m_capacity = a_size;
			m_size = 0;

			log_usage();
		}
		
		// allocate some memory from the memory pool.
		void* allocate(size_t a_size);
		
#if __clang__ || _WIN64
		// 
		void* allocate(Xbyak::CodeGenerator& a_code);
#endif

		// wrapper for allocate which uses a template. 
		template <typename T>
		T* allocate() { return static_cast<T*>(allocate(sizeof(T))); }

		template <size_t N>
		uintptr_t write_branch(uintptr_t a_src, uintptr_t a_dst)
		{
			uint8_t data = 0;
			if constexpr (N == 5) 
			{
				data = 0xE9;
			}
			else if constexpr (N == 6) 
			{
				data = 0x25;
			}
			else 
			{
				static_assert(false && N, "invalid branch size");
			}

			return write_branch<N>(a_src, a_dst, data);
		}

		template <size_t N, class F>
		uintptr_t write_branch(uintptr_t a_src, F a_dst)
		{
			return write_branch<N>(a_src, stl::unrestricted_cast<uintptr_t>(a_dst));
		}

		template <size_t N>
		uintptr_t write_call(uintptr_t a_src, uintptr_t a_dst)
		{
			uint8_t data = 0;
			if constexpr (N == 5) 
			{
				data = 0xE8;
			}
			else if constexpr (N == 6) 
			{
				data = 0x15;
			}
			else 
			{
				static_assert(false && N, "invalid call size");
			}

			return write_branch<N>(a_src, a_dst, data);
		}

		template <size_t N, class F>
		uintptr_t write_call(uintptr_t a_src, F a_dst)
		{
			return write_call<N>(a_src, stl::unrestricted_cast<uintptr_t>(a_dst));
		}

		// gives plugin some of the pooled memory.
		void* allocatePlugin(size_t a_handle, size_t a_size);
	protected:
		void write_5branch(uintptr_t a_src, uintptr_t a_dst, uint8_t a_opcode)
		{
#pragma region
#pragma pack(push, 1)
			// 5
			struct Assembly5
			{
				uint8_t opcode;
				int32_t disp;
			};
			static_assert(offsetof(Assembly5, opcode) == 0);
			static_assert(offsetof(Assembly5, disp) == 1);
			static_assert(sizeof(Assembly5) == 5);

			// 14
			struct Assembly14
			{
				uint8_t  jmp;
				uint8_t  modrm;
				uint32_t disp;
				uint64_t addr;
			};
			static_assert(offsetof(Assembly14, jmp) == 0x0);
			static_assert(offsetof(Assembly14, modrm) == 0x1);
			static_assert(offsetof(Assembly14, disp) == 0x2);
			static_assert(offsetof(Assembly14, addr) == 0x6);
			static_assert(sizeof(Assembly14) == 14);
#pragma pack(pop)
#pragma endregion

			Assembly14* mem = nullptr;
			if (const auto it = m_5branches.find(a_dst); it != m_5branches.end())
			{
				mem = reinterpret_cast<Assembly14*>(it->second);
			}
			else
			{
				mem = allocate<Assembly14>();
				m_5branches.emplace(a_dst, reinterpret_cast<uint8_t*>(mem));
			}

			auto disp = reinterpret_cast<const uint8_t*>(mem) - reinterpret_cast<const uint8_t*>(a_src + sizeof(Assembly5));
			if (!vaild_rage(disp))
			{
				Notify("write_5branch faild due to branch pool being out of range.");
				__asm volatile ("int3");
			}

			Assembly5 assembly;
			assembly.opcode = a_opcode;
			assembly.disp = disp;
			safe_write(a_src, &assembly, sizeof(assembly));

			mem->jmp = static_cast<uint8_t>(0xFF);
			mem->modrm = static_cast<uint8_t>(0x25);
			mem->disp = static_cast<int32_t>(0);
			mem->addr = a_dst;
		}

		void write_6branch(uintptr_t a_src, uintptr_t a_dst, uint8_t a_modrm)
		{
#pragma region
#pragma pack(push, 1)
			struct Assembly6
			{
				uint8_t opcode;
				uint8_t modrm;
				int32_t disp;
			};
			static_assert(offsetof(Assembly6, opcode) == 0x0);
			static_assert(offsetof(Assembly6, modrm) == 0x1);
			static_assert(offsetof(Assembly6, disp) == 0x2);
			static_assert(sizeof(Assembly6) == 6);
#pragma pack(pop)
#pragma endregion
			uintptr_t* mem = nullptr;
			if (const auto it = m_6branches.find(a_dst); it != m_6branches.end())
			{
				mem = reinterpret_cast<uintptr_t*>(it->second);
			}
			else
			{
				mem = allocate<uintptr_t>();
				m_6branches.emplace(a_dst, reinterpret_cast<uint8_t*>(mem));
			}

			const auto disp = reinterpret_cast<const uint8_t*>(mem) - reinterpret_cast<const uint8_t*>(a_src + sizeof(Assembly6));
			if (!vaild_rage(disp))
			{
				Notify("write_6branch faild due to branch pool being out of range.");
				__asm volatile ("int3");
			}

			Assembly6 assembly;
			assembly.opcode = static_cast<uint8_t>(0xFF);
			assembly.modrm = a_modrm;
			assembly.disp = static_cast<int32_t>(disp);
			safe_write(a_src, &assembly, sizeof(assembly));
			*mem = a_dst;
		}

		template <size_t N>
		uintptr_t write_branch(uintptr_t a_src, uintptr_t a_dst, uint8_t a_data)
		{
			const auto disp = reinterpret_cast<int32_t*>(a_src + N - 4);
			const auto nextOp = a_src + N;
			const auto func = nextOp + *disp;

			if constexpr (N == 5) 
			{
				write_5branch(a_src, a_dst, a_data);
			}
			else if constexpr (N == 6) 
			{
				write_6branch(a_src, a_dst, a_data);
			}
			else 
			{
				static_assert(false && N, "invalid branch size");
			}

			return func;
		}
		
		//
		void log_usage();

		//
		bool vaild_rage(ptrdiff_t a_disp) 
		{ 
			return a_disp >= INT32_MIN && a_disp <= INT32_MAX; 
		}

		// release all the memory that the trampoline will use(this should never be called pretty much).
		void release()
		{
			if (m_data && m_deallocator)
			{
				m_deallocator(m_data, m_capacity, m_physicalAddress);
			}
			
			//
			m_5branches.clear();
			m_6branches.clear();
			
			//
			m_physicalAddress = 0;
			m_data = nullptr;
			m_capacity = 0;
			m_size = 0;

			//
			m_PluginMap.clear();
		}
	public:
		// storage, names, and the deallocator
		std::map<uintptr_t, uint8_t*>				m_5branches;
		std::map<uintptr_t, uint8_t*>				m_6branches;
		std::function<void(void*, size_t, int64_t)>	m_deallocator;

		// Memory pool data
		int64_t										m_physicalAddress;			// 
		uint8_t*									m_data { nullptr };			// 
		size_t										m_capacity { 0 };			// allocated
		size_t										m_size { 0 };				// used

		// Plugin Data
		std::mutex									m_PluginMaplock;
		std::unordered_map<size_t, size_t>			m_PluginMap;
	};
}
#endif