#pragma once

#include "type_traits.h"

// C++17+
#if __cplusplus < 201703L
#include <functional>
#include <utility>
#elif __cplusplus >= 201703L
#include <optional>
#endif

namespace stl
{
#if __cplusplus < 201703L
	template <typename T>
	class scope_exit
	{
	public:
		scope_exit(T* const a_structure)
		{
			_structure = a_structure;
		}

		~scope_exit()
		{
			if (_structure)
			{
				(*_structure)--;
			}
		}
	public:
		T* _structure { nullptr };
	};
#elif __cplusplus >= 201703L
	template <typename EF>
	class scope_exit
	{
	public:
		template <class Fn>
		explicit scope_exit(Fn&& a_fn)
		{
			if constexpr (!std::is_lvalue_reference_v<Fn> && std::is_nothrow_constructible_v<EF, Fn>) 
			{
				_fn.emplace(std::forward<Fn>(a_fn));
			}
			else 
			{
				_fn.emplace(a_fn);
			}
		}
	
		~scope_exit() noexcept
		{
			if (_fn.has_value()) 
			{
				(*_fn)();
			}
		}
	public:
		std::optional<std::remove_reference_t<EF>> _fn;	
	};
	
	template <class EF>
	scope_exit(EF)->scope_exit<EF>;
#endif

	template <class To, class From>
	To unrestricted_cast(From a_from) noexcept
	{
		if constexpr (std::is_same_v<std::remove_cv_t<From>, std::remove_cv_t<To>>)
		{
			return To{ a_from };
		}
		else if constexpr (std::is_reference_v<From>)
		{
			return stl::unrestricted_cast<To>(std::addressof(a_from));
		}
		else if constexpr (std::is_reference_v<To>)
		{
			return *stl::unrestricted_cast<std::add_pointer_t<std::remove_reference_t<To>>>(a_from);
		}
		else if constexpr (std::is_pointer_v<From> && std::is_pointer_v<To>)
		{
			return static_cast<To>(const_cast<void*>(static_cast<const volatile void*>(a_from)));
		}
		else if constexpr ((std::is_pointer_v<From> && std::is_integral_v<To>) || (std::is_integral_v<From> && std::is_pointer_v<To>))
		{
			return reinterpret_cast<To>(a_from);
		}
		else
		{
			union
			{
				std::remove_cv_t<std::remove_reference_t<From>> from;
				std::remove_cv_t<std::remove_reference_t<To>>   to;
			};

			from = std::forward<From>(a_from);
			return to;
		}
	}
}