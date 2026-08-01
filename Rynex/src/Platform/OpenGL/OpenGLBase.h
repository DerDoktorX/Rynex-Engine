#pragma once
#include <rypch.h>
#include <Rynex/Core/Application.h>

#include <Rynex/Renderer/API/Buffer.h>

#include <Platform/OpenGL/OpenGLContext.h>
#include <Platform/OpenGL/OpenGLThreadContext.h>
#include <Platform/OpenGL/OpenGLRenderCommand.h>

#include <Platform/OpenGL/OpenGLArrayBuffer.h>
#include <Platform/OpenGL/OpenGLShaderStorageBuffer.h>

#include <Platform/OpenGL/OpenGLTextureStorage.h>


#include <magic_enum.hpp>

#include <glad/glad.h>
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


	#define RY_EXE_ON_MAIN_THREAD_FUNCTION(func, ...)		::OpenGL::ExecuteFunctionOnMainThread( RY_BIND_MEMBER_FN(::Rynex::##func, __VA_ARGS__), #func)
	#define RY_EXE_ON_MAIN_THREAD_RESUME(func, ...)			if(::OpenGL::ExecuteResumeOnMainThread( RY_BIND_MEMBER_FN(::Rynex::##func, __VA_ARGS__) , #func)) return

#endif


namespace OpenGL {

	inline static void ExecuteFunctionOnMainThread(const std::function<void()>& func, const char* name)
	{
		if (Rynex::OpenGLThreadContext::IsActive())
		{
			func();
			return;
		}

#if RY_GRAFIC_SUBMIT_TO_MAIN_THREAD_WITHE_OUT_WAIT
		RY_CORE_INFO("From Parel Thread Submite Func ({}) to exexute on main thread! executing", name);
		Rynex::Application::Get().SubmiteToMainThreedQueue(func);
#else
		RY_CORE_INFO("From Parel Thread Submite Func ({}) to exexute on main thread! Waiting", name);
		Rynex::Application::Get().SubmiteToMainThreedQueueWait(func);
#endif	
	}

	inline static void CheckForAktivContextFunktion(const std::function<void()>& func, const char* name)
	{
		if (Rynex::OpenGLThreadContext::IsActive())
		{
			RY_CORE_INFO("This Parel Thread has a Aktiv OpenGL Context {}, exute now {}", Rynex::ThreadPool::GetCurentThreadIDstr(), name);
			func();	

			return;
		}
		RY_CORE_FATAL("This Parel Thread Has No Aktiv OpenGL Context {}, wil try to {}", Rynex::ThreadPool::GetCurentThreadIDstr(), name);
		
	}

	inline static void CheckForAktivContextResume(const std::function<void()>& func, const char* name)
	{
		if (Rynex::OpenGLThreadContext::IsActive())
		{
			RY_CORE_INFO("This Parel Thread has a Aktiv OpenGL Context {}, exute now {}", Rynex::ThreadPool::GetCurentThreadIDstr(), name);
			return;
		}
		RY_CORE_FATAL("This Parel Thread Has No Aktiv OpenGL Context {}, wil try to {}", Rynex::ThreadPool::GetCurentThreadIDstr(), name);

	}

	inline static void ExecuteFunctionOnLocaleThread(const std::function<void()>& func, const char* name)
	{
		Rynex::OpenGLThreadContext threadContext;
		Rynex::Application& app = Rynex::Application::Get();
		if(!app.IsRunninig())
			return;
		
		if (Rynex::OpenGLThreadContext::IsActive())
		{
			func();
			return;
		}

		
		func();
		RY_CORE_INFO("From Parel Thread Submite Func ({}) to exexute on main thread! Waiting", name);
	}

	inline static bool ExecuteResumeOnMainThread(const std::function<void()>& func, const char* name)
	{
		if (Rynex::OpenGLThreadContext::IsActive())
			return false;

#if RY_GRAFIC_SUBMIT_TO_MAIN_THREAD_WITHE_OUT_WAIT
		RY_CORE_INFO("From Parel Thread Submite Func ({}) to exexute on main thread! executing", name);
		Rynex::Application::Get().SubmiteToMainThreedQueue(func);
#else
		RY_CORE_INFO("From Parel Thread Submite Func ({}) to exexute on main thread! Waiting", name);
		Rynex::Application::Get().SubmiteToMainThreedQueueWait(func);
#endif
		return true;
	}


	inline static bool ExecuteResumeOnLocaleThread(const std::function<void()>& func, const char* name)
	{
		Rynex::Application& app = Rynex::Application::Get();
		if (!app.IsRunninig())
			return true;

		if (Rynex::OpenGLThreadContext::IsActive())
			return false;

	
		func();
		RY_CORE_INFO("From Parel Thread Submite Func ({}) to exexute on main thread! Waiting", name);
		return true;
	}

	inline static glm::uvec2 GetMainWindowCurentSize()
	{
		Rynex::Application& app = Rynex::Application::Get();
		Rynex::Window& window = app.GetWindow();
		glm::uvec2 size = glm::uvec2{ 
			window.GetWidth(), 
			window.GetHeight() 
		};
		return size;
	}

	static void OnError(std::string typeStr)
	{
		RY_CORE_FATAL("OpenGL Error -> {} ", typeStr);
		RY_DEBUG_BREAK();
	}

	static void OnDebugeSource(std::string sourceStr)
	{
		RY_CORE_INFO("GL TYPE: ** {} **", sourceStr);
	}

	static void OnDebugeType(std::string typeStr)
	{
		RY_CORE_INFO("GL SOURCE: ** {} **", typeStr);
	}

	static void OnDebugeSeverity(std::string severity)
	{
		RY_CORE_INFO("GL SEVERITY: ** {} **", severity);
	}



	static void APIENTRY DebugeOutPut(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam)
	{
#if !RY_ENABLE_GARFIC_API_DBUGE_DETAILD_MASSEGES
		if (id == 131169u || id == 131185u || id == 131218 || id == 131204)
			return;
#endif

		RY_CORE_TRACE("OpenGL Debug Message(H: {:x}/D: {:L}): {}", id, id, message);
		switch (source)
		{
			RY_INTERNLE_GL_CHECK_CASE_LOG_FUNC(OnDebugeSource, GL_DEBUG_SOURCE_API);
			RY_INTERNLE_GL_CHECK_CASE_LOG_FUNC(OnDebugeSource, GL_DEBUG_SOURCE_WINDOW_SYSTEM);
			RY_INTERNLE_GL_CHECK_CASE_LOG_FUNC(OnDebugeSource, GL_DEBUG_SOURCE_SHADER_COMPILER);
			RY_INTERNLE_GL_CHECK_CASE_LOG_FUNC(OnDebugeSource, GL_DEBUG_SOURCE_THIRD_PARTY);
			RY_INTERNLE_GL_CHECK_CASE_LOG_FUNC(OnDebugeSource, GL_DEBUG_SOURCE_APPLICATION);
			RY_INTERNLE_GL_CHECK_CASE_LOG_FUNC(OnDebugeSource, GL_DEBUG_SOURCE_OTHER);
		default:
			RY_CORE_ASSERT(false,"Not Defnind OpenGL debug source!");
			break;
		}

		switch (type)
		{
			RY_INTERNLE_GL_CHECK_CASE_LOG_FUNC(OnDebugeType, GL_DEBUG_TYPE_ERROR);
			RY_INTERNLE_GL_CHECK_CASE_LOG_FUNC(OnDebugeType, GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR);
			RY_INTERNLE_GL_CHECK_CASE_LOG_FUNC(OnDebugeType, GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR);
			RY_INTERNLE_GL_CHECK_CASE_LOG_FUNC(OnDebugeType, GL_DEBUG_TYPE_PORTABILITY);
			RY_INTERNLE_GL_CHECK_CASE_LOG_FUNC(OnDebugeType, GL_DEBUG_TYPE_PERFORMANCE);
			RY_INTERNLE_GL_CHECK_CASE_LOG_FUNC(OnDebugeType, GL_DEBUG_TYPE_MARKER);
			RY_INTERNLE_GL_CHECK_CASE_LOG_FUNC(OnDebugeType, GL_DEBUG_TYPE_PUSH_GROUP);
			RY_INTERNLE_GL_CHECK_CASE_LOG_FUNC(OnDebugeType, GL_DEBUG_TYPE_POP_GROUP);
			RY_INTERNLE_GL_CHECK_CASE_LOG_FUNC(OnDebugeType, GL_DEBUG_TYPE_OTHER);
		default:
			RY_CORE_ASSERT(false, "Not Defnind OpenGL debug type!");
			break;
		}

		switch (severity)
		{
			RY_INTERNLE_GL_CHECK_CASE_LOG_FUNC(OnDebugeSeverity, GL_DEBUG_SEVERITY_HIGH);
			RY_INTERNLE_GL_CHECK_CASE_LOG_FUNC(OnDebugeSeverity, GL_DEBUG_SEVERITY_LOW);
			RY_INTERNLE_GL_CHECK_CASE_LOG_FUNC(OnDebugeSeverity, GL_DEBUG_SEVERITY_MEDIUM);
			RY_INTERNLE_GL_CHECK_CASE_LOG_FUNC(OnDebugeSeverity, GL_DEBUG_SEVERITY_NOTIFICATION);
		default:
			RY_CORE_ASSERT(false, "Not Defnind OpenGL debug severity!");
			break;
		}
#if RY_ENABLE_GARFIC_API_DEBUG_MEASGE_DEBUG_BREAK
		if(severity != GL_DEBUG_SEVERITY_LOW || type != GL_DEBUG_TYPE_OTHER || source != GL_DEBUG_SOURCE_API)
			RY_DEBUG_BREAK();
#elif RY_ENABLE_GARFIC_API_DEBUG_MEASGE_DEBUG_BREAK_PERFORMANCE_TYPE
		if (GL_DEBUG_TYPE_PERFORMANCE == type)
			RY_DEBUG_BREAK();
#elif RY_ENABLE_GARFIC_API_DEBUG_MEASGE_DEBUG_BREAK_ERROR_TYPE
		if (GL_DEBUG_TYPE_ERROR == type)
			RY_DEBUG_BREAK();
#endif
	}

	static void ErrorMassageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
	{

		if (type == GL_DEBUG_TYPE_ERROR)
		{
			RY_CORE_FATAL("GL CALLBACK: ** GL ERROR ** message = {}", message);
		}
#if RY_ENABLE_GRIFIC_API_WARN_MASGES
		else
		{
			RY_CORE_WARN("GL CALLBACK: ** GL WARN **  message = {}", message);
		}
		RY_CORE_TRACE("GL CALLBACK: ** Info ++ message = {}", message);
#endif



	}

	static void CheckError()
	{
		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR)
		{
			switch (err)
			{
				RY_INTERNLE_GL_CHECK_CASE_LOG_FUNC(OnError, GL_INVALID_ENUM);
				RY_INTERNLE_GL_CHECK_CASE_LOG_FUNC(OnError, GL_INVALID_VALUE);
				RY_INTERNLE_GL_CHECK_CASE_LOG_FUNC(OnError, GL_INVALID_OPERATION);
				RY_INTERNLE_GL_CHECK_CASE_LOG_FUNC(OnError, GL_STACK_OVERFLOW);
				RY_INTERNLE_GL_CHECK_CASE_LOG_FUNC(OnError, GL_STACK_UNDERFLOW);
				RY_INTERNLE_GL_CHECK_CASE_LOG_FUNC(OnError, GL_OUT_OF_MEMORY);
				RY_INTERNLE_GL_CHECK_CASE_LOG_FUNC(OnError, GL_INVALID_FRAMEBUFFER_OPERATION);
				RY_INTERNLE_GL_CHECK_CASE_LOG_FUNC(OnError, GL_CONTEXT_LOST);
				RY_INTERNLE_GL_CHECK_CASE_LOG_FUNC(OnError, GL_CONTEXT_FLAG_NO_ERROR_BIT);
			default:
				RY_CORE_ASSERT(false, "Not Defnind OpenGL Error!");
				break;
			}
		}
	}

	static std::string GetTargetStr(uint32_t t)
	{
		switch (t)
		{
			RY_CASE_STR_MACRO(GL_TEXTURE_1D);
			RY_CASE_STR_MACRO(GL_TEXTURE_2D);
			RY_CASE_STR_MACRO(GL_TEXTURE_2D_MULTISAMPLE);
			RY_CASE_STR_MACRO(GL_TEXTURE_3D);
			RY_CASE_STR_MACRO(GL_TEXTURE_RECTANGLE);
			RY_CASE_STR_MACRO(GL_TEXTURE_CUBE_MAP);
			RY_CASE_STR_MACRO(GL_TEXTURE_1D_ARRAY);
			RY_CASE_STR_MACRO(GL_TEXTURE_2D_ARRAY);
			RY_CASE_STR_MACRO(GL_TEXTURE_CUBE_MAP_ARRAY);
			RY_CASE_STR_MACRO(GL_TEXTURE_2D_MULTISAMPLE_ARRAY);

		default:
			RY_CORE_ASSERT(false, "Not Defined OpenGL Texture Target");
			break;
		}
		return "";
	}

	static std::string GetFomateInternelStr2(uint32_t f)
	{
		switch (f)
		{
			RY_CASE_STR_MACRO(GL_R8);
			RY_CASE_STR_MACRO(GL_RG8);

			RY_CASE_STR_MACRO(GL_RGB8);
			RY_CASE_STR_MACRO(GL_RGB16F);
			RY_CASE_STR_MACRO(GL_RGB32F);
			RY_CASE_STR_MACRO(GL_RGBA8);
			RY_CASE_STR_MACRO(GL_RGBA16F);
			RY_CASE_STR_MACRO(GL_RGBA32F);


			RY_CASE_STR_MACRO(GL_SRGB8);
			RY_CASE_STR_MACRO(GL_SRGB8_ALPHA8);

			RY_CASE_STR_MACRO(GL_R32I);
			RY_CASE_STR_MACRO(GL_DEPTH_COMPONENT);
			RY_CASE_STR_MACRO(GL_DEPTH_COMPONENT16);
			RY_CASE_STR_MACRO(GL_DEPTH_COMPONENT24);
			RY_CASE_STR_MACRO(GL_DEPTH_COMPONENT32);
			RY_CASE_STR_MACRO(GL_DEPTH_COMPONENT32F);
			RY_CASE_STR_MACRO(GL_DEPTH24_STENCIL8);
			RY_CASE_STR_MACRO(GL_DEPTH32F_STENCIL8);
		default:
			RY_CORE_ASSERT(false, "Not Defined OpenGL Texture Format");
			break;
		}
		return "";
	}

	static std::string GetTextureFomateInternelStr(uint32_t inFo)
	{
		switch (inFo)
		{
			RY_CASE_STR_MACRO(GL_R8);
			RY_CASE_STR_MACRO(GL_RG8);

			RY_CASE_STR_MACRO(GL_RGB8);
			RY_CASE_STR_MACRO(GL_RGB16F);
			RY_CASE_STR_MACRO(GL_RGB32F);
			RY_CASE_STR_MACRO(GL_RGBA8);
			RY_CASE_STR_MACRO(GL_RGBA16F);
			RY_CASE_STR_MACRO(GL_RGBA32F);

			RY_CASE_STR_MACRO(GL_SRGB8);
			RY_CASE_STR_MACRO(GL_SRGB8_ALPHA8);

			RY_CASE_STR_MACRO(GL_R32I);
			RY_CASE_STR_MACRO(GL_DEPTH_COMPONENT);
			RY_CASE_STR_MACRO(GL_DEPTH_COMPONENT16);
			RY_CASE_STR_MACRO(GL_DEPTH_COMPONENT24);
			RY_CASE_STR_MACRO(GL_DEPTH_COMPONENT32);
			RY_CASE_STR_MACRO(GL_DEPTH_COMPONENT32F);
			RY_CASE_STR_MACRO(GL_DEPTH24_STENCIL8);
			RY_CASE_STR_MACRO(GL_DEPTH32F_STENCIL8);
		default:
			RY_CORE_ASSERT(false, "Not Defined OpenGL Texture Format");
			break;
		}
		return "";
	}

	static std::string GetTextureFomateStr(uint32_t f)
	{
		switch (f)
		{
			RY_CASE_STR_MACRO(GL_RED);
			RY_CASE_STR_MACRO(GL_RG);

			RY_CASE_STR_MACRO(GL_RGB);
			RY_CASE_STR_MACRO(GL_SRGB);

			RY_CASE_STR_MACRO(GL_RGBA);
			RY_CASE_STR_MACRO(GL_SRGB_ALPHA);

			RY_CASE_STR_MACRO(GL_RED_INTEGER);
			RY_CASE_STR_MACRO(GL_DEPTH_COMPONENT);
			RY_CASE_STR_MACRO(GL_DEPTH_STENCIL);
		default:
			RY_CORE_ASSERT(false, "Not Defined OpenGL Texture Format");
			break;
		}
		return "";
	}

	static GLenum GetBuffersTarget(Rynex::BufferType target)
	{
		switch (target)
		{
			RY_INTERNLE_GET_OPENGL_MACRO_CASE(Rynex::BufferType::ShaderStorage, GL_SHADER_STORAGE_BUFFER);
			RY_INTERNLE_GET_OPENGL_MACRO_CASE(Rynex::BufferType::Vertex, GL_ARRAY_BUFFER);
			RY_INTERNLE_GET_OPENGL_MACRO_CASE(Rynex::BufferType::DrawIndrirect, GL_DRAW_INDIRECT_BUFFER);
			RY_INTERNLE_GET_OPENGL_MACRO_CASE(Rynex::BufferType::Uniform, GL_UNIFORM_BUFFER);
			case Rynex::BufferType::None:
			RY_INTERNLE_GET_OPENGL_MACRO_DEFAULT(Rynex::BufferType);
		}
		return 0;
	}

	static GLenum GetFlagsFromFlagTypes(Rynex::BufferFlagGPU flag)
	{
#if 0
		switch (flag)
		{
			RY_INTERNLE_GET_OPENGL_MACRO_CASE(Rynex::BufferFlag::None, 0u);
			RY_INTERNLE_GET_OPENGL_MACRO_CASE(Rynex::BufferFlag::Dynamic, GL_DYNAMIC_STORAGE_BIT);
			RY_INTERNLE_GET_OPENGL_MACRO_CASE(Rynex::BufferFlag::Read, GL_MAP_READ_BIT);
			RY_INTERNLE_GET_OPENGL_MACRO_CASE(Rynex::BufferFlag::Write, GL_MAP_WRITE_BIT);

			RY_INTERNLE_GET_OPENGL_MACRO_CASE(Rynex::BufferFlag::Presistent, GL_MAP_PERSISTENT_BIT);
			RY_INTERNLE_GET_OPENGL_MACRO_CASE(Rynex::BufferFlag::Coherent, GL_MAP_COHERENT_BIT);
			RY_INTERNLE_GET_OPENGL_MACRO_CASE(Rynex::BufferFlag::Client, GL_CLIENT_STORAGE_BIT);



			RY_INTERNLE_GET_OPENGL_MACRO_DEFAULT();
		}
#else
		uint8_t enumValue = 0;

		if(flag == enumValue)
			return 0u;
		GLenum result = 0u;
		constexpr uint8_t count = Rynex::BufferFlag::s_Count;
		
		for (uint8_t i = 1; i < count; i++)
		{
			enumValue = BIT(i);

			if (BIT_EQUAL(flag, enumValue))
			{
				Rynex::BufferFlag::BufferFlagBit bufferFlagBit = static_cast<Rynex::BufferFlag::BufferFlagBit>(enumValue);
				switch (enumValue)
				{
				case Rynex::BufferFlag::BufferFlagBit::Dynamic: 
				{
					result |= GL_DYNAMIC_STORAGE_BIT;
					break;
				}
				case Rynex::BufferFlag::BufferFlagBit::Read: 
				{
					result |= GL_MAP_READ_BIT;
					break;
				}
				case Rynex::BufferFlag::BufferFlagBit::Write: 
				{
					result |= GL_MAP_WRITE_BIT;
					break;
				}
				case Rynex::BufferFlag::BufferFlagBit::Presistent: 
				{
					result |= GL_MAP_PERSISTENT_BIT;
					break;
				}
				case Rynex::BufferFlag::BufferFlagBit::Coherent: 
				{
					result |= GL_MAP_COHERENT_BIT;
					break;
				}
				case Rynex::BufferFlag::BufferFlagBit::Client:
				{
					result |= GL_CLIENT_STORAGE_BIT;
					break;
				}
				default:
					RY_CORE_ERROR("value is not defnied: [{}] = {} ", i, enumValue);
					RY_CORE_ASSERT(false);
					break;
				}
			}
		}
#endif
		return result;
	}

#if 0
	class ObjectCount
	{
	public:
		ObjectCount()
			: m_Count(0ull)
			, m_Create(0ull)
			, m_Destroy(0ull)
		{
		}
		~ObjectCount()
		{
			m_Mutex.lock();
			::Rynex::Ref<::spdlog::logger> log = ::Rynex::Log::GetCoreLogger();
			if (log != nullptr)
			{
				log->info("Last OpenGL Buffer Destroyed! Create {}, Destroyed {}", m_Create, m_Destroy);
			}
			m_Mutex.unlock();
		}

		uint64_t GetCurentCount() 
		{
			std::scoped_lock<std::mutex> lock(m_Mutex);
			uint64_t count = m_Count;
			return count;
		}

		bool IsCountZero()
		{
			return GetCurentCount() != 0ull;
		}
		
		void Inroment()
		{
			std::scoped_lock<std::mutex> lock(m_Mutex);
			m_Count++;
			m_Create++;
		}
		

		void Decrement()
		{
			std::scoped_lock<std::mutex> lock(m_Mutex);
			m_Count--;
			m_Destroy++;
		}
	private:
		std::mutex m_Mutex;
		uint64_t m_Count;

		uint64_t m_Create;
		uint64_t m_Destroy;

		
	};

	class OpenGLObject
	{
	public:
		OpenGLObject()
		{
			if (nullptr == sm_Objects)
				sm_Objects = ::Rynex::CreateRef<ObjectCount>();
			sm_Objects->Inroment();
		}

		~OpenGLObject()
		{
			if (nullptr != sm_Objects && sm_Objects->IsCountZero())
				sm_Objects.reset();
		}
	private:

		static ::Rynex::Ref<ObjectCount> sm_Objects;
	};
#endif

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