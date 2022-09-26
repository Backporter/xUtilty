#pragma once

#if defined(__ORBIS__)
#include <kernel.h>
#include <gnm.h>
#include <gnmx.h>
#include <video_out.h>
#include <gnf.h>
#elif defined(__OPENORBIS__)
#include <orbis/libkernel.h>
#endif

namespace FrameUtil
{
	class Tracker
	{
	public:
		Tracker();
		~Tracker();
		
		void  Update();
		
		float GetFrameTime();
		float GetFrameDelta();
		float GetFrameDeltaTime();
		float GetFrameRate();
		
		static Tracker* GetSingleton()
		{
			static Tracker singletion;
			return &singletion;
		};

	public:
		volatile float FrameTime;
		volatile float FrameDelta;
		volatile float FrameDeltaTime;
		volatile float FrameRate;
	};
}