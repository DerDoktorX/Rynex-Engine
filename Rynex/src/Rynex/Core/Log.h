#pragma once

#include "Base.h"

#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Rynex {

	class RYNEX_API Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};

}


#define RY_CORE_TRACE(...)	::Rynex::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define	RY_CORE_INFO(...)	::Rynex::Log::GetCoreLogger()->info(__VA_ARGS__)
#define	RY_CORE_WARN(...)	::Rynex::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define	RY_CORE_ERROR(...)	::Rynex::Log::GetCoreLogger()->error(__VA_ARGS__)
#define	RY_CORE_FATAL(...)	::Rynex::Log::GetCoreLogger()->fatal(__VA_ARGS__)

#define	RY_TRACE(...)		::Rynex::Log::GetClientLogger()->trace(__VA_ARGS__)
#define	RY_INFO(...)		::Rynex::Log::GetClientLogger()->info(__VA_ARGS__)
#define	RY_WARN(...)		::Rynex::Log::GetClientLogger()->warn(__VA_ARGS__)
#define	RY_ERROR(...)		::Rynex::Log::GetClientLogger()->error(__VA_ARGS__)
#define	RY_FATAL(...)		::Rynex::Log::GetClientLogger()->fatal(__VA_ARGS__)

