#pragma once

#include "Rynex/Core/Base.h"
#include "Rynex/Core/Log.h"
#include <filesystem>

#define RY_CORE_NOT_IMPL() RY_CORE_FATAL("Function Not Implemted {} {}", std::filesystem::path(__FILE__).string(), __LINE__); RY_DEBUG_BREAK()


#ifndef RY_DIST
	
	
	#define RY_INTERALE_ASSERT_IMPL(type, check, msg, ...)  { if(!(check)) {RY##type##ERROR(msg, __VA_ARGS__); RY_DEBUG_BREAK(); } }
	
	#define RY_INTERALE_ASSERT_NO_MSG(type, check) RY_INTERALE_ASSERT_IMPL(type, check, "Assertion faild: {0} Faild at {1}:{2}", RY_STRINGIFY_MOAKRO(check), std::filesystem::path(__FILE__).string(), __LINE__)
	#define RY_INTERALE_ASSERT_WITH_MSG(type, check, msg) RY_INTERALE_ASSERT_IMPL(type, check, "Assertion Faild: {}", msg)
	#define RY_INTERALE_ASSERT_WITH_MSG_ONE_ARG(type, check, msg, msgArg1) RY_INTERALE_ASSERT_IMPL(type, check, "Assertion Faild: " msg, msgArg1)
	#define RY_INTERALE_ASSERT_WITH_MSG_MULTY_ARGS(type, check, msg, msgArg1, ...) RY_INTERALE_ASSERT_IMPL(type, check, "Assertion Faild: " msg,  msgArg1, __VA_ARGS__)
	
	#define RY_INTERALE_ASSERT_GET_MACRO_NAME(arg3, arg2, arg1, msgArg, check, marco, ...) marco
	#define RY_INTERALE_ASSERT_GET_MACRO(...) RY_EXPAND_MOAKRO(  RY_INTERALE_ASSERT_GET_MACRO_NAME(__VA_ARGS__, RY_INTERALE_ASSERT_WITH_MSG_MULTY_ARGS, RY_INTERALE_ASSERT_WITH_MSG_MULTY_ARGS, RY_INTERALE_ASSERT_WITH_MSG_ONE_ARG,  RY_INTERALE_ASSERT_WITH_MSG, RY_INTERALE_ASSERT_NO_MSG, RY_INTERALE_ASSERT_0)  )
	
	#define RY_ASSERT(...) RY_EXPAND_MOAKRO(  RY_INTERALE_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__)  )
	#define RY_CORE_ASSERT(...) RY_EXPAND_MOAKRO(  RY_INTERALE_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__)  )
	
#else


	#define RY_ASSERT(...)
	#define RY_CORE_ASSERT(...)
#endif