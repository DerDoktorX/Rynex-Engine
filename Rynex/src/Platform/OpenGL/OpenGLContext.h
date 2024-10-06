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

		virtual const RendererAPI::API GetRendererAPI() const override { return RendererAPI::API::OpenGL; };


	private:
		GLFWwindow* m_WindowHandle;
	};

}
