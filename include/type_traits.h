#pragma once

#include <type_traits>
#include <string>
#include <stddef.h>
#include "Macro.h"

#if __clang__

// if C++ version < c++14, error and write out the version.
#if __cplusplus < 201402
// PRINT_CPPVERSION(__cplusplus);
#endif

// _t/_v of C++/+=14 types.
namespace std
{
	// _t:
#if __cplusplus < 201402
	template <bool _Test, class _Ty>
	using enable_if_t = typename enable_if<_Test, _Ty>::type;

	template <class _Ty>
	using decay_t = typename decay<_Ty>::type;

	template <class _Ty>
	using remove_cv_t = typename remove_cv<_Ty>::type;

	template <class _Ty>
	using remove_pointer_t = typename remove_pointer<_Ty>::type;

	template <class T>
	using underlying_type_t = typename underlying_type<T>::type;

	template<bool _Test, class _Ty1, class _Ty2>
	using conditional_t = typename conditional<_Test, _Ty1, _Ty2>::type;
#endif

#if __cplusplus < 201703
	// c++17
	template <bool B>
	using bool_constant = integral_constant<bool, B>;

	template<class...>
	using void_t = void;
#endif

#if __cplusplus < 201402
	// c++14 but deprecated in C++17+
	template <class T>
	using result_of_t = typename result_of<T>::type;

	// c++14 but deprecated in C++23+
	template< size_t Len, size_t Align>
	using aligned_storage_t = typename aligned_storage<Len, Align>::type;
#endif

#if __cplusplus < 201703
	template< class T, class... Args >
	inline constexpr bool is_constructible_v = is_constructible<T, Args...>::value;

	template< class T, class... Args >
	inline constexpr bool is_trivially_constructible_v = is_trivially_constructible<T, Args...>::value;

	template< class T, class... Args >
	inline constexpr bool is_nothrow_constructible_v = is_nothrow_constructible<T, Args...>::value;

	// _v:
	template<class _From, class _To>
	inline constexpr bool is_convertible_v = is_convertible<_From, _To>::value;

	template< class T>
	inline constexpr size_t tuple_size_v = tuple_size<T>::value;

	template<class _Ty, class _Uty>
	inline constexpr bool is_same_v = is_same<_Ty, _Uty>::value;

	template< class T >
	inline constexpr bool is_enum_v = is_enum<T>::value;

	template< class T >
	inline constexpr bool is_member_pointer_v = is_member_pointer<T>::value;

	template< class T >
	inline constexpr bool is_reference_v = is_reference<T>::value;

	template< class T >
	inline constexpr bool is_pointer_v = is_pointer<T>::value;

	template< class T >
	inline constexpr bool is_union_v = is_union<T>::value;

	template< class T >
	inline constexpr bool is_class_v = is_class<T>::value;

	template< class T >
	inline constexpr bool is_member_function_pointer_v = is_member_function_pointer<T>::value;

	template< class T >
	inline constexpr bool is_function_v = is_function<T>::value;

	template< class T >
	inline constexpr bool is_lvalue_reference_v = is_lvalue_reference<T>::value;

	template< class T >
	inline constexpr bool is_empty_v = is_empty<T>::value;
#endif
}

// if C++ version < C++17
#if __cplusplus < 201703
// C++17 type_traits.
namespace std
{
	// TEMPLATE STRUCT negation
	template<class _Base>
	struct negation : integral_constant<bool, !_Base::value>
	{	// negate type of _Base
	};

	// TEMPLATE STRUCT conjunction
	template<class... _Bases>
	struct conjunction;

	template<>
	struct conjunction<> : true_type
	{	// empty list is true
	};

	template<class _Base>
	struct conjunction<_Base> : _Base
	{	// last base in line supplies value
	};


	template<class _Base, class... _Rest>
	struct conjunction<_Base, _Rest...> : integral_constant<bool, conditional<_Base::value == false, _Base, conjunction<_Rest...> >::type::value>
	{	// last two bases in line
	};

	// TEMPLATE STRUCT disjunction
	template<class... _Bases>
	struct disjunction;

	template<>
	struct disjunction<> : false_type
	{	// empty list is false
	};

	template<class _Base>
	struct disjunction<_Base> : _Base
	{	// last base in line supplies value
	};

	template<class _Base, class... _Rest>
	struct disjunction<_Base, _Rest...> : integral_constant<bool, conditional<_Base::value == true, _Base, disjunction<_Rest...> >::type::value>
	{	// last two bases in line
	};

}
#endif

namespace std
{
	template <typename T>
	struct is_not_const : negation<is_const<T>>
	{
	};

	template <typename T>
	struct is_not_volatile : negation<is_volatile<T>>
	{
	};

	template <typename T>
	struct is_not_reference : negation<is_reference<T>>
	{
	};

	template <typename T>
	struct is_not_pointer : negation<is_pointer<T>>
	{
	};

	// add custom.
	template <class T>
	struct _is_integer : is_integral<T>
	{
	};

	template <>
	struct _is_integer<bool> : false_type
	{
	};

	template <class T>
	struct is_integer : _is_integer<remove_cv_t<T>>
	{
	};

	template <class T>
	struct is_signed_integral : conjunction<is_integer<T>, is_signed<T>, integral_constant<bool, sizeof(T) == 4>>
	{
	};

	template <class T>
	struct is_unsigned_integral : conjunction<is_integer<T>, is_unsigned<T>, integral_constant<bool, sizeof(T) == 4>>
	{
	};
		
	template <class T>
	struct is_integral_ : disjunction<is_signed_integral<T>, is_unsigned_integral<T>>
	{
	};

	template <class T>
	struct is_floating_point_ : is_same<remove_cv_t<T>, float>
	{
	};

	template <class T>
	struct is_boolean_ : is_same<remove_cv_t<T>, bool>
	{
	};

	// if used in say, skyrim/fallout, B should equal BSFixedString, otherwise normally, string.
	template <class T, class B>
	struct is_string : is_same<remove_cv_t<T>, B>
	{
	};

	template <class T, class B>
	struct is_builtin : disjunction<is_void<T>, is_string<T, B>, is_signed_integral<T>, is_unsigned_integral<T>, is_floating_point<T>, is_boolean_<T>>
	{
	};

	template <class T>
	struct is_string_convertible : is_convertible<T, string>
	{
	};

	template <class T, class = void>
	struct is_signed_integral_convertible : conjunction<is_integer<T>, is_signed<T>>
	{
	};

	template <class T>
	struct is_signed_integral_convertible<T, enable_if_t<is_enum_v<T>>> : is_signed<underlying_type_t<T>>
	{
	};

	template <class T, class = void>
	struct is_unsigned_integral_convertible : conjunction<is_integer<T>, is_unsigned<T>>
	{
	};

	template <class T>
	struct is_unsigned_integral_convertible<T, enable_if_t<is_enum_v<T>>> : is_unsigned<underlying_type_t<T>>
	{
	};

	template <class T>
	struct is_integral_convertible : disjunction<is_signed_integral_convertible<T>, is_unsigned_integral_convertible<T>>
	{
	};

	template <class T>
	struct is_floating_point_convertible : is_floating_point_<T>
	{
	};

	template <class T>
	struct is_builtin_convertible : disjunction<is_void<T>, is_string_convertible<T>, is_signed_integral_convertible<T>, is_unsigned_integral_convertible<T>, is_floating_point_convertible<T>, is_boolean_<T>>
	{
	};

	template <class T>
	struct index_sequence_for_tuple : make_index_sequence<tuple_size_v<decay_t<T>>>
	{
	};

	template <class T>
	struct decay_pointer : decay_t<remove_pointer<T>>
	{
	};
}

// _t/_v of C++/+=17 types(added by ^).
namespace std
{
	// _t:
	template <class T>
	using decay_pointer_t = typename decay_pointer<T>::type;

	// _v:
#if __cplusplus < 201703
	template< class B>
	inline constexpr bool negation_v = negation<B>::value;

	template <class... _Traits>
	inline constexpr bool disjunction_v = disjunction<_Traits...>::value;

	template <class T>
	inline constexpr bool is_integral_v = is_integral<T>::value;

	template <class T>
	inline constexpr bool is_floating_point_v = is_floating_point<T>::value;

	template< class... B >
	inline constexpr bool conjunction_v = conjunction<B...>::value;
#endif


	template <class T>
	inline constexpr bool is_integer_v = is_integer<T>::value;

	template <class T>
	inline constexpr bool is_signed_integral_v = is_signed_integral<T>::value;

	template <class T>
	inline constexpr bool is_unsigned_integral_v = is_unsigned_integral<T>::value;

	template <class T>
	inline constexpr bool is_boolean_v = is_boolean_<T>::value;

	template <class T, class B = string>
	inline constexpr bool is_string_v = is_string<T, B>::value;

	template <class T, class B = string>
	inline constexpr bool is_builtin_v = is_builtin<T, B>::value;

	template <class T>
	inline constexpr bool is_string_convertible_v = is_string_convertible<T>::value;

	template <class T>
	inline constexpr bool is_signed_integral_convertible_v = is_signed_integral_convertible<T>::value;

	template <class T>
	inline constexpr bool is_unsigned_integral_convertible_v = is_unsigned_integral_convertible<T>::value;

	template <class T>
	inline constexpr bool is_integral_convertible_v = is_integral_convertible<T>::value;

	template <class T>
	inline constexpr bool is_floating_point_convertible_v = is_floating_point_convertible<T>::value;

	template <class T>
	inline constexpr bool is_builtin_convertible_v = is_builtin_convertible<T>::value;

	template <class T>
	inline constexpr bool is_not_const_v = is_not_const<T>::value;

	template <class T>
	inline constexpr bool is_not_volatile_v = is_not_volatile<T>::value;

	template <class T>
	inline constexpr bool is_not_reference_v = is_not_reference<T>::value;

	template <class T>
	inline constexpr bool is_not_pointer_v = is_not_pointer<T>::value;

}
#endif