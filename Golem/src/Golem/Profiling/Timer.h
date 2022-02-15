#pragma once
#include <chrono>

namespace golem
{
	template <typename Fn>
	class Timer
	{
		// Members ********************************************************************************
	private:
		const char* m_name;
		std::chrono::time_point<std::chrono::steady_clock> m_startTimepoint;
		Fn m_func;
		bool m_stopped;

		// Methods ********************************************************************************
	public:
		Timer(const char* name, Fn&& func)
			: m_name{name}, m_func{func}, m_stopped{false}
		{
			m_startTimepoint = std::chrono::high_resolution_clock::now();
		}

		~Timer()
		{
			if(!m_stopped)
				Stop()
		}

		void Stop()
		{
			auto endTimepoint = std::chrono::high_resolution_clock::now();

			long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_startTimepoint).time_since_epoch().count();
			long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

			m_stopped = true;

			float duration = (end - start) * 0.001f;
			m_func(m_name, duration);
		}

	};
}