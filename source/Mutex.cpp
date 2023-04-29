#include "../include/Mutex.h"

namespace xUtilty
{
	Mutex::Mutex(const char* name)
	{
		if (!pthread_mutexattr_init(&m_mutexAttr))
		{
			pthread_mutex_init(&m_mutex, &m_mutexAttr);
		}
	}

	Mutex::~Mutex()
	{
		if (!pthread_mutexattr_destroy(&m_mutexAttr))
		{
			pthread_mutex_destroy(&m_mutex);
		}

	}

	bool Mutex::TryLock()
	{
		if (!pthread_mutex_trylock(&m_mutex))
		{
			return true;
		}

		return false;
	}

	bool Mutex::Lock()
	{
		if (!pthread_mutex_lock(&m_mutex))
		{
			return true;
		}

		return false;
	}

	bool Mutex::TryUnlock()
	{
		if (!pthread_mutex_unlock(&m_mutex))
		{
			if (!pthread_mutex_lock(&m_mutex))
			{
				return true;
			}

			return false;
		}

		return false;
	}

	bool Mutex::Unlock()
	{
		if (!pthread_mutex_unlock(&m_mutex))
		{
			return true;
		}

		return false;
	}
}