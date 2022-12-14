#pragma once
#include "std.h"
#include "Mutex.h"

namespace Mutex
{
	template <typename T>
	class MutexType
	{
	public:
		MutexType(const char* a_mutexname = NULL) : Mutex(a_mutexname) { }
		~MutexType() { }
	public:
		T&	   type()  { return m_type; }
		Mutex& mutex() { return m_mutex; }
	public:
		template <typename T, typename = stl::enable_if_t<stl::is_stl_container<T>, void>>
		void push_back(T::value_type& a_type) 
		{

		}
	protected:
		T		m_type;
		Mutex	m_mutex;
	};
}