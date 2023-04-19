#include "../include/FramerateTracker.h"

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
	void FrameUtil::Tracker::Update()
	{
#if defined (__ORBIS__) || defined(__OPENORBIS__)
		float time     = (float)((sceKernelGetProcessTimeCounter() * 1000.0f) / sceKernelGetProcessTimeCounterFrequency());
#else
		float time     = 0;
#endif
		FrameDeltaTime = (float)(time - FrameDelta);
		FrameTime      = (FrameDeltaTime / 1000.0f);
		FrameRate      = 1.0f / FrameTime;
		FrameDelta     = time;
	}
}