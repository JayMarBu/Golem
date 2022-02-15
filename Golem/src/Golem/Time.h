#pragma once

namespace golem
{
	class TimeStep;

	class Time
	{
		// Methods ********************************************************************************
	public:
		static void Frame();

		static TimeStep DeltaTime();
		static float FPS();
	};

	class TimeStep
	{
		// Members ********************************************************************************
	private:
		float m_time;

		// Methods ********************************************************************************
	public:
		TimeStep(float time = 0.0f)
			: m_time{time}
		{

		}

		float GetSeconds() const {return m_time;}
		float GetMilliseconds() const {return m_time * 1000.0f;}

		inline operator float() const {return GetSeconds();}

	};
}