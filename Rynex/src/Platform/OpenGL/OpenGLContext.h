#pragma once
#include "Rynex/Renderer/API/GraphicsContext.h"

struct GLFWwindow;

namespace Rynex{
	

	class OpenGLContext : public GraphicsContext
	{
	public:
		OpenGLContext(GLFWwindow* windowHandle);

		virtual void Init() override;
		virtual void SwapBuffers() override;

	private:
		GLFWwindow* m_WindowHandle;
	};

}
