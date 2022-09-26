#include "../include/FramerateTracker.h"

namespace FrameUtil
{
	FrameUtil::Tracker::Tracker() :
		FrameTime(0), FrameDelta(0), FrameDeltaTime(0), FrameRate(0)
	{

	}

	FrameUtil::Tracker::~Tracker(){}

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

	float FrameUtil::Tracker::GetFrameTime()
	{
		return this->FrameTime;
	}

	float FrameUtil::Tracker::GetFrameDelta()
	{
		return this->FrameDelta;
	}

	float FrameUtil::Tracker::GetFrameDeltaTime()
	{
		return this->FrameDeltaTime;
	}

	float FrameUtil::Tracker::GetFrameRate()
	{
		return this->FrameRate;
	}
}