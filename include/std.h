#pragma once
#include <type_traits>
#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include <type_traits>

namespace stl
{
	template<class _Ty, class _Uty>
	constexpr bool is_same_v = std::is_same<_Ty, _Uty>::value;

	template<class _From, class _To>
	constexpr bool is_convertible_v = std::is_convertible<_From, _To>::value;

	template<bool _Test, class _Ty>
	using enable_if_t = typename std::enable_if<_Test, _Ty>::type;

	template<class _Ty>
	using decay_t = typename std::decay<_Ty>::type;

	template <typename T> struct is_stl_container { static constexpr bool const value = false; };
	template <typename T, std::size_t N> struct is_stl_container<std::array<T, N>> { static constexpr bool const value = true; };
	template <typename... Args> struct is_stl_container<std::vector<Args...>> { static constexpr bool const value = true; };
	template <typename... Args> struct is_stl_container<std::deque<Args...>> { static constexpr bool const value = true; };
	template <typename... Args> struct is_stl_container<std::list<Args...>> { static constexpr bool const value = true; };
	template <typename... Args> struct is_stl_container<std::forward_list<Args...>> { static constexpr bool const value = true; };
	template <typename... Args> struct is_stl_container<std::set<Args...>> { static constexpr bool const value = true; };
	template <typename... Args> struct is_stl_container<std::multiset<Args...>> { static constexpr bool const value = true; };
	template <typename... Args> struct is_stl_container<std::map<Args...>> { static constexpr bool const value = true; };
	template <typename... Args> struct is_stl_container<std::multimap<Args...>> { static constexpr bool const value = true; };
	template <typename... Args> struct is_stl_container<std::unordered_set<Args...>> { static constexpr bool const value = true; };
	template <typename... Args> struct is_stl_container<std::unordered_multiset<Args...>> { static constexpr bool const value = true; };
	template <typename... Args> struct is_stl_container<std::unordered_map<Args...>> { static constexpr bool const value = true; };
	template <typename... Args> struct is_stl_container<std::unordered_multimap<Args...>> { static constexpr bool const value = true; };
	template <typename... Args> struct is_stl_container<std::stack<Args...>> { static constexpr bool const value = true; };
	template <typename... Args> struct is_stl_container<std::queue<Args...>> { static constexpr bool const value = true; };
	template <typename... Args> struct is_stl_container<std::priority_queue<Args...>> { static constexpr bool const value = true; };
	template <typename T> using is_stl_container_v = typename is_stl_container<decay_t<T>>::value;
}