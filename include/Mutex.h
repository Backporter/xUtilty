#pragma once

#if __clang__
// pthread_mutex_t, pthread_mutexattr_t
#include <pthread.h>
#elif _WIN32 || _WIN64
#include <Windows.h>
#include <synchapi.h>
#endif

namespace xUtilty
{
	class Mutex
	{
	public:
		enum Type { kGame, kUI, kTasklet, kReserved1, kReserved2, kReserved3, kReserved4, kTotal };
	public:
		Mutex(const char* a_name = NULL);
		~Mutex();

		static Mutex& GetSingleton(int a_mutexType)
		{
			static Mutex Singleton[kTotal];
			return Singleton[a_mutexType];
		}

		// 
		bool TryLock();
		bool TryUnlock();
		bool Lock();
		bool Unlock();
	protected:
#if __clang__
		pthread_mutex_t	    m_mutex;
		pthread_mutexattr_t m_mutexAttr;
#elif _WIN32 || _WIN64
		CRITICAL_SECTION	m_crit;
#endif
	};

	template <typename T>
	class scoped_lock
	{
	public:
		scoped_lock(T& a_lock) : m_lock(a_lock) { m_lock.Lock(); }
		~scoped_lock() { m_lock.Unlock(); }
	public:
		T& m_lock;
	};
}