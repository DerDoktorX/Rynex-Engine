#pragma once
#include "Rynex/Renderer/API/ThreadContext.h"

struct GLFWwindow;

namespace Rynex {

	class OpenGLThreadContext : public ThreadContext
	{
	public:
		OpenGLThreadContext();
		~OpenGLThreadContext();

		virtual void Init() override;
		virtual void Destroy() override;

		virtual const RendererAPI::API GetRendererAPI() const override;
		virtual void Create(Window* parent) override;

	private:
		GLFWwindow* m_ThreadHandle;
		std::chrono::time_point<std::chrono::steady_clock> m_BeginPoint;
	};

}
