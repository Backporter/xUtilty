#pragma once
#include "../SystemWrapper.h"

#if __ORBIS__
#include <kernel.h>
#elif defined(__OPENORBIS__)
#endif

#include <memory>

// wrap  
class IMemoryAllocator
{
public:
	using allocatorfunc_t = void*(size_t);
	using deallocatefunc_t = void(void*);
public:
	IMemoryAllocator()
	{
		g_allocatefunc = SystemWrapper::malloc;
		g_deallocateFunc = SystemWrapper::free;
	}

	~IMemoryAllocator() { }

	static void* operator new(size_t a_size)
	{
		return Allocate(a_size);
	}

	static void operator delete(void* a_ptr)
	{
		return Deallocate(a_ptr);
	}
public:
	static IMemoryAllocator& get() { static IMemoryAllocator g_this; return g_this; }
	static void* Allocate(size_t a_size) { return g_allocatefunc(a_size); }
	static void Deallocate(void* a_ptr) { return g_deallocateFunc(a_ptr); }
public:
	allocatorfunc_t* GetAllocatorFunc() { return g_allocatefunc; }
	deallocatefunc_t* GetDealocateFunc() { return g_deallocateFunc; }
public:
	static allocatorfunc_t* g_allocatefunc;
	static deallocatefunc_t* g_deallocateFunc;
};

inline IMemoryAllocator::allocatorfunc_t* IMemoryAllocator::g_allocatefunc = nullptr;
inline IMemoryAllocator::deallocatefunc_t* IMemoryAllocator::g_deallocateFunc = nullptr;