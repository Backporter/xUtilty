#include "../include/Mutex.h"
#include "../include/MessageHandler.h"

// game
Mutex GlobalMutex5;

// UI
Mutex GlobalMutex10;

// tasklet
Mutex GlobalMutex11;

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

	PRINT_POS;
	return false;
}

bool Mutex::Lock()
{
	if (!pthread_mutex_lock(&m_mutex))
	{
		return true;
	}

	PRINT_POS;
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

		PRINT_POS;
		return false;
	}

	PRINT_POS;
	return false;
}

bool Mutex::Unlock()
{
	if (!pthread_mutex_unlock(&m_mutex))
	{
		return true;
	}

	PRINT_POS;
	return false;
}