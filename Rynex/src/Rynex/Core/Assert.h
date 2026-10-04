#pragma once

#include <Rynex/Core/Base.h>
#include <Rynex/Core/Log.h>

#define RY_ASSERT_HOOK_DEBUG_BREAK()    if (!(::Rynex::AssertHook::TryHandle(__FILE__, __LINE__))) { RY_DEBUG_BREAK(); }
#define RY_CORE_NOT_IMPL()              RY_CORE_FATAL("Function Not Implemted {} {}", std::filesystem::path(__FILE__).string(), __LINE__); RY_ASSERT_HOOK_DEBUG_BREAK()


#ifdef RY_DIST
    #define RY_ASSERT(...)
	#define RY_CORE_ASSERT(...)
#else
	#define RY_INTERNAL_ASSERT_IMPL(type, check, msg, ...)                              { if(!(check)) {RY##type##ERROR(msg, __VA_ARGS__); RY_ASSERT_HOOK_DEBUG_BREAK(); } }
	
	#define RY_INTERNAL_ASSERT_NO_MSG(type, check)                                      RY_INTERNAL_ASSERT_IMPL(type, check, "Assertion faild: {0} Faild at {1}:{2}", RY_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).string(), __LINE__)
	#define RY_INTERNAL_ASSERT_WITH_MSG(type, check, msg)                               RY_INTERNAL_ASSERT_IMPL(type, check, "Assertion Faild: {}", msg)
	#define RY_INTERNAL_ASSERT_WITH_MSG_ONE_ARG(type, check, msg, msgArg1)              RY_INTERNAL_ASSERT_IMPL(type, check, "Assertion Faild: " msg, msgArg1)
	#define RY_INTERNAL_ASSERT_WITH_MSG_MULTY_ARGS(type, check, msg, msgArg1, ...)      RY_INTERNAL_ASSERT_IMPL(type, check, "Assertion Faild: " msg,  msgArg1, __VA_ARGS__)
	
	#define RY_INTERNAL_ASSERT_GET_MACRO_NAME(arg3, arg2, arg1, msgArg, check, marco, ...) marco
	#define RY_INTERNAL_ASSERT_GET_MACRO(...)                                           RY_EXPAND_MACRO(  RY_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, RY_INTERNAL_ASSERT_WITH_MSG_MULTY_ARGS, RY_INTERNAL_ASSERT_WITH_MSG_MULTY_ARGS, RY_INTERNAL_ASSERT_WITH_MSG_ONE_ARG,  RY_INTERNAL_ASSERT_WITH_MSG, RY_INTERNAL_ASSERT_NO_MSG, RY_INTERNAL_ASSERT_0)  )
	
	#define RY_ASSERT(...)                                                              RY_EXPAND_MACRO(  RY_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__)  )
	#define RY_CORE_ASSERT(...)                                                         RY_EXPAND_MACRO(  RY_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__)  )
#endif

namespace Rynex::AssertHook {
    using HandlerFn = void(*)(const char* file, int line);

    inline std::atomic<HandlerFn>& Slot()
    {
        static std::atomic<HandlerFn> s_Handler{ nullptr };
        return s_Handler;
    }

    inline HandlerFn Set(const HandlerFn fn) { return Slot().exchange(fn); }   // reset to previous.

    // true = Handler was set and has returned; if the Handler throws, he doesn't come back
    inline bool TryHandle(const char* file, const int line)
    {
        const std::atomic<HandlerFn>&  slot = Slot();
        const HandlerFn fn = slot.load(std::memory_order_relaxed);
        if (nullptr == fn)
        {
            return false;
        }
        fn(file, line);
        return true;
    }
}