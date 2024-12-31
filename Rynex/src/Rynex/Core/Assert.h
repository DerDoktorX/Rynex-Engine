#pragma once

#include "Rynex/Core/Base.h"
#include "Rynex/Core/Log.h"
#include <filesystem>

// DebugBreakPoint on Error	
#if 0
namespace Rynex {

	struct RepitingText
	{
		std::string Text;
		uint64_t Counter = 0;
		uint32_t NotFoundeFrame = 0;

		RepitingText(std::string text)
			: Text(text) {}

		RepitingText(RepitingText&&) = default;
	};

	

	static std::map<RepitingText> RepetCounter;
	
}
	#define RY_FRAME_BEGINN 
	#define RY_INTERALE_LOOP_IMPL(msg, ) if() RY_CORE_INFO("({0}) -> {1}",)
	#define RY_INTERALE_LOOP_MSG()
	#define RY_CORE_LOOP_INFO(...) RY_EXPAND_MOAKRO()
#else

#endif
#if RY_ENABLE_ASSERTS_MSG 
#define RY_NEW_ASSERT 1
#if RY_ENABLE_ASSERTS
	


	
	#if RY_NEW_ASSERT
		#define RY_INTERALE_ASSERT_IMPL(type, check, msg, ...)  { if(!(check)) {RY##type##ERROR(msg, __VA_ARGS__); RY_DEBUG_BREAK(); } }
		#define RY_INTERALE_ASSERT_WITH_MSG(type, check, ...) RY_INTERALE_ASSERT_IMPL(type, check, "Assertion Faild: {0}",__VA_ARGS__)
		#define RY_INTERALE_ASSERT_NO_MSG(type, check) RY_INTERALE_ASSERT_IMPL(type, check, "Assertion faild: {0} Faild at {1}:{2}", RY_STRINGIFY_MOAKRO(check), std::filesystem::path(__FILE__).string(), __LINE__)
		
		#define RY_INTERALE_ASSERT_GET_MACRO_NAME(arg1, arg2, marco, ...) marco
		#define RY_INTERALE_ASSERT_GET_MACRO(...) RY_EXPAND_MOAKRO(  RY_INTERALE_ASSERT_GET_MACRO_NAME(__VA_ARGS__, RY_INTERALE_ASSERT_WITH_MSG,  RY_INTERALE_ASSERT_NO_MSG)  )
		
		#define RY_ASSERT(...) RY_EXPAND_MOAKRO(  RY_INTERALE_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__)  )
		#define RY_CORE_ASSERT(...) RY_EXPAND_MOAKRO(  RY_INTERALE_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__)  )
	#else
		#define RY_ASSERT(x, ...) { if(!(x)) { RY_ERROR("Assertion Faild: {0}", __VA_ARGS__); __debugbreak(); } }
		#define RY_CORE_ASSERT(x, ...) { if(!(x)) { RY_CORE_ERROR("Assertion Faild: {0}", __VA_ARGS__); __debugbreak(); } }
	#endif

#else	
	#if RY_NEW_ASSERT
#define RY_INTERALE_ASSERT_IMPL(type, check, msg, ...)  { if(!(check)) { RY##type##ERROR( msg, __VA_ARGS__ ); } }
		#define RY_INTERALE_ASSERT_WITH_MSG(type, check, ...) RY_INTERALE_ASSERT_IMPL(type, check, "Assertion Faild: {}",__VA_ARGS__)
		#define RY_INTERALE_ASSERT_NO_MSG(type, check) RY_INTERALE_ASSERT_IMPL(type, check, "Assertion faild: {0} Faild at {1}:{2}", RY_STRINGIFY_MOAKRO(check), std::filesystem::path(__FILE__).string(), __LINE__)
		
		#define RY_INTERALE_ASSERT_GET_MACRO_NAME(arg1, arg2, marco, ...) marco
		#define RY_INTERALE_ASSERT_GET_MACRO(...) RY_EXPAND_MOAKRO(  RY_INTERALE_ASSERT_GET_MACRO_NAME(__VA_ARGS__, RY_INTERALE_ASSERT_WITH_MSG,  RY_INTERALE_ASSERT_NO_MSG)  )
		
		#define RY_ASSERT(...) RY_EXPAND_MOAKRO(  RY_INTERALE_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__)  )
		#define RY_CORE_ASSERT(...) RY_EXPAND_MOAKRO(  RY_INTERALE_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__)  )
	#else
		#define RY_ASSERT(x, ...) { if(!(x)) { RY_ERROR("Assertion Faild: {0}", __VA_ARGS__);  } }
		#define RY_CORE_ASSERT(x, ...) { if(!(x)) { RY_CORE_ERROR("Assertion Faild: {0}", __VA_ARGS__); } }
	#endif
	
#endif
#else
	#define RY_ASSERT(...)
	#define RY_CORE_ASSERT(...)
#endif