#pragma once

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