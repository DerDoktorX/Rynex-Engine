#pragma once
#include "Rynex/Core/Config.h"

#include <memory>


// Check Curent Platfrom Seport + massages for curent State
#ifdef _WIN32

	#ifdef _WIN64
		#define RY_DEBUG_BREAK() __debugbreak();
	#else
		#error "Rynex only Seports x64 Bit Builds or Platforms (x84 Bit or x32 Bit Builds are not seported and x32 Bit Has no Plans to seport in Futer)"
		#define RY_DEBUG_BREAK()
	#endif // _WIN64

#elif defined(__APPLE__) || defined(__MACH__)

	#include <TargetConditionals.h>
	#if TARGET_IPHONE_SIMULATOR == 1
		#error "Rynex do not seported IOS simultion at Curent Time! (At the monoent they are no Plans to do it in Futer)"
	#elif TARGET_IPHONE_MAC == 1
		#error "Rynex do not seported IOS at Curent Time! (At the monoent they are no Plans to do it in Futer)"
	#elif TARGET_OS_MAC == 1
		#error "Rynex do not seported MacOS at Curent Time! (mabey in Futer seportetd)"
	#else
		#error "Rynex do not Unknown or seport this Aple Platform! (At the monoent they are no Plans to do it in Futer)"
	#endif
	#define RY_DEBUG_BREAK()
#elif defined(__ADROID__)
	#define RY_DEBUG_BREAK()
	#error "Rynex do not seported Android at Curent Time! (At the monoent they are no Plans to do it in Futer)"
#elif define(__linux__)
	#error "Rynex not seported Linux  at Curent Time!"
	#define RY_DEBUG_BREAK()
#else 
	#error "Rynex not seport Platform! Or Unknown!"
	#include <signal.h>
	#define RY_DEBUG_BREAK() raise(SIGTRAP)
#endif
// This Error Masseges are written from 21.07.2024
// TDOD: Check if State mants to Seport or not Seport has change


// Platforms specific Definse

// Windows	|
//			V
#ifdef RY_PLATFORM_WINDOWS
	#if RY_DYNAMIC_LINK
		#ifdef RY_BUILD_DLL
			#define RYNEX_API __declspec(dllexport)
			// #define RYNEX_API
		#else
			#define RYNEX_API __declspec(dllimport)
			// RYNEX_API
		#endif
	#else //  DYNAMIC LINKING
		#define RYNEX_API
	#endif // STATIC LINKING
#endif

// Bit Operation
#define BIT(x) (1 << x)
#define BIT_NOT(x) (~x)

#define BIT_AND(x, y) (x & y)
#define BIT_OR(x, y) (x | y)
#define BIT_XOR(x, y) (x ^ y)


#define BIT_EQUAL(x, y) BIT_AND(x, y)
#define BIT_SET(x, y) BIT_OR(x, y)
#define BIT_SET_NOT_EQUAL(x, y) BIT_XOR(x, y)
#define BIT_SET_ON(state, x, to)  state ? BIT_OR(to, x) : BIT_EQUAL(to, BIT_NOT(x))

// For Events
#define RY_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

#define RY_COMBINE_MOAKRO(x,y) x##y
#define RY_EXPAND_MOAKRO(x) x
#define RY_STRINGIFY_MOAKRO(x) #x

#define PRINT_BG_WHITE "\033[7m"
#define PRINT_BG_BLACK "\033[40m"
#define PRINT_BG_RED "\033[41m"
#define PRINT_BG_GREEN "\033[42m"
#define PRINT_BG_YALOW "\033[43m"
#define PRINT_BG_BLUE "\033[44m"
#define PRINT_BG_PURPL "\033[45m"
#define PRINT_BG_CYNE "\033[46m"
#define PRINT_BG_GRAY "\033[47m"

#define PRINT_TX_WHITE "\033[0m"
#define PRINT_TX_BLACK "\033[30m"
#define PRINT_TX_RED "\033[0;31m"
#define PRINT_TX_GREEN "\033[0;32m"
#define PRINT_TX_YALOW "\033[0;33m"
#define PRINT_TX_BLUE "\033[0;34m"
#define PRINT_TX_PURPL "\033[0;35m"
#define PRINT_TX_CYNE "\033[0;36m"
#define PRINT_TX_GRAY "\033[0;37m"

#define PRINT_BRIGTH_TX_DARK_GRAY "\033[90m"
#define PRINT_BRIGTH_TX_RED "\033[91m"
#define PRINT_BRIGTH_TX_GREEN "\033[02m"
#define PRINT_BRIGTH_TX_YALOW "\033[93m"
#define PRINT_BRIGTH_TX_BLUE "\033[94m"
#define PRINT_BRIGTH_TX_PURPL "\033[95m"
#define PRINT_BRIGTH_TX_CYNE "\033[96m"
#define PRINT_BRIGTH_TX_GRAY "\033[97m"


#define PRINT_BRIGTH_BG_DARK_GRAY "\033[101m"
#define PRINT_BRIGTH_BG_RED "\033[101m"
#define PRINT_BRIGTH_BG_GREEN "\033[102m"
#define PRINT_BRIGTH_BG_YALOW "\033[103m"
#define PRINT_BRIGTH_BG_BLUE "\033[104m"
#define PRINT_BRIGTH_BG_PURPL "\033[105m"
#define PRINT_BRIGTH_BG_CYNE "\033[106m"
#define PRINT_BRIGTH_BG_GRAY "\033[107m"

#define PRINT_REST "\033[0m\033[40m"


#include "Rynex/Core/Log.h"
#include "Rynex/Core/Assert.h"

// Globle Short Defines
#if RY_STD_PTR_IMPL

namespace Rynex {

	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}


	template<typename T>
	using Ref = std::shared_ptr<T>;

	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	// template<typename T>
	// static T GetEnumFormString(std::string_view e);
	// {
	// 	std::optional<T> type = magic_enum::enum_cast<T>(e, magic_enum::case_insensitive);
	// 	if (type.has_value())
	// 		return type.value();
	// 	return T::None;
	// }

	// template<typename T>
	// static std::string_view GetStringFromEnum(T e);
	// {
	// 	return magic_enum::enum_name(e);
	// }
#if RY_DISABLE_WEAK_PTR-1

	template<typename T>
	using Weak = std::weak_ptr<T>;

	

#endif

}
#else

#include "Rynex/Ptr/Ref.h"
#include "Rynex/Ptr/Scope.h"

#if RY_DISABLE_WEAK_PTR-1

#include "Rynex/Ptr/Weak.h"

#endif

#endif

