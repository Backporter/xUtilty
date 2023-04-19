#pragma once

#include <stdint.h>

template <typename T>
class BitFlags
{
public:
	using value_type = T;
	using const_type = const value_type;
	using volatile_type = volatile value_type;
public:
	BitFlags() = default;
	BitFlags(const_type a_data) : m_flags(a_data) { }
	BitFlags(const BitFlags& a_rhs) : m_flags(a_rhs.m_flags) { }
	~BitFlags() { }
public:
	BitFlags& operator=(value_type a_flag)
	{
		m_flags = a_flag;
		return *this;
	}

	BitFlags& operator=(const BitFlags& a_rhs) 
	{
		m_flags = a_rhs.m_flags;
		return *this;
	}

	bool operator==(const BitFlags& a_rhs)
	{
		return m_flags == a_rhs.m_flags;
	}

	bool operator!=(const BitFlags& a_rhs)
	{
		return m_flags != a_rhs.m_flags;
	}
public:
	const value_type GetFlags() 
	{
		return m_flags;
	}
	
	void SetFlags(uint32_t a_flags)
	{
		m_flags |= a_flags;
	}

	void ClearFlags(uint32_t a_flags)
	{
		m_flags &= ~(a_flags);
	}

	void ClearAllFlags()
	{
		m_flags = 0;
	}

	bool TestFlags(uint32_t a_flag)
	{
		return (m_flags & a_flag);
	}

	void SetBit(int32_t a_bit)
	{
		m_flags |= (1 << a_bit);
	}

	void ClearBit(int32_t a_bit)
	{
		m_flags &= (1 << a_bit);
	}

	void ClearAllBits()
	{
		m_flags = 0;
	}
	
	bool TestBits(int32_t a_bit)
	{
		return m_flags & (1 << a_bit);
	}
protected:
	volatile_type m_flags { 0 };
};