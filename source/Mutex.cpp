#include "../include/Mutex.h"
#include "../include/MessageHandler.h"

namespace OrbisMutex
{
	// game
	OrbisMutex GlobalMutex5;

	// UI
	OrbisMutex GlobalMutex10;

	OrbisMutex::OrbisMutex(const char* name):
		LASTFN(nullptr), RET(0)
	{
		if ((this->RET = pthread_mutexattr_init(&this->mutexattr)) != 0)
		{
			this->LASTFN = "pthread_mutexattr_init";
			this->PrintLastError();
			return;
		}

		if ((this->RET = pthread_mutex_init(&this->mutex, &this->mutexattr)) != 0)
		{
			this->LASTFN = "pthread_mutex_init";
			this->PrintLastError();
			return;
		}
		
	}

	OrbisMutex::~OrbisMutex()
	{
		if ((this->RET = pthread_mutexattr_destroy(&this->mutexattr)) != 0)
		{
			this->LASTFN = "pthread_mutexattr_destroy";
			this->PrintLastError();
			return;
		}

		if ((this->RET = pthread_mutex_destroy(&this->mutex)) != 0)
		{
			this->LASTFN = "pthread_mutex_destroy";
			this->PrintLastError();
			return;
		}
	}

	bool OrbisMutex::TryLock()
	{
		if ((this->RET = pthread_mutex_trylock(&this->mutex)) == 0)
			return true;
		
		this->LASTFN = "pthread_mutex_trylock";
		return false;

	}

	bool OrbisMutex::Lock()
	{
		if ((this->RET = pthread_mutex_lock(&this->mutex)) == 0)
			return true;
		
		this->LASTFN = "pthread_mutex_lock";
		return false;
	}

	bool OrbisMutex::TryUnlock()
	{
		if ((this->RET = pthread_mutex_unlock(&this->mutex)) == 0)
		{
			if ((this->RET = pthread_mutex_lock(&this->mutex)) == 0)
				return true;

			this->LASTFN = "pthread_mutex_lock";
			return false;
		}
		
		this->LASTFN = "pthread_mutex_unlock";
		return false;

	}

	bool OrbisMutex::Unlock()
	{
		if ((this->RET = pthread_mutex_unlock(&this->mutex)) == 0)
			return true;

		this->LASTFN = "pthread_mutex_unlock";
		return false;
	}

	bool OrbisMutex::IsOk()
	{
		return (this->RET == 0 && this->LASTFN == nullptr);
	}

	void OrbisMutex::PrintLastError()
	{
		MessageHandler::KernelPrintOut("%s failed with 0x%lx", this->LASTFN, this->RET);
	}
}