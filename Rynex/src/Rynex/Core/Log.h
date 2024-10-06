#pragma once


#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"
// Arrows dont work her
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

#if RY_CONSOLE
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
#else
#define RY_CORE_TRACE(...)	
#define	RY_CORE_INFO(...)	
#define	RY_CORE_WARN(...)	
#define	RY_CORE_ERROR(...)	
#define	RY_CORE_FATAL(...)	

#define	RY_TRACE(...)		
#define	RY_INFO(...)		
#define	RY_WARN(...)		
#define	RY_ERROR(...)		
#define	RY_FATAL(...)		
#endif

#if RY_CONSOLE_ALICATION_OFF_MEMORY
#define	RY_CORE_MEMORY_ALICATION(name, in, size)		::Rynex::Log::GetCoreLogger()->error("{0}, in class {1}: alication {2} bytes", name, in, sizeof(size))
#define	RY_CORE_MULTY_MEMORY_ALICATION(name, in, size)	::Rynex::Log::GetCoreLogger()->error("{0}, in class {1}: multy alication {2} bytes", name, in, size)

#define	RY_CORE_MEMORY_FREE(name, in)				::Rynex::Log::GetCoreLogger()->info("{0}, in class {1}: free", name, in)
#define	RY_CORE_MULTY_FREE_ALICATION(name, in)		::Rynex::Log::GetCoreLogger()->info("{0}, in class {1}: multy free", name, in)
#else
#define	RY_CORE_MEMORY_ALICATION(name, in, size)
#define	RY_CORE_MULTY_MEMORY_ALICATION(name, in, size)
#define	RY_CORE_MEMORY_FREE(name, in)
#define	RY_CORE_MULTY_FREE_ALICATION(name, in)
#endif