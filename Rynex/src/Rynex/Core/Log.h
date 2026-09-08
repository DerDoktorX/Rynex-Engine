#pragma once

#include "Rynex/Core/Base.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/fmt/std.h>
#pragma warning(pop)

// Arrows dont work her
namespace Rynex {

	class RYNEX_API Log
	{
	public:
		Log();
		~Log();
		void Init();
		void Shutdown();
		static Log& Get();

		inline std::shared_ptr<spdlog::logger>& GetCoreLogger() { return m_CoreLogger; }
		inline std::shared_ptr<spdlog::logger>& GetClientLogger() { return m_ClientLogger; }

		inline std::shared_ptr<spdlog::logger>& GetCoreGraficsCreateLogger() { return m_CoreGraficsCreateLogger; }
		inline std::shared_ptr<spdlog::logger>& GetCoreGraficsDeleteLogger() { return m_CoreGraficsDeleteLogger; }
		inline std::shared_ptr<spdlog::logger>& GetCoreAssetLogger() { return m_CoreAssetLogger; }
		inline std::shared_ptr<spdlog::logger>& GetCoreMemoryLogger() { return m_CoreMemoryLogger; }

		inline std::shared_ptr<spdlog::logger>& GetThreadLogger() { return m_ThreadLogger; }

		void EnableLockNumberLoging();
		void DisableLockNumberLoging();
	// --- public static constexpr function -----------------------------------------------------------------------------------
		template <typename... Args>
		inline static constexpr void LoggerMessage(std::shared_ptr<spdlog::logger>& logger, spdlog::format_string_t<Args...> fmt, spdlog::level::level_enum levelType, Args && ... args)
		{
			logger->log(levelType, fmt, std::forward<Args>(args)...);
		}

		template<typename T>
		inline static constexpr void LoggerMessage(std::shared_ptr<spdlog::logger>& logger, const T& msg, spdlog::level::level_enum levelType)
		{
			logger->log(levelType, msg);
		}

		

	private:
		static Log s_LogInstance;
	// --- private member varibels --------------------------------------------------------------------------------------------

		std::shared_ptr<spdlog::logger> m_CoreLogger;
		std::shared_ptr<spdlog::logger> m_CoreGraficsCreateLogger;
		std::shared_ptr<spdlog::logger> m_CoreGraficsDeleteLogger;
		std::shared_ptr<spdlog::logger> m_CoreAssetLogger;
		std::shared_ptr<spdlog::logger> m_CoreMemoryLogger;
		std::shared_ptr<spdlog::logger> m_ClientLogger;
		std::shared_ptr<spdlog::logger> m_ThreadLogger;
		std::atomic<uint32_t> m_NummberFormating;
	};
	
	

}
template<typename OStream, glm::length_t L, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::vec<L, T, Q>& vector)
{
	return os << glm::to_string(vector);
}

template<typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::mat<C, R, T, Q>& matrix)
{
	return os << glm::to_string(matrix);
}

template<typename OStream, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, glm::qua<T, Q> quaternion)
{
	return os << glm::to_string(quaternion);
}




#if RY_CONSOLE_LOG_NUMBER
	#define RY_LOG_ENABLE_NUMBER	Log::EnableLockNumberLoging()
	#define RY_LOG_DISABLE_NUMBER	Log::DisableLockNumberLoging()
#else
	#define RY_LOG_ENABLE_NUMBER
	#define RY_LOG_DISABLE_NUMBER
#endif

#if RY_CONSOLE



	#define RY_INTERNEL_LOG_FUNC_PLACEHOLDER_0 ""
	#define RY_INTERNEL_LOG_FUNC_PLACEHOLDER_1 "{}"
	#define RY_INTERNEL_LOG_FUNC_PLACEHOLDER_2 RY_INTERNEL_LOG_FUNC_PLACEHOLDER_1 ", {}"
	#define RY_INTERNEL_LOG_FUNC_PLACEHOLDER_3 RY_INTERNEL_LOG_FUNC_PLACEHOLDER_2 ", {}"
	#define RY_INTERNEL_LOG_FUNC_PLACEHOLDER_4 RY_INTERNEL_LOG_FUNC_PLACEHOLDER_3 ", {}"
	#define RY_INTERNEL_LOG_FUNC_PLACEHOLDER_5 RY_INTERNEL_LOG_FUNC_PLACEHOLDER_4 ", {}"
	#define RY_INTERNEL_LOG_FUNC_PLACEHOLDER_6 RY_INTERNEL_LOG_FUNC_PLACEHOLDER_5 ", {}"
	#define RY_INTERNEL_LOG_FUNC_PLACEHOLDER_7 RY_INTERNEL_LOG_FUNC_PLACEHOLDER_6 ", {}"
	#define RY_INTERNEL_LOG_FUNC_PLACEHOLDER_8 RY_INTERNEL_LOG_FUNC_PLACEHOLDER_7 ", {}"
	#define RY_INTERNEL_LOG_FUNC_PLACEHOLDER_9 RY_INTERNEL_LOG_FUNC_PLACEHOLDER_8 ", {}"
	#define RY_INTERNEL_LOG_FUNC_PLACEHOLDER_10 RY_INTERNEL_LOG_FUNC_PLACEHOLDER_9 ", {}"
	#define RY_INTERNEL_LOG_FUNC_PLACEHOLDER_11 RY_INTERNEL_LOG_FUNC_PLACEHOLDER_10 ", {}"
	#define RY_INTERNEL_LOG_FUNC_PLACEHOLDER_12 RY_INTERNEL_LOG_FUNC_PLACEHOLDER_11 ", {}"
	#define RY_INTERNEL_LOG_FUNC_PLACEHOLDER_13 RY_INTERNEL_LOG_FUNC_PLACEHOLDER_12 ", {}"
	#define RY_INTERNEL_LOG_FUNC_PLACEHOLDER_14 RY_INTERNEL_LOG_FUNC_PLACEHOLDER_13 ", {}"
	#define RY_INTERNEL_LOG_FUNC_PLACEHOLDER_15 RY_INTERNEL_LOG_FUNC_PLACEHOLDER_14 ", {}"
	#define RY_INTERNEL_LOG_FUNC_PLACEHOLDER_16 RY_INTERNEL_LOG_FUNC_PLACEHOLDER_15 ", {}"
	#define RY_INTERNEL_LOG_FUNC_PLACEHOLDER_17 RY_INTERNEL_LOG_FUNC_PLACEHOLDER_16 ", {}"

	
#ifndef RY_DIST
	#define	RY_LOG_MSG_IF_NOT(check, macroFunc, ...)\
		if(!(check))\
		{\
			macroFunc(__VA_ARGS__);\
		}

	#define	RY_LOG_MSG_IF(check, macroFunc, ...)\
		if(check)\
		{\
			macroFunc(__VA_ARGS__);\
		}
#else
	#define	RY_LOG_MSG_IF_NOT(check, macroFunc, ...)
	#define	RY_LOG_MSG_IF(check, macroFunc, ...)
#endif

		

	#define RY_CORE_TRACE(...)		::Rynex::Log::Get().GetCoreLogger()->trace(__VA_ARGS__)
	#define	RY_CORE_INFO(...)		::Rynex::Log::Get().GetCoreLogger()->info(__VA_ARGS__)

	#define	RY_CORE_WARN(...)		::Rynex::Log::Get().GetCoreLogger()->warn(__VA_ARGS__)
	#define	RY_CORE_ERROR(...)		::Rynex::Log::Get().GetCoreLogger()->error(__VA_ARGS__)
	#define	RY_CORE_FATAL(...)		::Rynex::Log::Get().GetCoreLogger()->critical(__VA_ARGS__)


	#define	RY_CORE_TRACE_IF(check, ...)	RY_LOG_MSG_IF(check, RY_CORE_TRACE, __VA_ARGS__)
	#define	RY_CORE_INFO_IF(check, ...)		RY_LOG_MSG_IF(check, RY_CORE_INFO, __VA_ARGS__)
	#define	RY_CORE_WARN_IF(check, ...)		RY_LOG_MSG_IF(check, RY_CORE_WARN, __VA_ARGS__)
	#define	RY_CORE_ERROR_IF(check, ...)	RY_LOG_MSG_IF(check, RY_CORE_ERROR, __VA_ARGS__)
	#define	RY_CORE_FATAL_IF(check, ...)	RY_LOG_MSG_IF(check, RY_CORE_FATAL, __VA_ARGS__)

	


#define RY_CORE_FUNC_LOG(func, ...)  \
	RY_CORE_TRACE(#func "("  RY_ACTOIN_PER_ARG(RY_INTERNEL_LOG_FUNC_PLACEHOLDER_, __VA_ARGS__) ");", __VA_ARGS__);

#define RY_CORE_FUNC_ERORO_LOG(func, ...)  \
	RY_ASSET_TRACE(#func "("  RY_ACTOIN_PER_ARG(RY_INTERNEL_LOG_FUNC_PLACEHOLDER_, __VA_ARGS__) ");", __VA_ARGS__);


	#define	RY_GRAFIC_DELETE(id, name)	::Rynex::Log::Get().GetCoreGraficsDeleteLogger()->trace("( ID: {}, "#name" ) Line:({})", id, __LINE__)
	#define	RY_GRAFIC_CREATE(id, name)	::Rynex::Log::Get().GetCoreGraficsCreateLogger()->trace("( ID: {}, "#name" ) Line:({})", id, __LINE__)

	#define	RY_THREAD_TASK_SUBMIT(threadID, disc) ::Rynex::Log::Get().GetThreadLogger()->info("Thread Task submit ID: {}, discrption {}", threadID, disc);
	#define	RY_THREAD_TASK_FINSHED(threadID, disc) ::Rynex::Log::Get().GetThreadLogger()->info("Thread Task finshed ID: {}, discrption {}", threadID, disc);


	#define RY_ASSET_TRACE(...)		::Rynex::Log::Get().GetCoreAssetLogger()->trace(__VA_ARGS__)	
	#define	RY_ASSET_INFO(...)		::Rynex::Log::Get().GetCoreAssetLogger()->info(__VA_ARGS__)
	#define	RY_ASSET_WARN(...)		::Rynex::Log::Get().GetCoreAssetLogger()->warn(__VA_ARGS__)
	#define	RY_ASSET_ERROR(...)		::Rynex::Log::Get().GetCoreAssetLogger()->error(__VA_ARGS__)
	#define	RY_ASSET_FATAL(...)		::Rynex::Log::Get().GetCoreAssetLogger()->critical(__VA_ARGS__)	


	#define	RY_TRACE(...)			::Rynex::Log::Get().GetClientLogger()->trace(__VA_ARGS__)
	#define	RY_INFO(...)			::Rynex::Log::Get().GetClientLogger()->info(__VA_ARGS__)
	#define	RY_WARN(...)			::Rynex::Log::Get().GetClientLogger()->warn(__VA_ARGS__)
	#define	RY_ERROR(...)			::Rynex::Log::Get().GetClientLogger()->error(__VA_ARGS__)
	#define	RY_FATAL(...)			::Rynex::Log::Get().GetClientLogger()->critical(__VA_ARGS__)


	#define	RY_TRACE_IF(check, ...)		RY_LOG_MSG_IF_NOT(check, RY_TRACE, __VA_ARGS__)
	#define	RY_INFO_IF(check, ...)		RY_LOG_MSG_IF_NOT(check, RY_INFO, __VA_ARGS__)
	#define	RY_WARN_IF(check, ...)		RY_LOG_MSG_IF_NOT(check, RY_WARN, __VA_ARGS__)
	#define	RY_ERROR_IF(check, ...)		RY_LOG_MSG_IF_NOT(check, RY_ERROR, __VA_ARGS__)
	#define	RY_FATAL_IF(check, ...)		RY_LOG_MSG_IF_NOT(check, RY_FATAL, __VA_ARGS__)
#else

	#define RY_CORE_FUNC(...)
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

#if !RY_CONSOLE_FUNC_FETURE_REMBER || !RY_CONSOLE

#define RY_REMBER_FUNC_CHANGE(...)

#elif RY_CONSOLE_FUNC_FETURE_REMBER_PRINT

#define RY_INTERNAL_EMPTY_REMBER_FUNC_CHANGE()			RY_CORE_FATAL(msg "\n Rember some Chnage in func {0} in File {1}:{2}", RY_STRINGIFY_MOAKRO(__FUNCSIG__), std::filesystem::path(__FILE__).string(), __LINE__)
#define RY_INTERNAL_MSG_REMBER_FUNC_CHANGE(msg)	RY_CORE_FATAL(msg "\n Rember some Chnage in func {0} in File {1}:{2}", RY_STRINGIFY_MOAKRO(__FUNCSIG__), std::filesystem::path(__FILE__).string(), __LINE__)
#define RY_INTERNAL_MSG_REMBER_FUNC_CHANGE_ARGS(msg, ...)	RY_CORE_FATAL(msg "\n Rember some Chnage in func {0} in File {1}:{2}", RY_STRINGIFY_MOAKRO(__FUNCSIG__), std::filesystem::path(__FILE__).string(), __LINE__, __VA_ARGS__)


#define RY_INTERALE_REMBER_FUNC_CHANGE_GET_MACRO_NAME(msg_arg3, msg_arg2, msg_arg1, msg, marco, ...)	marco
#define RY_INTERALE_REMBER_FUNC_CHANG_GET_MACRO(...)	RY_EXPAND_MOAKRO(  RY_INTERALE_REMBER_FUNC_CHANGE_GET_MACRO_NAME( __VA_ARGS__, RY_INTERNAL_MSG_REMBER_FUNC_CHANGE_ARGS, RY_INTERNAL_MSG_REMBER_FUNC_CHANGE_ARGS, RY_INTERNAL_MSG_REMBER_FUNC_CHANGE_ARGS, RY_INTERNAL_MSG_REMBER_FUNC_CHANGE, RY_INTERNAL_EMPTY_REMBER_FUNC_CHANGE, RY_INTERALE_ASSERT_0  ) )


#define RY_REMBER_FUNC_CHANGE(...) RY_EXPAND_MOAKRO( RY_INTERALE_REMBER_FUNC_CHANG_GET_MACRO(__VA_ARGS__)(__VA_ARGS__) )


#else

#define RY_INTERNAL_MSG_REMBER_FUNC_CHANGE() static_assert(false, "Remember to implement or change this function")
#define RY_INTERNAL_EMPTY_REMBER_FUNC_CHANGE(msg, ...)  static_assert(false, msg)


#define RY_INTERALE_REMBER_FUNC_CHANGE_GET_MACRO_NAME(no_msg, msg, msg_arg1, msg_arg2, msg_arg3, marco, ...) marco
#define RY_INTERALE_REMBER_FUNC_CHANG_GET_MACRO(...) \
	RY_EXPAND_MOAKRO( \
		 RY_INTERALE_REMBER_FUNC_CHANGE_GET_MACRO_NAME(\
			__VA_ARGS__, \
			RY_INTERNAL_MSG_REMBER_FUNC_CHANGE,  RY_INTERNAL_MSG_REMBER_FUNC_CHANGE, \
			RY_INTERNAL_MSG_REMBER_FUNC_CHANGE, RY_INTERNAL_MSG_REMBER_FUNC_CHANGE, RY_INTERNAL_MSG_REMBER_FUNC_CHANGE,\
			RY_INTERNAL_EMPTY_REMBER_FUNC_CHANGE \
		) \
	)


#define RY_REMBER_FUNC_CHANGE(...) RY_EXPAND_MOAKRO( RY_INTERALE_REMBER_FUNC_CHANG_GET_MACRO(__VA_ARGS__)(__VA_ARGS__) )


#undef RY_INTERNAL_MSG_REMBER_FUNC_CHANGE
#undef RY_INTERNAL_EMPTY_REMBER_FUNC_CHANGE
#undef RY_INTERALE_REMBER_FUNC_CHANGE_GET_MACRO_NAME
#undef RY_INTERALE_REMBER_FUNC_CHANG_GET_MACRO
#endif
