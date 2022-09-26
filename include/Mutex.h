#pragma once
#include "SystemWrapper.h"

#if defined(__ORBIS__)
#include <mutex>
#elif defined(__OPENORBIS__)
#include <orbis/libkernel.h>
typedef OrbisPthreadMutex ScePthreadMutex;
typedef OrbisPthreadMutexattr ScePthreadMutexattr;
#endif

#include <pthread.h>

namespace OrbisMutex
{
	class OrbisMutex
	{
	public:
		OrbisMutex(const char* name = NULL);
		~OrbisMutex();

		bool TryLock();
		bool Lock();

		bool TryUnlock();
		bool Unlock();

		bool IsOk();
		void PrintLastError();

	private:
		int			RET;
		const char* LASTFN;

		pthread_mutex_t	    mutex;
		pthread_mutexattr_t mutexattr;
	};

	// game
	extern OrbisMutex GlobalMutex5;

	// UI
	extern OrbisMutex GlobalMutex10;
}