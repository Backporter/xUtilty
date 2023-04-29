#pragma once

#include <stdint.h>

namespace xUtilty
{
	class OffsetManger
	{
	public:
		OffsetManger();
		~OffsetManger();

		static OffsetManger* GetSingleton()
		{
			static OffsetManger OffsetManger;
			return &OffsetManger;
		}

		//
		bool Initialize();
	public:
	};
}