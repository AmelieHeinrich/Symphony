#pragma once

#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

#include <memory>
#include <core/DLLExport.h>

namespace symphony 
{
	SymphonyClass Log
	{
	public:
		static void Init();

		static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};

}

// Core log macros
#define SY_CORE_TRACE(...)    ::symphony::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define SY_CORE_INFO(...)     ::symphony::Log::GetCoreLogger()->info(__VA_ARGS__)
#define SY_CORE_WARN(...)     ::symphony::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define SY_CORE_ERROR(...)    ::symphony::Log::GetCoreLogger()->error(__VA_ARGS__)
#define SY_CORE_CRITICAL(...) ::symphony::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define SY_TRACE(...)         ::symphony::Log::GetClientLogger()->trace(__VA_ARGS__)
#define SY_INFO(...)          ::symphony::Log::GetClientLogger()->info(__VA_ARGS__)
#define SY_WARN(...)          ::symphony::Log::GetClientLogger()->warn(__VA_ARGS__)
#define SY_ERROR(...)         ::symphony::Log::GetClientLogger()->error(__VA_ARGS__)
#define SY_CRITICAL(...)      ::symphony::Log::GetClientLogger()->critical(__VA_ARGS__)