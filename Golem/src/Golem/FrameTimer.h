#pragma once

namespace golem
{
	class FrameTimer
	{
		// Members ********************************************************************************
	private:
		int64_t frequency;
		float ticksPerS;
		int64_t startTime;
		float frameTime;
		float fps;
		float frames;
		float elapsedTime;

		// Methods ********************************************************************************
	public:
		FrameTimer();
		~FrameTimer();

		void frame();	///< Update delta time, called every frame

		float getTime();	///< Get delta time
		float getFPS();		///< Get FPS (for display)

	};
}