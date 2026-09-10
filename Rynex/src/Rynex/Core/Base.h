#pragma once
#include "Rynex/Core/Config.h"

#include <memory>
#include <robin_hood.h>
#include <unordered_map>
#include <map>

// Check Curent Platfrom Seport + massages for curent State
#ifdef _WIN32

	#ifdef _WIN64
		#if RY_GOOGLE_TEST
				#define RY_DEBUG_BREAK() std::abort()
		#else
				#define RY_DEBUG_BREAK() __debugbreak()
		#endif
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
#elif defined(__linux__)
	#error "Rynex not seported Linux  at Curent Time!"
	#include <signal.h>
	#define RY_DEBUG_BREAK() raise(SIGTRAP);
#else 
	#error "Rynex not seport Platform! Or Unknown!"
	#include <signal.h>
	#define RY_DEBUG_BREAK() raise(SIGTRAP)
#endif
// This Error Messages are written from 21.07.2024
// TODD: Check if State mans to Seport or not Seport has change

// check valid compile flags
#ifdef RY_DEBUG
	#ifdef RY_DIST
		#ifdef RY_RELEASE
			#error "RY_DIST is defined also RY_DEBUG and RY_RELEASE"
		#elif defined(RY_DIST)
			#error "RY_DIST is defined also RY_DEBUG"
		#endif
	#elif defined(RY_RELEASE)
		#error "RY_DEBUG is defined also RY_RELEASE"
	#endif
#elif defined(RY_RELEASE)
	#ifdef RY_DIST
		#error "RY_RELEASE is defined also RY_DIST"
	#elif defined(RY_DEBUG)
		#error "RY_RELEASE is defined also RY_DEBUG"
	#endif
#elif defined(RY_DIST)
	#ifdef RY_DEBUG
		#error "RY_DIST is defined also RY_DIST"
	#elif defined(RY_RELEASE)
		#error "RY_DIST is defined also RY_RELEASE"
	#endif
#endif

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

#ifdef RY_PLATFORM_LINUX
	#if RY_DYNAMIC_LINK
		#ifdef RY_BUILD_DLL
			#define RYNEX_API		__attribute__((visibility("default")))
	#else
		#define RYNEX_API		__attribute__((visibility("default")))
	#endif
#else
	#define RYNEX_API
	#endif
#endif


// Bit Operation
#define BIT(x)						(1 << x)

#define BIT_NOT(x)					(~x)

#define BIT_AND(x, y)				(x & y)
#define BIT_OR(x, y)				(x | y)
#define BIT_XOR(x, y)				(x ^ y) // 


#define BIT_EQUAL(x, y)				BIT_AND((x) , (y))
#define BIT_SET(x, y)				BIT_OR(x, y)
#define BIT_SET_NOT_EQUAL(x, y)		BIT_XOR(x, y)

#define BIT_SET_ON(state, x, to)	state ? BIT_OR(  to, x  ) : BIT_EQUAL(  to, BIT_NOT( x )  )


#define FRACTURE(a, b)				(a / b)
#define FRACTURE_FORM(a, b, x)		( FRACTURE( (a),(x) ) * (b) )
#define FRACTURE_ONE(x, a)			FRACTURE_FORM(a-1, a, x)





#define RY_COMBINE_MOAKRO(x,y) x##y
#define RY_EXPAND_MOAKRO(x) x
#define RY_STRINGIFY_MOAKRO(x) #x



// class member func


#pragma region ActionPerArgMacros

#define RY_GET_INTERNEL_PLEAZHOLDER(n, macro)		RY_COMBINE_MOAKRO(macro, n)

#define RY_INTERNEL_ARG_N( \
          _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
         _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
         _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
         _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
         _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
         _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
         _61,_62,_63, NAME,...) NAME

#define RY_INTERNEL_RSEQ_N() \
         63,62,61,60,                   \
         59,58,57,56,55,54,53,52,51,50, \
         49,48,47,46,45,44,43,42,41,40, \
         39,38,37,36,35,34,33,32,31,30, \
         29,28,27,26,25,24,23,22,21,20, \
         19,18,17,16,15,14,13,12,11,10, \
         9,8,7,6,5,4,3,2,1,0

#define RY_INTERNEL_NARG_(...)						RY_EXPAND_MOAKRO(RY_INTERNEL_ARG_N(__VA_ARGS__))
#define RY_INTERNEL_NARG(...)						RY_INTERNEL_NARG_(__VA_ARGS__, RY_INTERNEL_RSEQ_N())


#define RY_ACTOIN_PER_ARG(macro, ...)									RY_GET_INTERNEL_PLEAZHOLDER(  RY_INTERNEL_NARG( __VA_ARGS__ ) , macro  )

#define RY_ACTOIN_PER_ARG_USE_0(_1, macro, ...)																						RY_EXPAND_MOAKRO(  RY_ACTOIN_PER_ARG(macro, __VA_ARGS__)(							__VA_ARGS__ )  )
#define RY_ACTOIN_PER_ARG_USE_2(_1, _2, macro, ...)																					RY_EXPAND_MOAKRO(  RY_ACTOIN_PER_ARG(macro, __VA_ARGS__)(_1, _2,					__VA_ARGS__ )  )
#define RY_ACTOIN_PER_ARG_USE_3(_1, _2, _3, macro, ...)																				RY_EXPAND_MOAKRO(  RY_ACTOIN_PER_ARG(macro, __VA_ARGS__)(_1, _2, _3,				__VA_ARGS__ )  )
#define RY_ACTOIN_PER_ARG_USE_4(_1, _2, _3, _4, macro, ...)																			RY_EXPAND_MOAKRO(  RY_ACTOIN_PER_ARG(macro, __VA_ARGS__)(_1, _2, _3, _4,			__VA_ARGS__ )  )
#define RY_ACTOIN_PER_ARG_USE_5(_1, _2, _3, _4, _5, macro, ...)																		RY_EXPAND_MOAKRO(  RY_ACTOIN_PER_ARG(macro, __VA_ARGS__)(_1, _2, _3, _4, _5,		__VA_ARGS__ )  )
#define RY_ACTOIN_PER_ARG_USE_6(_1, _2, _3, _4, _5, _6, macro, ...)																	RY_EXPAND_MOAKRO(  RY_ACTOIN_PER_ARG(macro, __VA_ARGS__)(_1, _2, _3, _4, _5, _6,	__VA_ARGS__ )  )
#define RY_ACTOIN_PER_ARG_USE_7(_1, _2, _3, _4, _5, _6, _7, macro, ...)																RY_EXPAND_MOAKRO(  RY_ACTOIN_PER_ARG(macro, __VA_ARGS__)(_1, _2, _3, _4, _5, _6 ,_7,	__VA_ARGS__ )  )
#define RY_ACTOIN_PER_ARG_USE_8(_1, _2, _3, _4, _5, _6, _7,  _8, macro, ...)														RY_EXPAND_MOAKRO(  RY_ACTOIN_PER_ARG(macro, __VA_ARGS__)(_1, _2, _3, _4, _5, _6 ,_7, _8,	__VA_ARGS__ )  )
#define RY_ACTOIN_PER_ARG_USE_9(_1, _2, _3, _4, _5, _6, _7,  _8,  _9, macro, ...)													RY_EXPAND_MOAKRO(  RY_ACTOIN_PER_ARG(macro, __VA_ARGS__)(_1, _2, _3, _4, _5, _6 ,_7, _8,  _9,	__VA_ARGS__ )  )
#define RY_ACTOIN_PER_ARG_USE_10(_1, _2, _3, _4, _5, _6, _7, _8,  _9, _10, macro, ...)												RY_EXPAND_MOAKRO(  RY_ACTOIN_PER_ARG(macro, __VA_ARGS__)(_1, _2, _3, _4, _5, _6 ,_7, _8,  _9, _10,	__VA_ARGS__ )  )
#define RY_ACTOIN_PER_ARG_USE_11(_1, _2, _3, _4, _5, _6, _7, _8,  _9, _10, _11, macro, ...)											RY_EXPAND_MOAKRO(  RY_ACTOIN_PER_ARG(macro, __VA_ARGS__)(_1, _2, _3, _4, _5, _6 ,_7, _8,  _9, _10, _11,	__VA_ARGS__ )  )
#define RY_ACTOIN_PER_ARG_USE_12(_1, _2, _3, _4, _5, _6, _7, _8,  _9, _10, _11, _12, macro, ...)									RY_EXPAND_MOAKRO(  RY_ACTOIN_PER_ARG(macro, __VA_ARGS__)(_1, _2, _3, _4, _5, _6 ,_7, _8,  _9, _10, _11,	_12,__VA_ARGS__ )  )
#define RY_ACTOIN_PER_ARG_USE_13(_1, _2, _3, _4, _5, _6, _7, _8,  _9, _10, _11, _12, _13, macro, ...)								RY_EXPAND_MOAKRO(  RY_ACTOIN_PER_ARG(macro, __VA_ARGS__)(_1, _2, _3, _4, _5, _6 ,_7, _8,  _9, _10, _11, _12, _13,	__VA_ARGS__ )  )
#define RY_ACTOIN_PER_ARG_USE_14(_1, _2, _3, _4, _5, _6, _7, _8,  _9, _10, _11, _12, _13, _14, macro, ...)							RY_EXPAND_MOAKRO(  RY_ACTOIN_PER_ARG(macro, __VA_ARGS__)(_1, _2, _3, _4, _5, _6 ,_7, _8,  _9, _10, _11, _12, _13, _14,	__VA_ARGS__ )  )
#define RY_ACTOIN_PER_ARG_USE_15(_1, _2, _3, _4, _5, _6, _7, _8,  _9, _10, _11, _12, _13, _14, _15, macro, ...)						RY_EXPAND_MOAKRO(  RY_ACTOIN_PER_ARG(macro, __VA_ARGS__)(_1, _2, _3, _4, _5, _6 ,_7, _8,  _9, _10, _11, _12, _13, _14, _15,	__VA_ARGS__ )  )
#define RY_ACTOIN_PER_ARG_USE_16(_1, _2, _3, _4, _5, _6, _7, _8,  _9, _10, _11, _12, _13, _14, _15, _16, macro, ...)				RY_EXPAND_MOAKRO(  RY_ACTOIN_PER_ARG(macro, __VA_ARGS__)(_1, _2, _3, _4, _5, _6 ,_7, _8,  _9, _10, _11, _12, _13, _14, _15, _16,	__VA_ARGS__ )  )
#define RY_ACTOIN_PER_ARG_USE_17(_1, _2, _3, _4, _5, _6, _7, _8,  _9, _10, _11, _12, _13, _14, _15, _16, _17, macro, ...)			RY_EXPAND_MOAKRO(  RY_ACTOIN_PER_ARG(macro, __VA_ARGS__)(_1, _2, _3, _4, _5, _6 ,_7, _8,  _9, _10, _11, _12, _13, _14, _15,	_16, _17,	__VA_ARGS__ )  )
#define RY_ACTOIN_PER_ARG_USE_18(_1, _2, _3, _4, _5, _6, _7, _8,  _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, macro, ...)		RY_EXPAND_MOAKRO(  RY_ACTOIN_PER_ARG(macro, __VA_ARGS__)(_1, _2, _3, _4, _5, _6 ,_7, _8,  _9, _10, _11, _12, _13, _14, _15,	_16, _17, _18,		__VA_ARGS__ )  )
#define RY_ACTOIN_PER_ARG_USE_19(_1, _2, _3, _4, _5, _6, _7, _8,  _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, macro, ...)	RY_EXPAND_MOAKRO(  RY_ACTOIN_PER_ARG(macro, __VA_ARGS__)(_1, _2, _3, _4, _5, _6 ,_7, _8,  _9, _10, _11, _12, _13, _14, _15,	_16, _17, _18, _19,		__VA_ARGS__ )  )




#pragma endregion

#pragma region BindMemberFuncMacros

#define RY_BIND_EVENT_FN(func)				std::bind(&func, this, std::placeholders::_1)
#define RY_BIND_MEMBER_FN(func)				std::bind(&func, this)
#define RY_BIND_MEMBER_FN_ARGS(func, ...)	std::bind(&func, this, __VA_ARGS__)


#define RY_BIND_MEMBER_FUNC_INTERNALE_ARGS_PLACEHOLDER(macro)				RY_COMBINE_MOAKRO( std::placeholders::_, macro ) 

#define RY_BIND_MEMBER_FUNC_INTERNALE_NONE_ARGS(func)						std::bind(&func, this)
#define RY_BIND_MEMBER_FUNC_INTERNALE_ARGS(func, ...)						std::bind(&func, this, RY_BIND_MEMBER_FUNC_INTERNALE_ARGS_PLACEHOLDER( RY_INTERNEL_NARG(__VA_ARGS__))) 

#define RY_BIND_MEMBER_FUNC_INTERNALE_GET_MACRO(\
	 _0, _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
     _11, _12,_13,_14,_15,_16,_17,_18,_19,_20, \
	macro, ...) macro


#define RY_BIND_MEMBER_FUNC(...)		RY_EXPAND_MOAKRO(																			\
													RY_BIND_MEMBER_FUNC_INTERNALE_GET_MACRO(										\
														__VA_ARGS__																	\
														, RY_BIND_MEMBER_FUNC_INTERNALE_ARGS, RY_BIND_MEMBER_FUNC_INTERNALE_ARGS	\
														, RY_BIND_MEMBER_FUNC_INTERNALE_ARGS, RY_BIND_MEMBER_FUNC_INTERNALE_ARGS	\
														, RY_BIND_MEMBER_FUNC_INTERNALE_ARGS, RY_BIND_MEMBER_FUNC_INTERNALE_ARGS	\
														, RY_BIND_MEMBER_FUNC_INTERNALE_ARGS, RY_BIND_MEMBER_FUNC_INTERNALE_ARGS	\
														, RY_BIND_MEMBER_FUNC_INTERNALE_ARGS, RY_BIND_MEMBER_FUNC_INTERNALE_ARGS	\
														, RY_BIND_MEMBER_FUNC_INTERNALE_ARGS, RY_BIND_MEMBER_FUNC_INTERNALE_ARGS	\
														, RY_BIND_MEMBER_FUNC_INTERNALE_ARGS, RY_BIND_MEMBER_FUNC_INTERNALE_ARGS	\
														, RY_BIND_MEMBER_FUNC_INTERNALE_ARGS, RY_BIND_MEMBER_FUNC_INTERNALE_ARGS	\
														, RY_BIND_MEMBER_FUNC_INTERNALE_ARGS, RY_BIND_MEMBER_FUNC_INTERNALE_ARGS	\
														, RY_BIND_MEMBER_FUNC_INTERNALE_ARGS, RY_BIND_MEMBER_FUNC_INTERNALE_ARGS	\
														, RY_BIND_MEMBER_FUNC_INTERNALE_NONE_ARGS									\
													)																				\
											(__VA_ARGS__)																			\
										)

#pragma endregion 

#pragma region NoneMemberOperationMacros


#define RY_NONE_MEBER_OPERATOR_INTERNALE_MACRO(operation, varibleName) a.varibleName operation b.varibleName

#define RY_NONE_MEBER_OPERATOR_INTERNALE_MACRO_TYPE(operation, varibleName, valueLeftIn, valueRigthIn, combineSymbol) \
	valueLeftIn#combineSymbol#varibleName \
	operation \
	valueRigthIn#combineSymbol#varibleName 

#define RY_NONE_MEBER_OPERATOR_INTERNALE_MACRO_SPECIAL(operation, varibleLeftName, varibleRigthName) \
	varibleLeftName operation varibleRigthName

#define RY_NONE_MEBER_OPERATOR_PLACEHOLDER_0(operation, conector, macro)
#define RY_NONE_MEBER_OPERATOR_PLACEHOLDER_1(operation, conector, macro, _1)																							macro(operation, _1) 
#define RY_NONE_MEBER_OPERATOR_PLACEHOLDER_2(operation, conector, macro, _1, _2)																						macro(operation, _1) conector RY_NONE_MEBER_OPERATOR_PLACEHOLDER_1(operation, conector, macro, _2)
#define RY_NONE_MEBER_OPERATOR_PLACEHOLDER_3(operation, conector, macro, _1, _2, _3)																					macro(operation, _1) conector RY_NONE_MEBER_OPERATOR_PLACEHOLDER_2(operation, conector, macro, _2, _3)
#define RY_NONE_MEBER_OPERATOR_PLACEHOLDER_4(operation, conector, macro, _1, _2, _3, _4)																				macro(operation, _1) conector RY_NONE_MEBER_OPERATOR_PLACEHOLDER_3(operation, conector, macro, _2, _3, _4)
#define RY_NONE_MEBER_OPERATOR_PLACEHOLDER_5(operation, conector, macro, _1, _2, _3, _4, _5)																			macro(operation, _1) conector RY_NONE_MEBER_OPERATOR_PLACEHOLDER_4(operation, conector, macro, _2, _3, _4, _5)
#define RY_NONE_MEBER_OPERATOR_PLACEHOLDER_6(operation, conector, macro, _1, _2, _3, _4, _5, _6)																		macro(operation, _1) conector RY_NONE_MEBER_OPERATOR_PLACEHOLDER_5(operation, conector, macro, _2, _3, _4, _5, _6)					
#define RY_NONE_MEBER_OPERATOR_PLACEHOLDER_7(operation, conector, macro, _1, _2, _3, _4, _5, _6, _7)																	macro(operation, _1) conector RY_NONE_MEBER_OPERATOR_PLACEHOLDER_6(operation, conector, macro, _2, _3, _4, _5, _6, _7)				
#define RY_NONE_MEBER_OPERATOR_PLACEHOLDER_8(operation, conector, macro, _1, _2, _3, _4, _5, _6, _7, _8)																macro(operation, _1) conector RY_NONE_MEBER_OPERATOR_PLACEHOLDER_7(operation, conector, macro, _2, _3, _4, _5, _6, _7, _8)			
#define RY_NONE_MEBER_OPERATOR_PLACEHOLDER_9(operation, conector, macro, _1, _2, _3, _4, _5, _6, _7, _8, _9)															macro(operation, _1) conector RY_NONE_MEBER_OPERATOR_PLACEHOLDER_8(operation, conector, macro, _2, _3, _4, _5, _6, _7, _8, _9)		
#define RY_NONE_MEBER_OPERATOR_PLACEHOLDER_10(operation, conector, macro, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10)														macro(operation, _1) conector RY_NONE_MEBER_OPERATOR_PLACEHOLDER_9(operation, conector, macro, _2, _3, _4, _5, _6, _7, _8, _9, _10)
#define RY_NONE_MEBER_OPERATOR_PLACEHOLDER_11(operation, conector, macro, _1, _2, _3, _4, _5, _6, _7, _8,  _9, _10, _11)												macro(operation, _1) conector RY_NONE_MEBER_OPERATOR_PLACEHOLDER_10(operation, conector, macro, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11)
#define RY_NONE_MEBER_OPERATOR_PLACEHOLDER_12(operation, conector, macro, _1, _2, _3, _4, _5, _6, _7, _8,  _9, _10, _11, _12)											macro(operation, _1) conector RY_NONE_MEBER_OPERATOR_PLACEHOLDER_11(operation, conector, macro, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12)
#define RY_NONE_MEBER_OPERATOR_PLACEHOLDER_13(operation, conector, macro, _1, _2, _3, _4, _5, _6, _7, _8,  _9, _10, _11, _12, _13)										macro(operation, _1) conector RY_NONE_MEBER_OPERATOR_PLACEHOLDER_12(operation, conector, macro, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13)
#define RY_NONE_MEBER_OPERATOR_PLACEHOLDER_14(operation, conector, macro, _1, _2, _3, _4, _5, _6, _7, _8,  _9, _10, _11, _12, _13, _14)									macro(operation, _1) conector RY_NONE_MEBER_OPERATOR_PLACEHOLDER_13(operation, conector, macro, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14)
#define RY_NONE_MEBER_OPERATOR_PLACEHOLDER_15(operation, conector, macro, _1, _2, _3, _4, _5, _6, _7, _8,  _9, _10, _11, _12, _13, _14, _15)							macro(operation, _1) conector RY_NONE_MEBER_OPERATOR_PLACEHOLDER_14(operation, conector, macro, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15)
#define RY_NONE_MEBER_OPERATOR_PLACEHOLDER_16(operation, conector, macro, _1, _2, _3, _4, _5, _6, _7, _8,  _9, _10, _11, _12, _13, _14, _15, _16)						macro(operation, _1) conector RY_NONE_MEBER_OPERATOR_PLACEHOLDER_15(operation, conector, macro, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16)
#define RY_NONE_MEBER_OPERATOR_PLACEHOLDER_17(operation, conector, macro, _1, _2, _3, _4, _5, _6, _7, _8,  _9, _10, _11, _12, _13, _14, _15, _16, _17)					macro(operation, _1) conector RY_NONE_MEBER_OPERATOR_PLACEHOLDER_16(operation, conector, macro, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17)
#define RY_NONE_MEBER_OPERATOR_PLACEHOLDER_18(operation, conector, macro, _1, _2, _3, _4, _5, _6, _7, _8,  _9, _10, _11, _12, _13, _14, _15, _16, _17, _18)				macro(operation, _1) conector RY_NONE_MEBER_OPERATOR_PLACEHOLDER_17(operation, conector, macro, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18)
#define RY_NONE_MEBER_OPERATOR_PLACEHOLDER_19(operation, conector, macro, _1, _2, _3, _4, _5, _6, _7, _8,  _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19)		macro(operation, _1) conector RY_NONE_MEBER_OPERATOR_PLACEHOLDER_18(operation, conector, macro, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19)



#define RY_NONE_MEBER_OPERATOR_FUNC(typeIn, typeOut, operation, macro, ...)						\
		inline typeOut operator operation (const typeIn & a, const typeIn & b)					\
		{																						\
			typeOut result = RY_EXPAND_MOAKRO(macro(operation, __VA_ARGS__));					\
			return result;																		\
		}

#define RY_NONE_MEBER_OPERATOR_FUNC_TYPE(typeLeftIn, typeRigthtIn, typeOut, operation, macro, ...)		\
		inline typeOut operator operation (const typeLeftIn & a, const typeRigthtIn & b)				\
		{																								\
			typeOut result = RY_EXPAND_MOAKRO(macro(operation, __VA_ARGS__));							\
			return result;																				\
		}

#define RY_NONE_MEBER_OPERATOR_FUNC_SPECIAL(typeLeftIn, valueLeftIn, typeRigthtIn, valueRigthIn, typeOut, valueOperationLeft, valueOperationRigth, operation, macro, ...)	\
		inline typeOut operator operation (typeLeftIn valueLeftIn, typeRigthtIn valueRigthIn)								\
		{																													\
			typeOut result = RY_EXPAND_MOAKRO(macro(operation, valueOperationLeft, valueOperationRigth, __VA_ARGS__));		\
			return result;																									\
		}																													\

		
#define RY_NONE_MEBER_OPERATOR_BOOL_EQEUAL_AND_NOT_EQUAL_TYPE(typeInA, valueInA, typeInB, valueInB, valueOperationA, valueOperationB)										\
	RY_NONE_MEBER_OPERATOR_FUNC_SPECIAL(typeInA, valueInA, typeInB, valueInB, bool, valueOperationA, valueOperationB, ==,RY_NONE_MEBER_OPERATOR_INTERNALE_MACRO_SPECIAL)		\
	RY_NONE_MEBER_OPERATOR_FUNC_SPECIAL(typeInA, valueInA, typeInB, valueInB, bool, valueOperationA, valueOperationB, !=,RY_NONE_MEBER_OPERATOR_INTERNALE_MACRO_SPECIAL)		\

#define RY_NONE_MEBER_OPERATOR_BOOL_EQEUAL_AND_NOT_EQUAL_OBJECT(typeIn, valueOperation) \
	RY_NONE_MEBER_OPERATOR_BOOL_EQEUAL_AND_NOT_EQUAL_TYPE(typeIn, rigth, typeIn, left, RY_COMBINE_MOAKRO(rigth., valueOperation), RY_COMBINE_MOAKRO(left., valueOperation))


#define RY_NONE_MEBER_OPERATOR_INTERNALE_GET_MACRO(operation, conector, ...)		\
		RY_ACTOIN_PER_ARG_USE_3(operation, conector, RY_NONE_MEBER_OPERATOR_INTERNALE_MACRO, RY_NONE_MEBER_OPERATOR_PLACEHOLDER_, __VA_ARGS__)

#define RY_NONE_MEBER_OPERATOR_BOOL(typeIn, operation, conector, ...) \
	RY_NONE_MEBER_OPERATOR_FUNC_TYPE(typeIn, typeIn, bool, operation, RY_NONE_MEBER_OPERATOR_INTERNALE_GET_MACRO, conector, __VA_ARGS__)


#define RY_ADD_NONE_MEBER_OPERATOR_FUNC_AS_FRIND(typeIn, typeOut, operation) friend typeOut operator operation (const typeIn & a, const typeIn & b)


#pragma endregion

#define RY_CASE_STR_MACRO(macro)		case macro: return #macro
#define RY_CASE_RETURN(macro, value)	case macro: return value


#pragma region colorsPrintfFuncMacrois


#define PRINT_BG_WHITE				"\033[7m"
#define PRINT_BG_BLACK				"\033[40m"
#define PRINT_BG_RED				"\033[41m"
#define PRINT_BG_GREEN				"\033[42m"
#define PRINT_BG_YALOW				"\033[43m"
#define PRINT_BG_BLUE				"\033[44m"
#define PRINT_BG_PURPL				"\033[45m"
#define PRINT_BG_CYNE				"\033[46m"
#define PRINT_BG_GRAY				"\033[47m"

#define PRINT_TX_WHITE				"\033[0m"
#define PRINT_TX_BLACK				"\033[30m"
#define PRINT_TX_RED				"\033[0;31m"
#define PRINT_TX_GREEN				"\033[0;32m"
#define PRINT_TX_YALOW				"\033[0;33m"
#define PRINT_TX_BLUE				"\033[0;34m"
#define PRINT_TX_PURPL				"\033[0;35m"
#define PRINT_TX_CYNE				"\033[0;36m"
#define PRINT_TX_GRAY				"\033[0;37m"

#define PRINT_BRIGTH_TX_DARK_GRAY	"\033[90m"
#define PRINT_BRIGTH_TX_RED			"\033[91m"
#define PRINT_BRIGTH_TX_GREEN		"\033[02m"
#define PRINT_BRIGTH_TX_YALOW		"\033[93m"
#define PRINT_BRIGTH_TX_BLUE		"\033[94m"
#define PRINT_BRIGTH_TX_PURPL		"\033[95m"
#define PRINT_BRIGTH_TX_CYNE		"\033[96m"
#define PRINT_BRIGTH_TX_GRAY		"\033[97m"


#define PRINT_BRIGTH_BG_DARK_GRAY	"\033[101m"
#define PRINT_BRIGTH_BG_RED			"\033[101m"
#define PRINT_BRIGTH_BG_GREEN		"\033[102m"
#define PRINT_BRIGTH_BG_YALOW		"\033[103m"
#define PRINT_BRIGTH_BG_BLUE		"\033[104m"
#define PRINT_BRIGTH_BG_PURPL		"\033[105m"
#define PRINT_BRIGTH_BG_CYNE		"\033[106m"
#define PRINT_BRIGTH_BG_GRAY		"\033[107m"

#define PRINT_REST					"\033[0m\033[40m"

#pragma endregion

#include "Rynex/Core/Log.h"
#include "Rynex/Core/Assert.h"



namespace Rynex {
#ifdef RY_ROBINE_HOOD_HASH_MAIN_MAP
	template<typename Key, typename T, typename Hasher = robin_hood::hash<Key>, typename KeyEqual = std::equal_to<Key>, size_t MaxLoadFactor100 = 80ull>
	using HashMapFlat = robin_hood::unordered_flat_map<Key, T, Hasher, KeyEqual, MaxLoadFactor100>;

	template<typename Key, typename T, typename Hasher = robin_hood::hash<Key>, typename KeyEqual = std::equal_to<Key>, size_t MaxLoadFactor80 = 80ull>
	using HashMapNode = robin_hood::unordered_node_map<Key, T, Hasher, KeyEqual, MaxLoadFactor80>;


	
#else


	template<typename Key, typename T, typename Hasher = std::hash<Key>, typename KeyEqual = std::equal_to<Key>, typename Alloc = std::allocator<std::pair<const Key, T>>>
	using HashMapFlat = std::unordered_map<Key, T, Hasher, KeyEqual, Alloc>;

	template<typename Key, typename T, typename Hasher = std::hash<Key>, typename KeyEqual = std::equal_to<Key>, typename Alloc = std::allocator<std::pair<const Key, T>>>
	using HashMapNode = std::unordered_map<Key, T, Hasher, KeyEqual, Alloc>;

	
#endif

	template<typename Key, typename T, typename Pr = std::less<Key>, typename Alloc = std::equal_to<Key>>
	using OrderedMap = std::map<Key, T, std::allocator<std::pair<const Key, T>>>;


	template<typename T>
	struct IsScopePtr : std::false_type {};

	template<typename T>
	struct IsScopePtr<std::unique_ptr<T>> : std::true_type {};

	template<typename T>
	constexpr void CheckScopePtr(const T&)
	{
		static_assert(IsScopePtr<T>::value, "No Scope Ptr!");
	}

	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	};

	template<typename T>
	using Ref = std::shared_ptr<T>;

	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
	
	template<typename T>
	struct IsRefPtr : std::false_type {};

	template<typename T>
	struct IsRefPtr<std::shared_ptr<T>> : std::true_type {};

	template<typename T>
	constexpr void CheckRefPtr(const T&)
	{
		static_assert(IsRefPtr<T>::value, "No Ref Ptr!");
	}

	// Safe casting type primary for numbers
	template<typename T, typename N>
	constexpr T CastSafeFunc(N value)
	{
		static_assert(std::is_arithmetic_v<T> && std::is_arithmetic_v<N>, "Only for arithmetice Typs");
		if constexpr (std::is_same_v<T, N>)
		{
			return value;
		}
		else
		{
			T valueResult = static_cast<T>(value);
			RY_CORE_ASSERT(valueResult == value, "not the same Values!");
			N valueBackwards = static_cast<N>(valueResult);
			RY_CORE_ASSERT(valueBackwards == value, "not backward Competible!");

			return valueResult;
		}
	}


	inline std::filesystem::path GetPathAsGenaric(std::filesystem::path path)
	{
		std::filesystem::path pathLexically = path.lexically_normal();
		std::string pathGenericStr = pathLexically.generic_string();
		pathLexically = pathGenericStr;
		return pathLexically;
	}



	template<typename T>
	using Weak = std::weak_ptr<T>;


	template<typename T>
	struct IsWeakPtr : std::false_type {};

	template<typename T>
	struct IsWeakPtr<std::weak_ptr<T>> : std::true_type {};

	template<typename T>
	constexpr void CheckWeakPtr(const T&)
	{
		static_assert(IsWeakPtr<T>::value, "No Weak Ptr!");
	}
	template<typename T>
	constexpr bool CheckAllRefsVaild(Ref<T>& ref)
	{
		bool v = nullptr != ref;
		return v;
	}

	template<typename T, typename ... Args>
	constexpr bool CheckAllRefsVaild(Ref<T>& ref, Ref<Args>& ... args)
	{
		bool v = CheckAllRefsVaild(ref);

		if constexpr (sizeof...(args) > 0)
		{
			return v && CheckAllRefsVaild(args...);
		}
	}


	template<typename T>
	constexpr bool CheckAllVecsVaild(std::vector<T>& vec)
	{
		bool v = !vec.empty();
		return v;
	}

	template<typename T, typename ... Args>
	constexpr bool CheckAllVecsVaild(std::vector<T>& vec, std::vector<Args>& ... args)
	{
		bool v = CheckAllRefsVaild(vec);

		if constexpr (sizeof...(args) > 0)
		{
			return v && CheckAllRefsVaild(args...);
		}
	}

	constexpr std::size_t BinaryPresentionCount(std::uint64_t value)
	{
		if (value == 0ull)
			return 1ull;

		std::size_t bitCount = 0ull;
		do
		{
			value >>= 1;
			++bitCount;
		} while (value);

		return bitCount;
	}

	
	

};
#pragma region Hash_Structs_for_std_PtrObjects


namespace std {
#if 0
	template<typename T>
	struct hash<Rynex::Ref<T>>
	{
		std::size_t operator()(const Rynex::Ref<T>& ptrObject) const
		{
			if (nullptr == ptrObject)
			{
				std::string_view strView = typeid(T).name();
				RY_CORE_ASSERT(false, "Rynex::Ref<{}>(nullptr) Is not Exapteble!", strView.data());
				return 0ull;
			}
			T* ptrObject = ptrObject.get();
			int64_t ptrAdresse = (int64_t)ptrObject;
			uint64_t ptrUadresse = (uint64_t)ptrAdresse;
			return ptrUadresse;
		}
	};
#endif
	template<typename T>
	struct hash<Rynex::Weak<T>>
	{
		std::size_t operator()(const Rynex::Weak<T>& ptrObjectWeak) const
		{
			if (Rynex::Ref<T> ptrObjectRef = ptrObjectWeak.lock())
			{
				T* ptrObject = ptrObjectRef.get();
				uint64_t ptrAdresse = static_cast<uint64_t>(ptrObject);
				return ptrAdresse;
			}
			std::string_view strView = typeid(T).name();
			RY_CORE_ERROR("Failed Type: {}", strView);
			RY_CORE_ASSERT(false, "This Weak ptr has no reference object available!");
			return 0ull;
		}
	};
#if 0
	template<typename T>
	struct hash<Rynex::Scope<T>>
	{
		std::size_t operator()(const Rynex::Scope<T>& ptrObject) const
		{
			if (nullptr == ptrObject)
			{
				std::string_view strView = typeid(T).name();
				RY_CORE_ASSERT(false, "Rynex::Ref<{}>(nullptr) Is not Exapteble!", strView.data());
				return 0ull;
			}
			T* ptrObject = ptrObject.get();
			int64_t ptrAdresse = (int64_t)ptrObject;
			uint64_t ptrUadresse = (uint64_t)ptrAdresse;
			return ptrUadresse;
		}
	};
#endif
}

#pragma endregion

#define RY_DESTROY_REF(ref) \
	CheckRefPtr(ref); \
	if(nullptr != ref) \
	{ \
		ref.reset(); \
		ref = nullptr; \
	} \
	RY_CORE_ASSERT(nullptr == ref, "Referz not Deleted!")

#define RY_DESTROY_SCOPE(scope) \
	CheckScopePtr(scope); \
	if(nullptr != scope) \
	{ \
		scope.reset(); \
		scope = nullptr; \
	} \
	RY_CORE_ASSERT(nullptr == scope, "Referz not Deleted!")

#define RY_DESTROY_WEAK(weak) \
	CheckWeakPtr(weak); \
	if(nullptr != weak) \
	{ \
		weak.reset(); \
		weak = nullptr;\
	} \
	RY_CORE_ASSERT(nullptr == weak, "Referz not Deleted!")



