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

	OpenGLContext::~OpenGLContext()
	{
		m_WindowHandle = nullptr;
	}

	void OpenGLContext::Init()
	{
		RY_PROFILE_FUNCTION();
		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

		RY_CORE_ASSERT(status , "Faild to instalize Glad!");
		const GLubyte* glVersion = glGetString(GL_VERSION);
		std::string_view version(reinterpret_cast<const char*>(glVersion));
		RY_CORE_INFO("OpenGL version: {}", version.data());
	}

	void OpenGLContext::SwapBuffers()
	{
		RY_PROFILE_FUNCTION();
		glfwSwapBuffers(m_WindowHandle);
	}

}