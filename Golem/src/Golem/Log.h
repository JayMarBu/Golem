#pragma once

#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace golem
{
	class Log
	{
		// Members ********************************************************************************
	private:
		static std::shared_ptr<spdlog::logger> s_coreLogger;
		static std::shared_ptr<spdlog::logger> s_clientLogger;
		static std::shared_ptr<spdlog::logger> s_validationLayerLogger;

		// Methods ********************************************************************************
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_coreLogger;}
		inline static std::shared_ptr<spdlog::logger>& GetCLientLogger() { return s_clientLogger;}
		inline static std::shared_ptr<spdlog::logger>& GetValidationLayerLogger() { return s_validationLayerLogger;}
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

// Client Validation Layer Macros *****************************************************************
#define GOL_VL_ERROR(...)   ::golem::Log::GetValidationLayerLogger()->error(__VA_ARGS__)
#define GOL_VL_WARN(...)    ::golem::Log::GetValidationLayerLogger()->warn(__VA_ARGS__)
#define GOL_VL_INFO(...)    ::golem::Log::GetValidationLayerLogger()->info(__VA_ARGS__)
#define GOL_VL_TRACE(...)   ::golem::Log::GetValidationLayerLogger()->trace(__VA_ARGS__)
#define GOL_VL_FATAL(...)   ::golem::Log::GetValidationLayerLogger()->fatal(__VA_ARGS__)