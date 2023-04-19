#pragma once

#include <stdint.h>
#include <vector>
#include <map>

class OffsetManger
{
public:
	struct Entry
	{
	public:
		uint64_t  m_id{ 0 };
		uintptr_t m_offset{ 0 };
	};

	struct Header
	{
	public:
		uint32_t m_magic { 0 };
		uint32_t m_version{ 0 };
		uint32_t m_pointerSize{ 0 };
		uint32_t m_entryCount{ 0 };
	};
public:
	OffsetManger();
	~OffsetManger();

	bool Initialize();
public:
	static OffsetManger* GetSingleton()
	{
		static OffsetManger OffsetManger;
		return &OffsetManger;
	}
public:
	bool						  m_Initialized{ false };
	Header						  m_Header;
	std::map<uint32_t, uintptr_t> m_Entires;
};