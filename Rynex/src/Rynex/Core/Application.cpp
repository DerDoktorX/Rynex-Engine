#include "rypch.h"
#include "Application.h"

#include "Rynex/Core/Log.h"
#include "Input.h"
#include "Rynex/Renderer/Rendering/Renderer.h"
#include "Rynex/Scripting/ScriptingEngine.h"

#include <GLFW/glfw3.h>


#if RY_TODO_APPLICATION_REMABER

#endif // TODO: Remeber what was that! then decide and Dealet?





#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

namespace Rynex {


	Application* Application::s_Instance = nullptr;


	Application::Application(const ApplicationSpecification& specification)
		: m_Specification(specification)
	{
		RY_CORE_ASSERT(!s_Instance, "Applicationse allrady exists!");
		s_Instance = this;

		if (!m_Specification.WorkingDirectory.empty())
			std::filesystem::current_path(m_Specification.WorkingDirectory);

		m_Window = Window::Create(WindowProps(m_Specification.Name));
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));
		//m_Window->SetVSync(false);

		m_ImGuiLayer = new ImGuiLayer();
		RY_CORE_MEMORY_ALICATION("m_ImGuiLayer", "Application::Application", ImGuiLayer);
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application()
	{

		for (Layer* layer : m_LayerStack)
			layer->OnDetach();
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
		RY_PROFILE_FUNCTION();
		//m_Camera.SetPostione({ 0.5f, 0.5f, 0.0f });
		//m_Camera.SetRotation(45.0f);
		while (m_Running) 
		{	
			RY_PROFILE_SCOPE("Main UpdateLoop");
			float time = (float)glfwGetTime();
			TimeStep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

#if RY_KONSOLE_FPS
			RY_CORE_INFO("FPS: {0}", 1/timestep);
#endif
			// Thread!
			ExecuteMainThreedQueue();
			ExecuteMainThreedQueueAssetFileWatcher();

			if (!m_Mineized) 
			{
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

	///////////////////////////////////////////////////
	//// Thread ///////////////////////////////////////
	///////////////////////////////////////////////////
	
	void Application::SubmiteToMainThreedQueue(const std::function<void()>& func)
	{
		std::scoped_lock<std::mutex> lock(m_MainThreedQueueMutex);
		m_MainThreedQueue.emplace_back(func);
	}

#if 1
	void Application::SubmiteToMainThreedQueueAssetFileWatcher(const std::function<void()>& func)
	{
		std::scoped_lock<std::mutex> lock(m_MainThreedQueueMutexAssetFileWatcher);
		//AssetFileWatcherThreadData data(func, path);
		m_MainThreedQueueAssetFileWatcher.emplace_back(func);
	}
#endif

	void Application::ExecuteMainThreedQueue()
	{
		RY_PROFILE_FUNCTION();
#if RY_TODO_APPLICATION_MULTI_THREAD
		std::vector<std::function<void()>> copy;
		{
			std::scoped_lock<std::mutex> lock(m_MainThreedQueueMutex);
			copy = m_MainThreedQueue;
			m_MainThreedQueue.clear();
		}


		for (auto& func : copy)
			func();
#else
		std::scoped_lock<std::mutex> lock(m_MainThreedQueueMutex);

		for (auto& func : m_MainThreedQueue)
			func();

		m_MainThreedQueue.clear();
#endif		
	}

#if 1
	void Application::ExecuteMainThreedQueueAssetFileWatcher()
	{
		RY_PROFILE_FUNCTION();
		std::scoped_lock<std::mutex> lock(m_MainThreedQueueMutexAssetFileWatcher);

		for (auto& func : m_MainThreedQueueAssetFileWatcher)
			func();

		m_MainThreedQueueAssetFileWatcher.clear();
	}
#endif
}