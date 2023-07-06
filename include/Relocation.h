#pragma once

#include "type_traits.h"
#include "stl.h"

#include "RelocationManager.h"
#include "OffsertManger.h"
#include "MemoryHandler.h"

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <type_traits>

namespace REL
{
	namespace detail
	{
		template <class T>
		struct meets_length_req : std::disjunction<std::integral_constant<bool, sizeof(T) == 1>, std::integral_constant<bool, sizeof(T) == 2>, std::integral_constant<bool, sizeof(T) == 4>, std::integral_constant<bool, sizeof(T) == 8>>
		{};

		template <class T>
		struct meets_function_req : std::conjunction<std::is_trivially_constructible<T>, std::is_trivially_destructible<T>, std::is_trivially_copy_assignable<T>, std::negation<std::is_polymorphic<T>>>
		{};

		template <class T>
		struct meets_member_req : std::is_standard_layout<T>
		{};

		template <class T, class = void>
		struct is_x64_pod : std::true_type
		{};

		template <class T>
		struct is_x64_pod<T, std::enable_if_t<std::is_union_v<T>>> : std::false_type
		{};

		template <class T>
		struct is_x64_pod<T, std::enable_if_t<std::is_class_v<T>>> : std::conjunction<meets_length_req<T>, meets_function_req<T>, meets_member_req<T>>
		{};

		template <class T>
		inline constexpr bool is_x64_pod_v = is_x64_pod<T>::value;

		template <class F>
		struct member_function_pod_type;

		template <class R, class Cls, class... Args>
		struct member_function_pod_type<R(Cls::*)(Args...)>
		{
			using type = R(Cls*, Args...);
		};

		template <class R, class Cls, class... Args>
		struct member_function_pod_type<R(Cls::*)(Args...) const>
		{
			using type = R(const Cls*, Args...);
		};

		template <class R, class Cls, class... Args>
		struct member_function_pod_type<R(Cls::*)(Args..., ...)>
		{
			using type = R(Cls*, Args..., ...);
		};

		template <class R, class Cls, class... Args>
		struct member_function_pod_type<R(Cls::*)(Args..., ...) const>
		{
			using type = R(const Cls*, Args..., ...);
		};

		template <class F>
		using member_function_pod_type_t = typename member_function_pod_type<F>::type;

		template <class F>
		struct member_function_non_pod_type;

		template <class R, class Cls, class... Args>
		struct member_function_non_pod_type<R(Cls::*)(Args...)>
		{
			using type = R&(Cls*, void*, Args...);
		};

		template <class R, class Cls, class... Args>
		struct member_function_non_pod_type<R(Cls::*)(Args...) const>
		{
			using type = R&(const Cls*, void*, Args...);
		};

		template <class R, class Cls, class... Args>
		struct member_function_non_pod_type<R(Cls::*)(Args..., ...)>
		{
			using type = R&(Cls*, void*, Args..., ...);
		};

		template <class R, class Cls, class... Args>
		struct member_function_non_pod_type<R(Cls::*)(Args..., ...) const>
		{
			using type = R&(const Cls*, void*, Args..., ...);
		};

		template <class F>
		using member_function_non_pod_type_t = typename member_function_non_pod_type<F>::type;

		template <class F, class First, class... Rest>
		decltype(auto) invoke_member_function_non_pod(F&& a_func, First&& a_first, Rest&&... a_rest)
		{
			using result_t = std::result_of_t<F(First, Rest...)>;
			std::aligned_storage_t<sizeof(result_t), alignof(result_t)> result;

			using func_t = member_function_non_pod_type_t<F>;
			auto func = stl::unrestricted_cast<func_t*>(std::forward<F>(a_func));

			return func(std::forward<First>(a_first), std::addressof(result), std::forward<Rest>(a_rest)...);
		}
	}

	template <class F, class... Args>
	std::result_of_t<F(Args...)> invoke(F&& a_func, Args&&... a_args)
	{
		if constexpr (std::is_member_function_pointer_v<std::decay_t<F>>)
		{
			if constexpr (detail::is_x64_pod_v<std::result_of_t<F(Args...)>>)
			{
				using func_t = detail::member_function_pod_type_t<std::decay_t<F>>;
				auto func = stl::unrestricted_cast<func_t*>(std::forward<F>(a_func));
				return func(std::forward<Args>(a_args)...);
			}
			else
			{
				return detail::invoke_member_function_non_pod(std::forward<F>(a_func), std::forward<Args>(a_args)...);
			}
		}
		else
		{
			return std::forward<F>(a_func)(std::forward<Args>(a_args)...);
		}
	}

	template <size_t N>
	class Name
	{
	public:
		constexpr Name() noexcept = default;

		explicit constexpr Name(const char* a_name) noexcept :
			m_name(a_name)
		{}

		constexpr Name& operator=(const char* a_name) noexcept
		{
			m_name = a_name;
			return *this;
		}

		uintptr_t				address() const { return xUtilty::RelocationManager::RelocationManager::ApplicationBaseAddress[N] + offset(); }
		constexpr const char*	name() const noexcept { return m_name; }
		uintptr_t				offset() const { return xUtilty::OffsetManger::GetSingleton().GetOffsetFromName(m_name); }
	public:
		const char* m_name { 0 };
	};

	template <size_t N>
	class Offset
	{
	public:
		constexpr Offset() noexcept = default;

		explicit constexpr Offset(size_t a_offset) noexcept : 
			m_offset(a_offset)
		{}

		constexpr Offset& operator=(size_t a_offset) noexcept
		{
			m_offset = a_offset;
			return *this;
		}

		constexpr size_t offset() const noexcept 
		{ 
			return m_offset; 
		}

		uintptr_t address() const 
		{ 
			return offset() + xUtilty::RelocationManager::RelocationManager::ApplicationBaseAddress[N];
		}
	public:
		size_t m_offset{ 0 };
	};

	template <size_t N>
	class ID
	{
	public:
		constexpr ID() noexcept
		{}

		explicit constexpr ID(uint64_t a_id) noexcept : m_id(a_id)
		{}

		constexpr ID& operator=(uint64_t a_id) noexcept
		{
			m_id = a_id;
			return *this;
		}

		uintptr_t          address() const { return xUtilty::RelocationManager::RelocationManager::ApplicationBaseAddress[N] + offset(); }
		constexpr uint64_t id() const noexcept { return m_id; }
		size_t             offset() const { return xUtilty::OffsetManger::GetSingleton().GetOffsetFromID(m_id); }
	public:
		uint64_t m_id{ 0 };
	};

	template <typename T, size_t N = 0>
	class Relocation
	{
	public:
		using value_type = std::conditional_t<std::is_member_pointer_v<T> || std::is_function_v<std::remove_pointer_t<T>>, std::decay_t<T>, T>;

		constexpr Relocation() noexcept = default;

		constexpr Relocation(uintptr_t a_offset) noexcept :
			m_offset{ a_offset + xUtilty::RelocationManager::RelocationManager::ApplicationBaseAddress[N] }
		{}

		constexpr Relocation(uint32_t a_id, uintptr_t a_offset) :
			m_offset{ a_offset + xUtilty::RelocationManager::RelocationManager::ApplicationBaseAddress[N] }
		{}

		constexpr Relocation(uint32_t a_id, uint32_t a_id2, uintptr_t a_offset) :
			m_offset{ a_offset + xUtilty::RelocationManager::RelocationManager::ApplicationBaseAddress[N] }
		{}

		constexpr Relocation(const char* a_id, uintptr_t a_offset) : Relocation(static_cast<uint32_t>(0), a_offset)
		{}


		constexpr Relocation(const char* a_id, const char* a_id2, uintptr_t a_offset) : Relocation(static_cast<uint32_t>(0), static_cast<uint32_t>(0), a_offset)
		{}

		constexpr Relocation& operator=(uintptr_t a_address) noexcept
		{
			m_offset = a_address;
			return *this;
		}

		decltype(auto) operator*()
		{
			return *get();
		}

		auto	operator->()
		{
			return get();
		}

		template <class... Args>
		std::result_of_t<value_type(Args...)> operator() (Args&&... a_args)
		{
			return REL::invoke(get(), std::forward<Args>(a_args)...);
		}

		constexpr uintptr_t address() const noexcept { return m_offset; }
		size_t              offset() const { return m_offset - xUtilty::RelocationManager::RelocationManager::ApplicationBaseAddress[N]; }

		value_type get()
		{
			assert(m_offset != 0);
			return stl::unrestricted_cast<value_type>(m_offset);
		}

		template <class U = value_type>
		uintptr_t write_vfunc(size_t a_idx, uintptr_t a_newFunc)
		{
			const auto addr = address() + (sizeof(void*) * a_idx);
			const auto result = *reinterpret_cast<uintptr_t*>(addr);
			xUtilty::safe_write(addr, a_newFunc);
			return result;
		}

		template <typename F>
		uintptr_t write_vfunc(size_t a_idx, F a_newFunc)
		{
			return write_vfunc(a_idx, stl::unrestricted_cast<uintptr_t>(a_newFunc));
		}
	private:
		uintptr_t m_offset{ 0 };
	};
}

template <typename T, size_t N = 0>
using Relocation = REL::Relocation<T, N>;