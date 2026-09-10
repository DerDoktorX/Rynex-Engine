#pragma once
#include <Rynex/Renderer/API/ProtypeAPI.h>
#include <Platform/OpenGL/OpenGLThreadContext.h>
#include <Platform/OpenGL/OpenGLRenderCommand.h>



#include <glm/gtc/type_ptr.hpp>

#define RY_INTERNLE_GL_CHECK_CASE_LOG_FUNC(func, type) case type: { func(#type); break; }


#define RY_INTERNLE_GL_CHECK_CASE_ASSERT(type) case type: { RY_CORE_ASSERT(false, "OpenGL Error ->" #type ); break; }

#define RY_INTERNLE_GET_OPENGL_MACRO_CASE(rynexEnum, glMacro) case rynexEnum: return glMacro
#define RY_INTERNLE_GET_OPENGL_WARN_MACRO_CASE(rynexEnum, glMacro, ...) case rynexEnum: RY_CORE_WARN(__VA_ARGS__); return glMacro


#define RY_INTERNLE_GET_OPENGL_MACRO_DEFAULT(rynexEnumType)\
	default:\
		RY_CORE_ASSERT(false, "Not Defined " RY_STRINGIFY_MOAKRO(rynexEnumType) "Enum" RY_STRINGIFY_MOAKRO(__FUNCSIG__) );\
		return 0;

#define RY_GET_OPENGL_RESULT_INT(glMarco, ptr)\
{\
	GLint* dataPtr = ptr;\
	glGetIntegerv(glMarco, dataPtr)\
}
	

#define RY_IS_OPENGL_RESULT_INT(glMarco, result)\
{\
	GLint ptr = 0; \
	RY_GET_OPENGL_RESULT_INT(glMarco, &ptr); \
	RY_CORE_ASSERT(ptr == result, "{} is not OpenGL" #glMarco, ptr);\
}

#define GL_CHECK_BINDINGS_BEFOR_DRAW RY_ENABLE_GARFIC_API_DRAW_CALL_CHECKS

#define RY_DISABLE_OPENGL_SEPARETE_TEXTURE_SAMPLER 1

#define RY_PRINT_OPENGL_TEX_SPECS 0
#define RY_OPENGL_TEXTURE_LOG_FUNC 0

#define RY_CHECK_MULTYSAMPLE(samples) samples > 1



#ifdef RY_PARELE_GRAFIC_CONTEXT
#if 1

	#define RY_OPENGL_BUFFER_ID_SCOPE_LOCK()		std::scoped_lock<std::mutex> lockBufferID = std::scoped_lock<std::mutex>(::Rynex::OpenGLContext::GetBufferOperationLock())
	#define RY_OPENGL_TEXTURE_ID_SCOPE_LOCK() 		std::scoped_lock<std::mutex> lockTextureID = std::scoped_lock<std::mutex>(::Rynex::OpenGLContext::GetTextureOperationLock())
	#define RY_OPENGL_SHADER_ID_SCOPE_LOCK()		std::scoped_lock<std::mutex> lockShaderID = std::scoped_lock<std::mutex>(::Rynex::OpenGLContext::GetShaderOperationLock())
	#define RY_OPENGL_FRAME_BUFFER_ID_SCOPE_LOCK()	std::scoped_lock<std::mutex> lockFrambufferID = std::scoped_lock<std::mutex>(::Rynex::OpenGLContext::GetFrambufferOperationLock())
	#define RY_OPENGL_FANCE_ID_SCOPE_LOCK()			std::scoped_lock<std::mutex> lockFanceID = std::scoped_lock<std::mutex>(::Rynex::OpenGLContext::GetFanceOperationLock())
	#define RY_OPENGL_VERTEX_ARRAY_ID_SCOPE_LOCK()	std::scoped_lock<std::mutex> vertexArrayOperationID = std::scoped_lock<std::mutex>(::Rynex::OpenGLContext::GetVertexArrayOperationLock())	

	#define RY_OPENGL_ALL_ID_OPERTIONS_SCOPE_LOCK()	RY_OPENGL_BUFFER_ID_SCOPE_LOCK();		\
													RY_OPENGL_TEXTURE_ID_SCOPE_LOCK();		\
													RY_OPENGL_SHADER_ID_SCOPE_LOCK();		\
													RY_OPENGL_FRAME_BUFFER_ID_SCOPE_LOCK(); \
													RY_OPENGL_FANCE_ID_SCOPE_LOCK();		\
													RY_OPENGL_VERTEX_ARRAY_ID_SCOPE_LOCK()
#elif 1
	#define RY_OPENGL_BUFFER_ID_SCOPE_LOCK()
	#define RY_OPENGL_TEXTURE_ID_SCOPE_LOCK()
	#define RY_OPENGL_SHADER_ID_SCOPE_LOCK()
	#define RY_OPENGL_FRAME_BUFFER_ID_SCOPE_LOCK()
	#define RY_OPENGL_FANCE_ID_SCOPE_LOCK()
	#define RY_OPENGL_VERTEX_ARRAY_ID_SCOPE_LOCK()
	
	#define RY_OPENGL_ALL_ID_OPERTIONS_SCOPE_LOCK()

#else	


	#define RY_OPENGL_ALL_ID_OPERTIONS_SCOPE_LOCK()	std::scoped_lock<std::mutex> lockBufferID = std::scoped_lock<std::mutex>(::Rynex::OpenGLContext::GetBufferOperationLock())	

	#define RY_OPENGL_BUFFER_ID_SCOPE_LOCK()		RY_OPENGL_ALL_ID_OPERTIONS_SCOPE_LOCK()
	#define RY_OPENGL_TEXTURE_ID_SCOPE_LOCK() 		RY_OPENGL_ALL_ID_OPERTIONS_SCOPE_LOCK()
	#define RY_OPENGL_SHADER_ID_SCOPE_LOCK()		RY_OPENGL_ALL_ID_OPERTIONS_SCOPE_LOCK()
	#define RY_OPENGL_FRAME_BUFFER_ID_SCOPE_LOCK()	RY_OPENGL_ALL_ID_OPERTIONS_SCOPE_LOCK()
	#define RY_OPENGL_FANCE_ID_SCOPE_LOCK()			RY_OPENGL_ALL_ID_OPERTIONS_SCOPE_LOCK()
	#define RY_OPENGL_VERTEX_ARRAY_ID_SCOPE_LOCK()	RY_OPENGL_ALL_ID_OPERTIONS_SCOPE_LOCK()
#endif

#if 0
	#define RY_EXE_ON_MAIN_THREAD_FUNCTION(func, ...)		::OpenGL::ExecuteFunctionOnLocaleThread( RY_BIND_MEMBER_FN(::Rynex::##func, __VA_ARGS__), #func)
	#define RY_EXE_ON_MAIN_THREAD_RESUME(func, ...)			if(::OpenGL::ExecuteResumeOnLocaleThread( RY_BIND_MEMBER_FN(::Rynex::##func, __VA_ARGS__) , #func)) return
#else
	#define RY_EXE_ON_MAIN_THREAD_FUNCTION(func, ...)	::OpenGL::CheckForAktivContextFunktion(RY_BIND_MEMBER_FN(::Rynex::##func, __VA_ARGS__), #func)
	#define RY_EXE_ON_MAIN_THREAD_RESUME(func, ...)		::OpenGL::CheckForAktivContextResume(RY_BIND_MEMBER_FN(::Rynex::##func, __VA_ARGS__), #func)
#endif

#else
	#define RY_OPENGL_BUFFER_ID_SCOPE_LOCK()
	#define RY_OPENGL_TEXTURE_ID_SCOPE_LOCK()
	#define RY_OPENGL_SHADER_ID_SCOPE_LOCK()
	#define RY_OPENGL_FRAME_BUFFER_ID_SCOPE_LOCK()
	#define RY_OPENGL_FANCE_ID_SCOPE_LOCK()
	#define RY_OPENGL_VERTEX_ARRAY_ID_SCOPE_LOCK()

	#define RY_OPENGL_ALL_ID_OPERTIONS_SCOPE_LOCK()


	#define RY_EXE_ON_MAIN_THREAD_FUNCTION(func)		::OpenGL::ExecuteFunctionOnMainThread( RY_BIND_MEMBER_FN(::Rynex:: func), #func)
	
	#define RY_EXE_ON_MAIN_THREAD_RESUME_ARGS(func, ...)\
		if(\
			::OpenGL::ExecuteResumeOnMainThread( \
				RY_BIND_MEMBER_FN_ARGS(::Rynex:: func, __VA_ARGS__ ) \
				, #func \
			) \
		) \
			return

	#define RY_EXE_ON_MAIN_THREAD_RESUME(func)\
		if(\
			::OpenGL::ExecuteResumeOnMainThread( \
				RY_BIND_MEMBER_FN(::Rynex:: func ) \
				, #func \
			) \
		) \
			return

#endif


namespace OpenGL {
	
	void ExecuteFunctionOnMainThread(const std::function<void()>& func, const char* name);
	bool ExecuteResumeOnMainThread(const std::function<void()>& func, const char* name);

	void CheckForAktivContextFunktion(const std::function<void()>& func, const char* name);
	void CheckForAktivContextResume(const std::function<void()>& func, const char* name);

	void ExecuteFunctionOnLocaleThread(const std::function<void()>& func, const char* name);
	bool ExecuteResumeOnLocaleThread(const std::function<void()>& func, const char* name);

	glm::uvec2 GetMainWindowCurentSize();


	void OnError(std::string typeStr);

	void OnDebugeSource(std::string sourceStr);

	void OnDebugeType(std::string typeStr);
	void OnDebugeSeverity(std::string severity);



	void APIENTRY DebugeOutPut(uint32_t source, uint32_t type, uint32_t id, uint32_t severity, int32_t length, const char* message, const void* userParam);
	void ErrorMassageCallback(uint32_t source, uint32_t type, int32_t id, uint32_t severity, int32_t length, const char* message, const void* userParam);
	void CheckError();

	std::string GetTargetStr(uint32_t t);
	std::string GetFomateInternelStr2(uint32_t f);
	std::string GetTextureFomateInternelStr(uint32_t inFo);
	std::string GetTextureFomateStr(uint32_t f);

	uint32_t GetBuffersTarget(Rynex::BufferType target);
	uint32_t GetFlagsFromFlagTypes(Rynex::BufferFlagGPU flag);


}

#if RY_ENABLE_GARFIC_API_CHECKS 
	#define GL_CHECK() ::OpenGL::CheckError()
#else
	#define GL_CHECK()
#endif

#if RY_ENABLE_GARFIC_API_CHECKS_LOOP
	#define GL_CHECK_LOOP() ::OpenGL::CheckError()
#else
	#define GL_CHECK_LOOP()
#endif