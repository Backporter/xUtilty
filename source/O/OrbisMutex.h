#pragma once


#if defined(__ORBIS__)
#include <pthread.h>
#include <mutex>
#elif defined(__OPENORBIS__)
#include <orbis/libkernel.h>
#include <pthread.h>
typedef OrbisPthreadMutex ScePthreadMutex;
typedef OrbisPthreadMutexattr ScePthreadMutexattr;
#endif

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

		ScePthreadMutex		mutex;
		ScePthreadMutexattr mutexattr;
	};

	extern OrbisMutex GlobalMutex;
}