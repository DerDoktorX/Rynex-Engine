#include <rypch.h>
#include "OpenGLThreadContext.h"


#include <Platform/OpenGL/OpenGLContext.h>
#include <Platform/OpenGL/OpenGLFence.h>

#include <Rynex/Core/Application.h>
#include <Rynex/Core/Window.h>

#include <GLFW/glfw3.h>
//SECOND
#define RY_ENABLE_LOADING_THREAD_FENCE          1
#define SECONDS(x)                              (x * 1000000000)
#define MILLIE_SECONDS(x)                      (x * 1000000)

namespace Rynex {


	OpenGLThreadContext::OpenGLThreadContext()
		: m_ThreadHandle(nullptr)
	{
	}

	OpenGLThreadContext::~OpenGLThreadContext()
	{
		Destroy();
	}


	void OpenGLThreadContext::Init()
	{
		RY_CORE_ASSERT(nullptr != m_ThreadHandle, "no Handle set!");

		m_BeginPoint = std::chrono::high_resolution_clock::now();
		glfwMakeContextCurrent(m_ThreadHandle);
		GLADloadproc gladLoadProcesFuncPtr = reinterpret_cast<GLADloadproc>(glfwGetProcAddress);
		int status = gladLoadGLLoader(gladLoadProcesFuncPtr);
		RY_CORE_ASSERT(status, "Failed to initialize Glad!");
	}



	


	void OpenGLThreadContext::Destroy()
	{
		if (!m_ThreadHandle)
		    return;

#if RY_ENABLE_LOADING_THREAD_FENCE
	    GLsync fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
#else
		OpenGLFence fence;
		fence.SetupFence();
#endif

		GLenum result;
		std::chrono::time_point<std::chrono::high_resolution_clock> startPoint = std::chrono::high_resolution_clock::now();

#if RY_ENABLE_LOADING_THREAD_FENCE
		uint32_t loopCount = 0;
		do
		{
			result = glClientWaitSync(fence, GL_SYNC_FLUSH_COMMANDS_BIT, SECONDS(1));
			loopCount++;
		} while (result != GL_ALREADY_SIGNALED && result != GL_CONDITION_SATISFIED);
#else
	    using namespace std::chrono_literals;
	    std::this_thread::sleep_for(10s);
#endif
	    std::chrono::time_point<std::chrono::high_resolution_clock> endePoint = std::chrono::high_resolution_clock::now();
	    int64_t pastTime = std::chrono::time_point_cast<std::chrono::microseconds>(endePoint).time_since_epoch().count()
	        - std::chrono::time_point_cast<std::chrono::microseconds>(startPoint).time_since_epoch().count();
	    int64_t pastThreadTime = std::chrono::time_point_cast<std::chrono::microseconds>(endePoint).time_since_epoch().count()
	        - std::chrono::time_point_cast<std::chrono::microseconds>(m_BeginPoint).time_since_epoch().count();

#if RY_ENABLE_LOADING_THREAD_FENCE
	    RY_CORE_FATAL("Finish Loading Thread: Loading Time {} Wait Time is {}, LoopCount {}", pastThreadTime, pastTime, loopCount);
	    glDeleteSync(fence);
#else
	    RY_CORE_FATAL("Finish Loading Thread: Loading Time {} Wait Time is {}", pastThreadTime, pastTime);
#endif
	    m_ThreadHandle = nullptr;

	}

	const RendererAPI::API OpenGLThreadContext::GetRendererAPI() const
	{
		return RendererAPI::API::OpenGL;
	}

	void OpenGLThreadContext::Create(Window* parent)
	{
		GLFWwindow* parentWindow = reinterpret_cast<GLFWwindow*>(parent->GetNativeWindow());
		RY_CORE_ASSERT(parentWindow, "parent null Ptr!");
#if 1
		int glfwResultInit = glfwInit();
		RY_CORE_ASSERT(GLFW_TRUE == glfwResultInit, "Init not success full!");
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

		constexpr const char* windowThreatName = "";
		constexpr int width = 1;
		constexpr int height = 1;
		constexpr GLFWmonitor* monitorPtr = nullptr;

		m_ThreadHandle = glfwCreateWindow(width, height, windowThreatName, monitorPtr, parentWindow);
		
		RY_CORE_ASSERT(m_ThreadHandle, "thread null Ptr!");
#else
		m_ThreadHandle = parentWindow;
#endif

	}

	bool OpenGLThreadContext::IsActive()
	{
		bool resultContextValid = nullptr != glfwGetCurrentContext();
		bool resultMainThread = OpenGLContext::Get().IsMainContextThread();
		if (resultContextValid != resultMainThread)
		{
			if (!resultMainThread)
				RY_CORE_WARN("Valid OpenGL Context but not main Thread!");
			else
				RY_CORE_FATAL("Currently On Main Thread but OpenGL Context is no longer valid!");
		}

		return resultContextValid;
	}
}
