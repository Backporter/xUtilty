#pragma once
#include <stdint.h>

class BitFlags
{
public:
	BitFlags(int64_t a_flags) : flags(a_flags) { }
	BitFlags() : flags(-1) { }
	~BitFlags() { }
public:
	void operator=(int64_t a_flags) { flags = a_flags; }
public:
	bool											NotT(int a)		{ return (flags & a) == 0; }
	bool											IsT(int a)		{ return (flags & a) == a; }
	void											SetT(int a)		{ flags |= a;			   }
	void											RemoveT(int a)	{ flags &= ~a;			   }
	int64_t											GetFlags()		{ return flags;			   }
public:
	volatile int64_t flags;
};