#pragma once

#include <memory>

#ifdef RY_PLATFORM_WINDOWS
	#if RY_DYNAMIC_LINK
		#ifdef RY_BUILD_DLL
			#define RYNEX_API __declspec(dllexport)
			//#define RYNEX_API
		#else
			#define RYNEX_API __declspec(dllimport)
			//#define RYNEX_API
		#endif
	#else
		#define RYNEX_API
	#endif // RY_DYNAMIC_LINK
#else
	#error Rynex only seport Windows! for now
#endif

#define RY_ENABLE_ASSERTS

#ifdef  RY_ENABLE_ASSERTS
	#define RY_ASSERT(x, ...) { if(!(x)) { RY_ERROR("Assertion Faild: {0}", __VA_ARGS__); __debugbreak(); } }
	#define RY_CORE_ASSERT(x, ...) { if(!(x)) { RY_CORE_ERROR("Assertion Faild: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define RY_ASSERT(x, ...) 
	#define RY_CORE_ASSERT(x, ...) 
#endif

#define BIT(x) (1 << x)

#define RY_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

#define WINDOW_PROZENT_SIZE 0.6
#define IM_GUI_DEMO_WINDOW 0

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