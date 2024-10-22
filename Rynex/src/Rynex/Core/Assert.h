#pragma once

#include "Rynex/Core/Base.h"
#include "Rynex/Core/Log.h"
#include <filesystem>

// DebugBreakPoint on Error
#if RY_ENABLE_ASSERTS
	#define RY_NEW_ASSERT 1
	#if RY_NEW_ASSERT
		#define RY_INTERALE_ASSERT_IMPL(type, check, msg, ...)  { if(!(check)) {RY##type##ERROR(msg, __VA_ARGS__); RY_DEBUG_BREAK(); } }
		#define RY_INTERALE_ASSERT_WITH_MSG(type, check, ...) RY_INTERALE_ASSERT_IMPL(type, check, "Assertion faild: {0}",__VA_ARGS__)
		#define RY_INTERALE_ASSERT_NO_MSG(type, check) RY_INTERALE_ASSERT_IMPL(type, check, "Assertion faild: {0} faild at {1}:{2}", RY_STRINGIFY_MOAKRO(check), std::filesystem::path(__FILE__).string(), __LINE__)
		
		#define RY_INTERALE_ASSERT_GET_MACRO_NAME(arg1, arg2, marco, ...) marco
		#define RY_INTERALE_ASSERT_GET_MACRO(...) RY_EXPAND_MOAKRO(  RY_INTERALE_ASSERT_GET_MACRO_NAME(__VA_ARGS__, RY_INTERALE_ASSERT_NO_MSG,  RY_INTERALE_ASSERT_NO_MSG)  )
		
		#define RY_ASSERT(...) RY_EXPAND_MOAKRO(  RY_INTERALE_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__)  )
		#define RY_CORE_ASSERT(...) RY_EXPAND_MOAKRO(  RY_INTERALE_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__)  )
	#else
		#define RY_ASSERT(x, ...) { if(!(x)) { RY_ERROR("Assertion Faild: {0}", __VA_ARGS__); __debugbreak(); } }
		#define RY_CORE_ASSERT(x, ...) { if(!(x)) { RY_CORE_ERROR("Assertion Faild: {0}", __VA_ARGS__); __debugbreak(); } }
	#endif

#else	
	#define RY_ASSERT(...) 
	#define RY_CORE_ASSERT(...) 
#endif