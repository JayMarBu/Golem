#pragma once

namespace golem
{
	class Profiler
	{
		// Methods ********************************************************************************
	public:
		static void Record(const std::string& filename = "");
		static void End();

		static bool IsRecording();
	};
}