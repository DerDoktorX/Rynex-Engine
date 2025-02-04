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
		RY_CORE_INFO("Application::Application Start!");
		RY_CORE_ASSERT(!s_Instance, "Applicationse allrady exists!");
		s_Instance = this;

		if (!m_Specification.WorkingDirectory.empty())
			std::filesystem::current_path(m_Specification.WorkingDirectory);

		m_Window = Window::Create(WindowProps(m_Specification.Name));
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

		m_ImGuiLayer = new ImGuiLayer();
		RY_CORE_MEMORY_ALICATION("m_ImGuiLayer", "Application::Application", ImGuiLayer);
		PushOverlay(m_ImGuiLayer);
		RY_CORE_INFO("Application::Application Finished!");
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

		// RY_CORE_TRACE("{0}", e.ToString());

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
	
	void Application::PopLayer(Layer* rlayer)
	{
		
		rlayer->OnDetach();
		m_LayerStack.PopLayer(rlayer);
	}

	void Application::Close()
	{
		m_Running = false;
	}



	void Application::Run()
	{
		RY_CORE_INFO("Application::Run Starte!");
		while (m_Running) 
		{	
			RY_PROFILE_SCOPE("Main UpdateLoop");
			double time = glfwGetTime();
			TimeStep timestep((float)time - m_LastFrameTime, time);
			m_LastFrameTime = (float)time;

			// Thread!
			ExecuteMainThreedQueue();

			if (!m_Mineized) 
			{
				RY_PROFILE_SCOPE("Update Render!");
				for (Layer* layer : m_LayerStack)
					layer->OnUpdate(timestep);
			}

			{
				RY_PROFILE_SCOPE("ImGui Render!");
				m_ImGuiLayer->Begin();
				for (Layer* layer : m_LayerStack)
					layer->OnImGuiRender();
				m_ImGuiLayer->End();
			}



			{
				RY_PROFILE_SCOPE("Windows Update!");
				m_Window->OnUpdate();
			}
			
		}

		RY_CORE_INFO("Application::Run Finished! -> Next Shutdown From APP are Exexuted!");
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
#if RY_MAX_MAIN_THREAD_QUEUE_PER_FRAME
		if(m_MainThreedQueue.size() < m_MaxMainThread)

			m_MainThreedQueue.emplace_back(func);
		else
		{
			m_MainThreedQueueWaiting.emplace_back(func);
		}
#else
		m_MainThreedQueue.emplace_back(func);
#endif
	}

	void Application::SubmiteToMainThreedQueueCreateObject(const std::function<void()>& func)
	{
		std::scoped_lock<std::mutex> lock(m_MainThreedQueueMutex);
		m_MainThreedQueueWaitingCreateObject.emplace_back(func);
	}

	void Application::SubmiteToMainThreedQueueDestroyObject(const std::function<void()>& func)
	{
		std::scoped_lock<std::mutex> lock(m_MainThreedQueueMutex);
		m_MainThreedQueueWaitingCreateObject.emplace_back(func);
	}


	void Application::ExecuteMainThreedQueue()
	{
#if RY_TODO_APPLICATION_MULTI_THREAD
		std::vector<std::function<void()>> copy;
		{
			std::scoped_lock<std::mutex> lock(m_MainThreedQueueMutex);
			copy = m_MainThreedQueue;
			m_MainThreedQueue.clear();
#if RY_MAX_MAIN_THREAD_QUEUE_PER_FRAME
			uint32_t i = 0;
			if (m_MainThreedQueueWaitingCreateObject.size() > 0)
			{
				
				std::vector<std::function<void()>>::iterator itFunc = m_MainThreedQueueWaitingCreateObject.begin();
				std::vector<std::function<void()>>::iterator itend = m_MainThreedQueueWaitingCreateObject.end();
				std::vector<std::function<void()>>::const_iterator itBegin = itFunc;
				for (; itFunc != itend; ++itFunc, i++)
				{
					std::function<void()>& func = *itFunc;
					if (i >= m_MaxMainThread)
						break;

					m_MainThreedQueue.emplace_back(func);

				}

				m_MainThreedQueueWaitingCreateObject.erase(itBegin, itFunc);
			}

			if (m_MainThreedQueueWaitingDestroyObject.size() > 0 && i >= m_MaxMainThread)
			{
				std::vector<std::function<void()>>::iterator itFunc = m_MainThreedQueueWaitingDestroyObject.begin();
				std::vector<std::function<void()>>::iterator itend = m_MainThreedQueueWaitingDestroyObject.end();
				std::vector<std::function<void()>>::const_iterator itBegin = itFunc;
				for (; itFunc != itend; ++itFunc, i++)
				{
					std::function<void()>& func = *itFunc;
					if (i >= m_MaxMainThread)
						break;

					m_MainThreedQueue.emplace_back(func);

				}
			}


			 if(m_MainThreedQueueWaiting.size() > 0&& i >= m_MaxMainThread)
			 {
			 	std::vector<std::function<void()>>::iterator itFunc = m_MainThreedQueueWaiting.begin();
			 	std::vector<std::function<void()>>::iterator itend = m_MainThreedQueueWaiting.end();
			 	std::vector<std::function<void()>>::const_iterator itBegin = itFunc;
			 	for (; itFunc != itend; ++itFunc, i++)
			 	{
			 		std::function<void()>& func = *itFunc;
			 		if (i >= m_MaxMainThread)
			 			break;
			 
			 		m_MainThreedQueue.emplace_back(func);
			 
			 	}
			 
			 	m_MainThreedQueueWaiting.erase(itBegin, itFunc);
			 }
#endif
		}


		for (std::function<void()>& func : copy)
			func();
#else
		std::scoped_lock<std::mutex> lock(m_MainThreedQueueMutex);

		for (std::function<void()>& func : m_MainThreedQueue)
			func();

		m_MainThreedQueue.clear();
#endif		
	}

	
}