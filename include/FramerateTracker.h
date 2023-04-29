#pragma once

namespace xUtilty
{
	namespace FrameUtil
	{
		class Tracker
		{
		public:
			Tracker() = default;
			~Tracker() = default;

			//
			void Update();

			// Getters
			inline float GetFrameTime() { return FrameTime; }
			inline float GetFrameDelta() { return FrameDelta; }
			inline float GetFrameDeltaTime() { return FrameDeltaTime; }
			inline float GetFrameRate() { return FrameRate; }

			static Tracker* GetSingleton()
			{
				static Tracker singletion;
				return &singletion;
			}
		public:
			volatile float FrameTime{ 0 };
			volatile float FrameDelta{ 0 };
			volatile float FrameDeltaTime{ 0 };
			volatile float FrameRate{ 0 };
		};
	}
}