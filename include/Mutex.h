#pragma once

#include <pthread.h>

#if defined(__ORBIS__)
#include <mutex>
#elif defined(__OPENORBIS__)
#include <orbis/libkernel.h>
#endif

class Mutex
{
public:
	enum Type
	{
		kLoadGame,
		kUserInterfaceTasklet,
		kTaskPoolTasklet,
		kMax,
	};

	// using pthread_mutex_t = pthread_mutex_t;
	// using pthread_mutexattr_t = pthread_mutexattr_t;
public:
	Mutex(const char* name = NULL);
	~Mutex();

	//
	bool TryLock();
	bool TryUnlock();
	bool Lock();
	bool Unlock();

	static Mutex& GetSingleton(Type a_mutexType)
	{
		static Mutex singleton[Type::kMax];
		return singleton[a_mutexType];
	}

protected:
	pthread_mutex_t	    m_mutex;
	pthread_mutexattr_t m_mutexAttr;
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

// game
extern Mutex GlobalMutex5;

// UI
extern Mutex GlobalMutex10;

// Tasklet
extern Mutex GlobalMutex11;