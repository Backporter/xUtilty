#pragma once
#include "IMemoryAllocator.h"
#include <stddef.h>
#include <set>
#include <map>
#include <queue>

namespace MEMORY
{
	template <typename T>
	class stl_allocator
	{
	public:
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;
		typedef T* pointer;
		typedef const T* const_pointer;
		typedef T& reference;
		typedef const T& const_reference;
		typedef T value_type;

		stl_allocator() {}
		~stl_allocator() {}

		pointer address(reference x) const { return &x; }
		const_pointer address(const_reference x) const { return &x; }
		size_type max_size() const throw() { return size_t(-1) / sizeof(value_type); }

		pointer allocate(size_type n, const T* hint = 0)
		{
			return static_cast<pointer>(IMemoryAllocator::Allocate(n * sizeof(T)));
		}

		void deallocate(pointer p, size_type n)
		{
			IMemoryAllocator::Deallocate(p);
		}

		void construct(pointer p, const T& val)
		{
			new(static_cast<void*>(p)) T(val);
		}

		void construct(pointer p)
		{
			new(static_cast<void*>(p)) T();
		}

		void destroy(pointer p)
		{
			p->~T();
		}
	};

	template<class _Kty, class _Pr = std::less<_Kty>, class _Alloc = stl_allocator<_Kty>>
	using set = std::set<_Kty, _Pr, _Alloc>;

	template<class _Kty, class _Ty, class _Pr = std::less<_Kty>, class _Alloc = stl_allocator<std::pair<const _Kty, _Ty>>>
	using map = std::map<_Kty, _Ty, _Pr, _Alloc>;

	template<class _Ty, class _Alloc = stl_allocator<_Ty>>
	using vector = std::vector<_Ty, _Alloc>;

	using string = std::basic_string<char, std::char_traits<char>, stl_allocator<char>>;
	using wstring = std::basic_string<wchar_t, std::char_traits<wchar_t>, stl_allocator<wchar_t>>;
}