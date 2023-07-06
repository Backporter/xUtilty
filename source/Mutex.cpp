#include "../include/Mutex.h"

namespace xUtilty
{
	Mutex::Mutex(const char* name)
	{
#if __clang__
		if (!pthread_mutexattr_init(&m_mutexAttr))
		{
			pthread_mutex_init(&m_mutex, &m_mutexAttr);
		}
#elif _WIN32 || _WIN64
		::InitializeCriticalSection(&m_crit);
#endif
	}

	Mutex::~Mutex()
	{
#if __clang__
		if (!pthread_mutexattr_destroy(&m_mutexAttr))
		{
			pthread_mutex_destroy(&m_mutex);
		}
#elif _WIN32 || _WIN64
		DeleteCriticalSection(&m_crit);
#endif
	}

	bool Mutex::TryLock()
	{
#if __clang__
		if (!pthread_mutex_trylock(&m_mutex))
		{
			return true;
		}
#elif _WIN32 || _WIN64
		if (::TryEnterCriticalSection(&m_crit) != 0)
		{
			return true;
		}

#endif
		return false;
	}

	bool Mutex::Lock()
	{
#if __clang__
		if (!pthread_mutex_lock(&m_mutex))
		{
			return true;
		}
#elif _WIN32 || _WIN64
		::EnterCriticalSection(&m_crit);
		return true;
#endif
		return false;
	}

	bool Mutex::TryUnlock()
	{
#if __clang__
		if (!pthread_mutex_unlock(&m_mutex))
		{
			if (!pthread_mutex_lock(&m_mutex))
			{
				return true;
			}

			return false;
		}
#elif _WIN32 || _WIN64
		::LeaveCriticalSection(&m_crit);
		return true;
#endif
		return false;
	}

	bool Mutex::Unlock()
	{
#if __clang__
		if (!pthread_mutex_unlock(&m_mutex))
		{
			return true;
		}
#else
#endif
		return false;
	}
}