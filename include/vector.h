#pragma once
#include <memory>
#include <assert.h>

namespace stl
{
	// template <typename T>
	// class vector
	// {
	// public:
	// 	vector()
	// 	{
	// 	}
	// 
	// 	~vector()
	// 	{
	// 		if (m_data)
	// 			free((void*)m_data);
	// 	}
	// 
	// 	T* data()
	// 	{
	// 		return m_data;
	// 	}
	// 
	// 	void resize(int i)
	// 	{
	// 		if (m_data)
	// 			free((void*)m_data);
	// 
	// 		m_data = (T*)malloc(sizeof(T) * i);
	// 		assert(m_data != nullptr);
	// 		m_size = i;
	// 		m_capacity = i;
	// 
	// 		for (int j = 0; j < i; j++)
	// 		{
	// 			m_data ();
	// 		}
	// 	}
	// 
	// 	size_t size()
	// 	{
	// 		return m_size;
	// 	}
	// 
	// 	size_t capacity()
	// 	{
	// 		return m_capacity;
	// 	}
	// 
	// 	T& operator[](int i)
	// 	{
	// 		return m_data[i];
	// 	}
	// private:
	// 	T*			m_data;
	// 	uint64_t	m_size;
	// 	uint64_t	m_capacity;
	// };
}