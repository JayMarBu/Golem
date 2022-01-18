#include "golpch.h"
#include "FrameTimer.h"

namespace golem
{
	// Initialise timer. Check for high performance timers.
	FrameTimer::FrameTimer()
	{
		QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
		if (frequency == 0)
		{
			MessageBox(NULL, L"No support for high performance timer", L"ERROR", MB_OK);
		}

		// Find out how many times the frequency counter ticks every second.
		ticksPerS = (float)(frequency);

		QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

		elapsedTime = 0.f;
		frames = 0.f;
		fps = 0.f;
	}


	FrameTimer::~FrameTimer()
	{
	}

	// Once per frame calculate delta timer and update FPS calculation.
	void FrameTimer::frame()
	{
		INT64 currentTime;
		float timeDifference;

		// Query the current time.
		QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
		timeDifference = (float)(currentTime - startTime);
		frameTime = timeDifference / ticksPerS;

		// Calc FPS
		frames += 1.f;
		elapsedTime += frameTime;
		if (elapsedTime > 1.0f)
		{
			fps = frames / elapsedTime;
			frames = 0.0f;
			elapsedTime = 0.0f;
		}

		// Restart the timer.
		startTime = currentTime;

		return;
	}


	float FrameTimer::getTime()
	{
		return frameTime;
	}

	float FrameTimer::getFPS()
	{
		return fps;
	}
}