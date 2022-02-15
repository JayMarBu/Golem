#include "golpch.h"
#include "Time.h"
#include "GLFW/glfw3.h"

namespace golem
{

	struct TimeData
	{
		float lastTime = 0;
		TimeStep timestep = 0;
	};

	static TimeData s_timeData;

	void Time::Frame()
	{
		float time = (float)glfwGetTime();
		
		s_timeData.timestep = time - s_timeData.lastTime;

		s_timeData.lastTime = time;
	}

	golem::TimeStep Time::DeltaTime()
	{
		return s_timeData.timestep;
	}

}