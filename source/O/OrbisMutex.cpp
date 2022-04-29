#include "../O/OrbisMutex.h"
#include "../O/OrbisMessageHandler.h"

namespace OrbisMutex
{
	OrbisMutex GlobalMutex;

	OrbisMutex::OrbisMutex(const char* name)
		:LASTFN(nullptr), RET(0)
	{
		if ((this->RET = scePthreadMutexattrInit(&this->mutexattr)) != 0)
		{
			this->LASTFN = "scePthreadMutexattrInit";
			this->PrintLastError();
			return;
		}

		if ((this->RET = scePthreadMutexInit(&this->mutex, &this->mutexattr, name)) != 0)
		{
			this->LASTFN = "scePthreadMutexInit";
			this->PrintLastError();
			return;
		}
	}

	OrbisMutex::~OrbisMutex()
	{
		if ((this->RET = scePthreadMutexattrDestroy(&this->mutexattr)) != 0)
		{
			this->LASTFN = "scePthreadMutexattrDestroy";
			this->PrintLastError();
			return;
		}

		if ((this->RET = scePthreadMutexDestroy(&this->mutex)) != 0)
		{
			this->LASTFN = "scePthreadMutexDestroy";
			this->PrintLastError();
			return;
		}

	}

	bool OrbisMutex::TryLock()
	{
		if ((this->RET = scePthreadMutexTrylock(&this->mutex)) == 0)
			return true;

		this->LASTFN = "scePthreadMutexTrylock";
		return false;
	}

	bool OrbisMutex::Lock()
	{
		if ((this->RET = scePthreadMutexLock(&this->mutex)) == 0)
			return true;

		this->LASTFN = "scePthreadMutexLock";
		return false;
	}

	bool OrbisMutex::TryUnlock()
	{
		if ((this->RET = scePthreadMutexUnlock(&this->mutex)) == 0)
		{
			if ((this->RET = scePthreadMutexLock(&this->mutex)) == 0)
				return true;

			this->LASTFN = "scePthreadMutexLock";
			return false;
		}

		this->LASTFN = "scePthreadMutexUnlock";
		return false;
	}

	bool OrbisMutex::Unlock()
	{
		if ((this->RET = scePthreadMutexUnlock(&this->mutex)) == 0)
			return true;

		this->LASTFN = "scePthreadMutexUnlock";
		return false;
	}

	bool OrbisMutex::IsOk()
	{
		return (this->RET == 0 && this->LASTFN == nullptr);
	}

	void OrbisMutex::PrintLastError()
	{
		OrbisMessageHandler::KernelPrintOut("%s failed with 0x%lx", this->LASTFN, this->RET);
	}
}