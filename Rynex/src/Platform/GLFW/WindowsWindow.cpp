#include <rypch.h>
#include "WindowsWindow.h"


#include <Rynex/Events/ApplicationEvent.h>
#include <Rynex/Events/Event.h>
#include <Rynex/Events/KeyEvent.h>
#include <Rynex/Events/MouseEvent.h>

#include <Platform/OpenGL/OpenGLContext.h>
#include <Platform/OpenGL/OpenGLThreadContext.h>



namespace Rynex {


	static bool s_GlfwInitialized = false;


	static void GLFWErrorCallBack(int error, const char* description)
	{
		RY_CORE_ERROR("GLFW Error ({0}): {1} ", error, description);
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
		Shutdown();
	}

	Ref<ThreadContext> WindowsWindow::CreateThreadContext()
	{
		Ref<ThreadContext> threadContext = CreateRef<OpenGLThreadContext>();
		threadContext->Create(this);
		return threadContext;
	}

	void WindowsWindow::Init(const WindowProps& props)
	{
		m_Data.Title = props.m_Title;
		m_Data.Width = props.m_Width;
		m_Data.Height = props.m_Height;
	    m_Data.EventCallback = [](Event& event) -> void
	        {
	            RY_CORE_TRACE("Default Event:", event.ToString());
	        };

		RY_CORE_INFO("Creating window {0} ({1}, {2})", props.m_Title, props.m_Width, props.m_Height);

		if (!s_GlfwInitialized)
		{
		    constexpr int trueGlfw = GLFW_TRUE;
			const int success = glfwInit();
			RY_CORE_ASSERT(trueGlfw == success, "Could not initialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallBack);
			s_GlfwInitialized = true;
		}
		else
		{
		    RY_CORE_WARN("Glfw is already initialized!");
		}

#if defined(RY_DIST)


#endif

#ifdef RY_DEBUG
		m_Data.Title += { " Debug Mode" };
#elif defined(RY_DIST)
		m_Data.Title += { " Dist Mode" };
#elif RY_RELEASE
		m_Data.Title += { " Release Mode" };
#endif
#if RY_ENABLE_GARFIC_API_DBUGE_MASSEGES
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif

	    const int width = m_Data.Width;
	    const int height = m_Data.Height;
	    const char* titleChar = m_Data.Title.c_str();

		m_Window = glfwCreateWindow(width, height, titleChar, nullptr, nullptr);

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

	    SetEventCallbacks();
	    RY_CORE_INFO("Window success full init!");
	}

	void WindowsWindow::Shutdown()
	{
	    RY_CORE_FATAL_IF(0 != m_Context->GetMemoryUsage(), "Not Delete Buffer! {} KB", m_Context->GetMemoryUsage());

		RY_DESTROY_REF(m_Context);
		glfwDestroyWindow(m_Window);

#ifdef RY_SHOUTDOWN_GLFW_UNLOADE
	    if (s_GlfwInitialized)
	    {
	        glfwTerminate();
	        s_GlfwInitialized = false;
	        RY_CORE_INFO("Glfw should be deinitialized!");
	    }
#endif

	}

    void WindowsWindow::SetEventCallbacks()
    {
	    // Set GLFW callbacks
		glfwSetCursorEnterCallback(m_Window, [](GLFWwindow* window, int entered)
			{
				WindowData* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));

				if (GLFW_TRUE == entered)
				{
				    data->Hovered = true;
					WindowCurserEnterEvent event;
					data->EventCallback(event);
				}
				else if(GLFW_FALSE == entered)
				{
				    data->Hovered = false;
					WindowCurserLeaveEvent event;
					data->EventCallback(event);
				}
		        else
		        {
		            RY_CORE_WARN("Not expect callback cursor event!");
		        }

			});

		glfwSetWindowPosCallback(m_Window, [](GLFWwindow* window, int xPos, int yPos)
			{
		        WindowData* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
				data->PosX = xPos;
				data->PosY = yPos;

				WindowMovedEvent event(xPos, yPos);
#if RY_CONSOLE_WINDOW_POSTION
				RY_CORE_WARN("Window Position(x: {0}, y: {1})", xPos, yPos);
#endif

				data->EventCallback(event);

			});

		glfwSetWindowFocusCallback(m_Window, [](GLFWwindow* window, int focused)
			{
				WindowData* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));


				if(GLFW_TRUE == focused)
				{
				    data->Focus = true;
					WindowFocusEvent event;
					data->EventCallback(event);
				}
				else if (GLFW_FALSE == focused)
				{
				    RY_CORE_ASSERT(data->Focus && focused == GLFW_FALSE, "Focus callback method");
				    data->Focus = false;
					WindowLostFocusEvent event;
					data->EventCallback(event);
				}
		        else
		        {
                    RY_CORE_WARN("Not expect callback cursor focus!");
                }

			});


		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				WindowData* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
				data->Width = width;
				data->Height = height;

				WindowResizeEvent event(width, height);
#if RY_CONSOLE_WINDOW_SIZE
				RY_CORE_WARN("Window Size( x: {0}, y: {1})", width, height);
#endif
				data->EventCallback(event);
			});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				WindowData* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
				WindowCloseEvent event;
				data->EventCallback(event);
			});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				WindowData* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));

				switch (action)
				{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(key, 0);
					data->EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleaseEvent event(key);
					data->EventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(key, 1);
					data->EventCallback(event);
					break;
				}
				default:
				    RY_CORE_WARN("Not expect Callback Key event!");
				    break;
				}
			});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keyCode)
			{
				WindowData* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
				KeyTypedEvent event(keyCode);
				data->EventCallback(event);

			});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
			{
				WindowData* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));

				switch (action)
				{
				case GLFW_PRESS:
				{
					MouseButtenPressedEvent event(button);
					data->EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtenRealsedEvent event(button);
					data->EventCallback(event);
					break;
				}
				default:
				    RY_CORE_WARN("Not expect Callback Mouse Butten event!");
				}
			});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
			{
				WindowData* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));

				MouseSrolledEvent event(static_cast<float>(xOffset), static_cast<float>(yOffset));
				data->EventCallback(event);
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
			{
				WindowData* data = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
				MouseMoveEvent event(static_cast<float>(xPos), static_cast<float>(yPos));
				data->MousePosX = static_cast<float>(xPos);
				data->MousePosY = static_cast<float>(yPos);
#if RY_CONSOLE_MOUSE_POSTION
				RY_CORE_WARN("Mouse Position(x: {0}, y: {1})", xPos, yPos);
#endif
				data->EventCallback(event);
			});

		RY_CORE_INFO("Window Callbacks Setup (Required to set callback methode/function withe SetEventCallback)!");
    }

    void WindowsWindow::OnUpdate()
	{
		Ref<PlatformTimer> timer = PlatformTimer::Create(&m_UpdateScreeTime);
		glfwPollEvents();
		OnScreenRefresh();
	}

	void WindowsWindow::OnScreenRefresh()
	{
		m_Context->SwapBuffers();
	}

	void WindowsWindow::SetVSync(bool enabled)
	{
	    const int interval = enabled ? 1 : 0;
	    glfwSwapInterval(interval);
		m_Data.VSync = enabled;
	}

	bool WindowsWindow::IsFocused() const
	{
		return m_Data.Focus;
	}

	bool WindowsWindow::IsVSync() const
	{
		return m_Data.VSync;
	}

	
}
