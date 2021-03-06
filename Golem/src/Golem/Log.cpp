#include "golpch.h"
#include "Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace golem
{
	std::shared_ptr<spdlog::logger> Log::s_coreLogger;
	std::shared_ptr<spdlog::logger> Log::s_clientLogger;
	std::shared_ptr<spdlog::logger> Log::s_validationLayerLogger;

	void Log::Init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");

		s_coreLogger = spdlog::stdout_color_mt("GOLEM");
		s_coreLogger->set_level(spdlog::level::trace);

		s_clientLogger = spdlog::stdout_color_mt("APP");
		s_clientLogger->set_level(spdlog::level::trace);

		s_validationLayerLogger = spdlog::stdout_color_mt("VALIDATION LAYER");
		s_validationLayerLogger->set_level(spdlog::level::trace);
	}

}