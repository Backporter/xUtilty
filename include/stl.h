#pragma once

#include "type_traits.h"

namespace stl
{
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