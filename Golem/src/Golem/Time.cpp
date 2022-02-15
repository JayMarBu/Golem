#include "golpch.h"
#include "Time.h"
#include "GLFW/glfw3.h"

namespace golem
{

	struct TimeData
	{
		float lastTime = 0;
		TimeStep timestep = 0;

		float fps = 0;
		float timecount = 0;
		int frames = 0;
	};

	static TimeData s_timeData;

	void Time::Frame()
	{
		float time = (float)glfwGetTime();
		
		s_timeData.timestep = time - s_timeData.lastTime;

		s_timeData.timecount += s_timeData.timestep;

		s_timeData.frames++;
		if(s_timeData.timecount >= 1.0f)
		{
			s_timeData.fps = (float)s_timeData.frames / s_timeData.timecount;

			s_timeData.timecount -= 1.0f;
			s_timeData.frames = 0;

		}

		s_timeData.lastTime = time;
	}

	golem::TimeStep Time::DeltaTime()
	{
		return s_timeData.timestep;
	}

	float Time::FPS()
	{
		return s_timeData.fps;
	}

}