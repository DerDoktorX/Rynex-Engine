#include "rypch.h"
#include "WindowsWindow.h"

#include "Rynex/Events/Event.h"
#include "Rynex/Events/MouseEvent.h"
#include "Rynex/Events/KeyEvent.h"
#include "Rynex/Events/ApplicationEvent.h"

#include "Platform/OpenGL/OpenGLContext.h"

//#include <glad/glad.h>


namespace Rynex {


	static bool s_GEFWInitialzed = false;


	static void GLFWErrorCallBack(int error, const char* discription)
	{
		RY_PROFILE_FUNCTION();
		RY_CORE_ERROR("GLFW Error ({0}): {1} ", error, discription);
	}

	Scope<Window> Window::Create(const WindowProps& props)
	{
		RY_PROFILE_FUNCTION();
		return CreateScope<WindowsWindow>(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		RY_PROFILE_FUNCTION();
		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		RY_PROFILE_FUNCTION();
	}

	void WindowsWindow::Init(const WindowProps& props)
	{
		RY_PROFILE_FUNCTION();
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		RY_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);



		if (!s_GEFWInitialzed)
		{
			int success = glfwInit();
			RY_CORE_ASSERT(success, "Coud not inialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallBack);
			s_GEFWInitialzed = true;
		}
#ifdef RY_DEBUG
		m_Data.Title += { " Debug Mode" };
#elif RY_DIST
		m_Data.Title += { " Dist Mode" };
#elif RY_REALSE
		m_Data.Title += { " Realse Mode" };
#endif

		m_Window = glfwCreateWindow((int)m_Data.Width, (int)m_Data.Height, m_Data.Title.c_str(), nullptr, nullptr);

		m_Context = CreateRef<OpenGLContext>(m_Window);
		m_Context->Init();

		
		glfwSetWindowUserPointer(m_Window, &m_Data);
#ifdef RY_DEBUG
		SetVSync(true);
		RY_CORE_WARN("(SetVSync) is Executed -> true!");
#else
		SetVSync(false);
		RY_CORE_INFO("(SetVSync) is Executed -> false!");
#endif
		// Set GLFW callbackes

		
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int widthe, int height) 
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.Width = widthe;
				data.Height = height;

				WindowResizeEvent event(widthe, height);
				RY_CORE_WARN("{0}, {1}", widthe, height);
				data.EventCallback(event);
			});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) 
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				WindowCloseEvent event;
				data.EventCallback(event);
			});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scanecode, int actione, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (actione)
				{
					case GLFW_PRESS:
					{
						KeyPressedEvent event(key, 0);
						data.EventCallback(event);
						break;
					}
					case GLFW_RELEASE:
					{
						KeyReleasdEvent event(key);
						data.EventCallback(event);
						break;
					}
					case GLFW_REPEAT:
					{
						KeyPressedEvent event(key, 1);
						data.EventCallback(event);
						break;
					}
				}
			});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keyCode)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				KeyTypedEvent event(keyCode);
				data.EventCallback(event);

			});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int buttom, int actione,  int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (actione)
				{
					case GLFW_PRESS:
					{
						MouseButtenPressedEvent event(buttom);
						data.EventCallback(event);
						break;
					}
					case GLFW_RELEASE:
					{
						MouseButtenRealsedEvent event(buttom);
						data.EventCallback(event);
						break;
					}
				}
			});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseSrolledEvent event((float)xOffset, (float)yOffset);
				data.EventCallback(event);
			});
		
		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				MouseMoveEvent event((float)xPos, (float)yPos);
				data.EventCallback(event);


			});
		


	}

	void WindowsWindow::Shutdown()
	{
		RY_PROFILE_FUNCTION();
		glfwDestroyWindow(m_Window);
		m_Context.reset();
	}

	void WindowsWindow::OnUpdate()
	{
		RY_PROFILE_FUNCTION();
		glfwPollEvents();
		m_Context->SwapBuffers();
	}

	void WindowsWindow::SetVSync(bool enabled)
	{
		RY_PROFILE_FUNCTION();
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);
		
		m_Data.VSync = enabled;
	}

	bool WindowsWindow::IsVSync() const
	{
		RY_PROFILE_FUNCTION();
		return m_Data.VSync;
	}

	
}
