#pragma once

#include <memory>
#include "Rynex/Core/CoreConfig.h"

// Check Curent Platfrom Seport + massages for curent State
#ifdef _WIN32

	#ifdef _WIN64
		#define RY_PLATFORM_WINDOWS
	#else
		#error "Rynex only Seports x64 Bit Builds or Platforms (x84 Bit or x32 Bit Builds are not seported and x32 Bit Has no Plans to seport in Futer)"
	#endif // _WIN64

#elif defined(__APPLE__) || defined(__MACH__)

	#include <TargetConditionals.h>
	#if TARGET_IPHONE_SIMULATOR == 1
		#error "Rynex do not seported IOS simultion at Curent Time! (At the monoent they are no Plans to do it in Futer)"
	#elif TARGET_IPHONE_MAC == 1
		#define RY_PLATFORM_IOS
		#error "Rynex do not seported IOS at Curent Time! (At the monoent they are no Plans to do it in Futer)"
	#elif TARGET_OS_MAC == 1
		#define RY_PLATFORM_MAC
		#error "Rynex do not seported MacOS at Curent Time! (mabey in Futer seportetd)"
	#else
		#error "Rynex do not Unknown or seport this Aple Platform! (At the monoent they are no Plans to do it in Futer)"
	#endif

#elif defined(__ADROID__)

	#define RY_PLATFORM_ADROID
	#error "Rynex do not seported Android at Curent Time! (At the monoent they are no Plans to do it in Futer)"
#elif define(__linux__)

	#define RY_PLATFORM_LINUX
	#error "Rynex not seported Linux  at Curent Time!"

#else 
	#error "Rynex not seport Platform! Or Unknown!"
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
			// #define RYNEX_API
		#endif
	#else
		#define RYNEX_API
	#endif // RY_DYNAMIC_LINK
#endif


// DebugBreakPoint on Error
#if RY_ENABLE_ASSERTS
	#define RY_ASSERT(x, ...) { if(!(x)) { RY_ERROR("Assertion Faild: {0}", __VA_ARGS__); __debugbreak(); } }
	#define RY_CORE_ASSERT(x, ...) { if(!(x)) { RY_CORE_ERROR("Assertion Faild: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define RY_ASSERT(x, ...) 
	#define RY_CORE_ASSERT(x, ...) 
#endif

// Bit Operation
#define BIT(x) (1 << x)
#define BIT_AND(x, y) (x & y)
#define BIT_OR(x, y) (x | y)

// For Events
#define RY_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

#define COMBINE(x,y) x##y

// Globle Short Defines
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

}