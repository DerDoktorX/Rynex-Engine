#include "rypch.h"
#include "WindowsWindow.h"

#include "Rynex/Events/Event.h"
#include "Rynex/Events/MouseEvent.h"
#include "Rynex/Events/KeyEvent.h"
#include "Rynex/Events/ApplicationEvent.h"

#include "Platform/OpenGL/OpenGLContext.h"
#include "Platform/OpenGL/OpenGLThreadContext.h"
//#include <glad/glad.h>


namespace Rynex {


	static bool s_GEFWInitialzed = false;


	static void GLFWErrorCallBack(int error, const char* discription)
	{
		RY_CORE_ERROR("GLFW Error ({0}): {1} ", error, discription);
	}

	

	Scope<Window> Window::Create(const WindowProps& props)
	{
		return CreateScope<WindowsWindow>(props);
	}


	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
	}

	Ref<ThreadContext> WindowsWindow::CreateThreadeContext()
	{
		Ref<ThreadContext> threadContext = CreateRef<OpenGLThreadContext>();
		threadContext->Create(this);
		return threadContext;
	}

	void WindowsWindow::Init(const WindowProps& props)
	{
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
		glfwSetCursorEnterCallback(m_Window, [](GLFWwindow* window, int entered)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				
				if (data.Hovered = entered == GLFW_TRUE)
				{
					WindowCurserEnterEvent event;
					data.EventCallback(event);
				}
				else if (data.Hovered = !(entered == GLFW_FALSE))
				{
					WindowCurserLeaveEvent event;
					data.EventCallback(event);
				}

			});

		glfwSetWindowPosCallback(m_Window, [](GLFWwindow* window, int xPos, int yPos)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.PosX = xPos;
				data.PosY = yPos;
				
				WindowMovedEvent event(xPos, yPos);
#if RY_CONSOLE_WINDOW_POSTION
				RY_CORE_WARN("Window Postion(x: {0}, y: {1})", xPos, yPos);
#endif

				data.EventCallback(event);

			});

		glfwSetWindowFocusCallback(m_Window, [](GLFWwindow* window, int focused)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				if(data.Focuse = focused == GLFW_TRUE)
				{
					WindowFocuseEvent event;
					data.EventCallback(event);
				}
				else if(data.Focuse = !(focused == GLFW_FALSE))
				{
					WindowLostFocuseEvent event;
					data.EventCallback(event);
				}
				
			});

		
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int widthe, int height) 
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.Width = widthe;
				data.Height = height;

				WindowResizeEvent event(widthe, height);
#if RY_CONSOLE_WINDOW_SIZE
				RY_CORE_WARN("Window Size( x: {0}, y: {1})", widthe, height);
#endif
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
				data.MousePosX = (float)xPos;
				data.MousePosY = (float)yPos;
#if RY_CONSOLE_MOUSE_POSTION
				RY_CORE_WARN("Mouse Postion(x: {0}, y: {1})", xPos, yPos);
#endif
				data.EventCallback(event);
			});
		
		RY_CORE_INFO("WindowsWindow::Init Succese!");

	}

	void WindowsWindow::Shutdown()
	{
		glfwDestroyWindow(m_Window);
		m_Context.reset();
	}

	void WindowsWindow::OnUpdate()
	{
		glfwPollEvents();
		m_Context->SwapBuffers();
	}

	void WindowsWindow::SetVSync(bool enabled)
	{
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);
		
		m_Data.VSync = enabled;
	}

	bool WindowsWindow::IsFocused() const
	{
		return m_Data.Focuse;
	}

	bool WindowsWindow::IsVSync() const
	{
		return m_Data.VSync;
	}

	
}
