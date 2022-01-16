#pragma once

#include "Core.h"
#include "spdlog/spdlog.h"
#include <memory>

namespace golem
{
	class GOLEM_API Log
	{
		// Members ********************************************************************************
	private:
		static std::shared_ptr<spdlog::logger> s_coreLogger;
		static std::shared_ptr<spdlog::logger> s_clientLogger;

		// Methods ********************************************************************************
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_coreLogger;}
		inline static std::shared_ptr<spdlog::logger>& GetCLientLogger() { return s_clientLogger;}
	};
}

// Core Log Macros ********************************************************************************
#define GOL_CORE_ERROR(...) ::golem::Log::GetCoreLogger()->error(__VA_ARGS__)
#define GOL_CORE_WARN(...)  ::golem::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define GOL_CORE_INFO(...)  ::golem::Log::GetCoreLogger()->info(__VA_ARGS__)
#define GOL_CORE_TRACE(...) ::golem::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define GOL_CORE_FATAL(...) ::golem::Log::GetCoreLogger()->fatal(__VA_ARGS__)

// Client Log Macros ******************************************************************************
#define GOL_ERROR(...)      ::golem::Log::GetCLientLogger()->error(__VA_ARGS__)
#define GOL_WARN(...)       ::golem::Log::GetCLientLogger()->warn(__VA_ARGS__)
#define GOL_INFO(...)       ::golem::Log::GetCLientLogger()->info(__VA_ARGS__)
#define GOL_TRACE(...)      ::golem::Log::GetCLientLogger()->trace(__VA_ARGS__)
#define GOL_FATAL(...)      ::golem::Log::GetCLientLogger()->fatal(__VA_ARGS__)