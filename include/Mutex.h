#pragma once

#include <pthread.h>

#if defined(__ORBIS__)
#include <mutex>
#elif defined(__OPENORBIS__)
#include <orbis/libkernel.h>
#endif

namespace Mutex
{
	class Mutex
	{
	public:
		// using pthread_mutex_t = struct pthread_mutex*;
		// using pthread_mutexattr_t = struct pthread_mutex_attr*;
	public:
		Mutex(const char* name = NULL);
		~Mutex();

		bool TryLock();
		bool TryUnlock();
		bool Lock();
		bool Unlock();

		bool Enter() { return Lock(); }
		bool Leave() { return Unlock(); }
		bool TryEnter() { return TryLock(); }
	protected:
		pthread_mutex_t	    m_mutex;
		pthread_mutexattr_t m_mutexAttr;
	};

	// game
	extern Mutex GlobalMutex5;

	// UI
	extern Mutex GlobalMutex10;

	// Tasklet
	extern Mutex GlobalMutex11;
}