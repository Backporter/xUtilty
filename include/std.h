#pragma once

#include <type_traits>
#include <utility>
#include <iterator>

namespace std
{
	template <typename _Tp>
	inline void destroy_at(_Tp* __location)
	{
		if constexpr (__cplusplus > 201703L && std::is_array<_Tp>::value)
		{
			for (auto& __x : *__location)
				destroy_at(std::addressof(__x));
		}
		else
			__location->~_Tp();
	}

	template<typename _Tp, typename... _Args>
	constexpr auto
		construct_at(_Tp* __location, _Args&&... __args)
		noexcept(noexcept(::new((void*)0) _Tp(std::declval<_Args>()...)))
		-> decltype(::new((void*)0) _Tp(std::declval<_Args>()...))
	{
		return ::new((void*)__location) _Tp(std::forward<_Args>(__args)...);
	}

	template<class _InIt, class _Diff, class _FwdIt> inline
		std::pair<_InIt, _FwdIt> uninitialized_move_n(_InIt _First, _Diff _Count, _FwdIt _Dest)
	{	// move [_First, _First + _Count) to [_Dest, ...)
		for (; 0 < _Count; ++_First, (void)++_Dest, --_Count)
			::new (static_cast<void *>(std::addressof(*_Dest)))
			typename std::iterator_traits<_FwdIt>::value_type(std::move(*_First));
		return (std::pair<_InIt, _FwdIt>(_First, _Dest));
	}

	template<class _FwdIt, class Size>
	_FwdIt uninitialized_default_construct_n(_FwdIt _First, Size _Count)
	{	// default construct by count
		for (; 0 < _Count; (void)++_First, --_Count)
			::new (static_cast<void *>(std::addressof(*_First)))
			typename std::iterator_traits<_FwdIt>::value_type;
		return (_First);
	}


	template<class _FwdIt, class _Size>
	_FwdIt destroy_n(_FwdIt _First, _Size _Count)
	{	// destroy by count
		for (; 0 < _Count; (void)++_First, --_Count)
			destroy_at(std::addressof(*_First));
		return (_First);
	}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wlogical-op-parentheses"
	template <typename _Fp>
	constexpr _Fp _lerp(_Fp a, _Fp b, _Fp t)
	{
		if (a <= 0 && b >= 0 || a >= 0 && b <= 0)
			return t * b + (1 - t) * a;

		if (t == 1)
			return b;

		const _Fp x = a + t * (b - a);
		return t > 1 == b > a ? (b < x ? x : b) : (b > x ? x : b);
	}
#pragma clang diagnostic pop

	inline constexpr float lerp(float a, float b, float t) noexcept 
	{ 
		return _lerp(a, b, t); 
	}

	inline constexpr double lerp(double a, double b, double t) noexcept 
	{ 
		return _lerp(a, b, t); 
	}

	inline constexpr long double lerp(long double a, long double b, long double t) noexcept 
	{ 
		return _lerp(a, b, t); 
	}
}