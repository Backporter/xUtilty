#pragma once
#include <type_traits>

namespace stl
{
	template<class _Ty, class _Uty>
	constexpr bool is_same_v = std::is_same<_Ty, _Uty>::value;

	template<class _From, class _To>
	constexpr bool is_convertible_v = std::is_convertible<_From, _To>::value;

	template<bool _Test, class _Ty>
	using enable_if_t = typename std::enable_if<_Test, _Ty>::type;
}