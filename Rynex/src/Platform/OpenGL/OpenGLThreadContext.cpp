#include "rypch.h"
#include "OpenGLThreadContext.h"
#include "Rynex/Core/Window.h"


#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <gl/GL.h>

namespace Rynex {
#define RY_ANABLE_LODING_THREADE_FANCE 1
#define SECOUNDS(x) (x * 1000000000)
#define MILI_SECOUNDS(x) (x * 1000000)

	OpenGLThreadContext::OpenGLThreadContext()
	{
		
		
	}

	OpenGLThreadContext::~OpenGLThreadContext()
	{
		Destroy();
	}

	void OpenGLThreadContext::Init()
	{
		m_BeginPoint = std::chrono::high_resolution_clock::now();
		glfwMakeContextCurrent(m_ThreadHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		RY_CORE_ASSERT(status, "Faild to instalize Glad!");
	}

	void OpenGLThreadContext::Destroy()
	{
		if (m_ThreadHandle)
		{

#if RY_ANABLE_LODING_THREADE_FANCE
			GLsync fance = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
#endif

			GLenum result;
			std::chrono::time_point<std::chrono::steady_clock> startPoint = std::chrono::high_resolution_clock::now();

#if RY_ANABLE_LODING_THREADE_FANCE
			uint32_t loopCount = 0;
			do
			{
				result = glClientWaitSync(fance, GL_SYNC_FLUSH_COMMANDS_BIT, SECOUNDS(1));
				loopCount++;
			} while (result != GL_ALREADY_SIGNALED && result != GL_CONDITION_SATISFIED);
#else

			using namespace std::chrono_literals;
			std::this_thread::sleep_for(10s);
#endif
			std::chrono::time_point<std::chrono::steady_clock> endePoint = std::chrono::high_resolution_clock::now();
			int64_t pastTime = std::chrono::time_point_cast<std::chrono::microseconds>(endePoint).time_since_epoch().count()
				- std::chrono::time_point_cast<std::chrono::microseconds>(startPoint).time_since_epoch().count();
			int64_t pastThreadTime = std::chrono::time_point_cast<std::chrono::microseconds>(endePoint).time_since_epoch().count()
				- std::chrono::time_point_cast<std::chrono::microseconds>(m_BeginPoint).time_since_epoch().count();

#if RY_ANABLE_LODING_THREADE_FANCE
			RY_CORE_FATAL("Finish Loading Threade: Loding Time {} Wait Time is {}, LoopCount {}", pastThreadTime, pastTime, loopCount);
			glDeleteSync(fance);
#else
			RY_CORE_FATAL("Finish Loading Threade: Loding Time {} Wait Time is {}", pastThreadTime, pastTime);
#endif
#if 1
			// glFinish();
			m_ThreadHandle = nullptr;
#endif
		}
	}

	const RendererAPI::API OpenGLThreadContext::GetRendererAPI() const
	{
		return RendererAPI::API();
	}

	void OpenGLThreadContext::Create(Window* parent)
	{
		GLFWwindow* parentWindow = (GLFWwindow*)parent->GetNativeWindow();
#if 1
		// glfwSetErrorCallback(GLFWErrorCallBack);
		RY_CORE_ASSERT(glfwInit(), "Init not sucesfull!");
		RY_CORE_ASSERT(parentWindow, "parent null Ptr!");
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

		m_ThreadHandle = glfwCreateWindow(1, 1, "", nullptr, parentWindow);
		RY_CORE_ASSERT(m_ThreadHandle, "thread null Ptr!");
#else
		m_ThreadHandle = parentWindow;
#endif

	}

	bool OpenGLThreadContext::IsActive()
	{
		return glfwGetCurrentContext() != nullptr && Asset::CurrentOnMainThread();
	}
}