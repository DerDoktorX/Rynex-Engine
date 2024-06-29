#include "rypch.h"
#include "Application.h"
#include "Rynex/Core/Log.h"
#include "Rynex/Renderer/Renderer.h"
#include "Input.h"
#include <GLFW/glfw3.h>
#include "Rynex/Scripting/ScriptingEngine.h"

#if 0
struct AlicationMetrics
{
	uint32_t TotalAliction = 0;
	uint32_t TotalFrees = 0;

	uint32_t CurrentUsage() { return TotalAliction - TotalFrees;  }
};

static AlicationMetrics s_AlicationMetrics;

void* operator new(size_t size)
{
	s_AlicationMetrics.TotalAliction += size;
	//printf("Alicating: %i size\n", (int)size);
	return malloc(size);
}

void operator delete(void* memory, size_t size)
{
	s_AlicationMetrics.TotalFrees += size;
	//printf("Freing Alicating: %i size\n", (int)size);
	return free(memory);
}

static void PrintMemoryUsage()
{
	printf("Memory Usage: %i bytes\n", s_AlicationMetrics.CurrentUsage());
}
#endif // TODO: Remaber what was that! than decide and Dealet?

#define RY_KONSOLE_FPS 0



namespace Rynex {

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)
	Application* Application::s_Instance = nullptr;


	Application::Application(const std::string& name)
	{
		RY_CORE_ASSERT(!s_Instance, "Applicationse allrady exists!");
		s_Instance = this;

		m_Window = Window::Create(WindowProps(name));
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));
		//m_Window->SetVSync(false);

		Renderer::Init();
		ScriptingEngine::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application()
	{
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowCloseEvent));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));

		//RY_CORE_TRACE("{0}", e.ToString());

		for ( std::vector<Rynex::Layer*>::iterator it = m_LayerStack.end(); it != m_LayerStack.begin(); )
		{
			
			if (e.Handled)
				break;
			(*--it)->OnEvent(e);
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}
	
	void Application::Close()
	{
		m_Running = false;
	}

	void Application::Run()
	{
		//m_Camera.SetPostione({ 0.5f, 0.5f, 0.0f });
		//m_Camera.SetRotation(45.0f);
		while (m_Running) 
		{	
			
			float time = (float)glfwGetTime();
			TimeStep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

#if RY_KONSOLE_FPS
			RY_CORE_INFO("FPS: {0}", 1/timestep);
#endif
			if (!m_Mineized) {
				for (Layer* layer : m_LayerStack)
					layer->OnUpdate(timestep);
				
				
			}

			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack)
				layer->OnImGuiRender();
			m_ImGuiLayer->End();


			

			m_Window->OnUpdate();
			
		}

	
	}
	
	bool Application::OnWindowCloseEvent(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		//m_win
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Mineized = true;
			return false;
		}
		m_Mineized = false;
		Renderer::OnWindowsResize(e.GetWidth(), e.GetHeight());

		return false;
	}
}