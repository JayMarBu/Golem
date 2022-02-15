#include "golpch.h"
#include "Profiler.h"

namespace golem
{
	struct ProfilerInfo
	{
		bool isRecording = false;
		std::string filename = "";
	};

	static ProfilerInfo s_profilerInfo;

	void Profiler::Record(const std::string& filename)
	{
		if(s_profilerInfo.isRecording)
			return;

		GOL_CORE_TRACE("Profiler session has begun recording");

		s_profilerInfo.isRecording = true;
		GOL_PROFILE_BEGIN_SESSION("Recording", "Recording.json");
	}

	void Profiler::End()
	{
		if (!s_profilerInfo.isRecording)
			return;

		GOL_PROFILE_END_SESSION();
		s_profilerInfo.isRecording = false;

		GOL_CORE_TRACE("Profiler session has ended recording");
	}

	bool Profiler::IsRecording()
	{
		return s_profilerInfo.isRecording;
	}

}