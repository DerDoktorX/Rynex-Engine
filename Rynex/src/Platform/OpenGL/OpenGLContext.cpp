#include "rypch.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <gl/GL.h>

namespace Rynex {
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle)
	{
		RY_PROFILE_FUNCTION();
		RY_CORE_ASSERT(windowHandle, "Window hale is null!")
	}

	void OpenGLContext::Init()
	{
		RY_PROFILE_FUNCTION();
		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		RY_CORE_ASSERT(status, "Faild to instalize Glad!");
	}

	void OpenGLContext::SwapBuffers()
	{
		RY_PROFILE_FUNCTION();
		glfwSwapBuffers(m_WindowHandle);
	}

}