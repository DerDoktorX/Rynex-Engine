#include "rypch.h"
#include "Application.h"

#include <Rynex/Core/Log.h>
#include <Rynex/Core/Input.h>
#include <Rynex/Renderer/Rendering/Renderer.h>
#include <Rynex/Scripting/Mono/ScriptingEngine.h>

#include <GLFW/glfw3.h>



#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

namespace Rynex {


	Application* Application::s_Instance = nullptr;


	Application::Application(const ApplicationSpecification& specification)
		: m_Specification(specification)
		, m_MaxMainThread(RY_MAX_QUEUE_PER_FRAME)
		, m_LastFrameTime(0.0f)
		, m_ThreadPool(RY_MAX_THREAD_IN_USE)
		, m_MainThreedQueueMutex()
		, m_Running(true)
		, m_Minmized(false)
		, m_QueueTimer(nullptr)
		, m_LayerStack()
		, m_ImGuiLayer(nullptr)
		, m_ImGuiTimeFrame(1ull)
		, m_MaxQueueMainThreadTime(0ull)
		, m_QueuePastTime(0ull)
		, m_QueueDivedFrameTime(8ull)
		, m_RenderThread()
		, m_RenderThreadMutex()
	{
		m_QueueTimer = PlatformTimer::Create(&m_QueuePastTime);
		int64_t multyply = 1000ll;	// move from sec in milsec
		multyply *= 1000ll;			// move from milsec in microsec
		multyply *= 1000ll;			// move from microsec in nanosec
		multyply *= 10ll;			// move from 1/60 in 10/6
		
		int64_t frameTime60up = 1ll * multyply;
		int64_t frameTime60down = 6ll;
		int64_t up = 1;
		int64_t down = m_QueueDivedFrameTime;
		int64_t frameTime60By1DivedBy8up = frameTime60up * up;
		int64_t frameTime60By1DivedBy8down = frameTime60down * down;
		m_MaxQueueMainThreadTime = frameTime60By1DivedBy8up / frameTime60By1DivedBy8down;
		

		RY_CORE_INFO("Application::Application Start!");
		RY_CORE_ASSERT(!s_Instance, "Applicationse allrady exists!");

		s_Instance = this;

		if (!m_Specification.WorkingDirectory.empty())
			std::filesystem::current_path(m_Specification.WorkingDirectory);

		m_Window = Window::Create(WindowProps(m_Specification.Name));
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));
		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
		RY_CORE_INFO("Application::Application Finished!");


	}

	Application::~Application()
	{		
		ExecuteAllMainThreedQueue();
		m_ThreadPool.~ThreadPool();
		ExecuteAllMainThreedQueue();

		for (Layer* layer : m_LayerStack)
		{
				layer->OnDetach();
		}

		
		for (Ref<ThreadContext>& layer : m_ThreadContextVec)
		{
			RY_DESTROY_REF(layer);
		}
		m_ThreadContextVec.clear();
		
		
		RY_DESTROY_REF(m_Window);

		if(s_Instance == this)
			s_Instance = nullptr;

	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowCloseEvent));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));

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
	
	void Application::PopLayer(Layer* layer)
	{
		
		layer->OnDetach();
		m_LayerStack.PopLayer(layer);
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
			
			TimeStep timestep(static_cast<float>(time) - m_LastFrameTime, static_cast<float>(time));
			m_LastFrameTime = static_cast<float>(time);

			// Thread!
			ExecuteMainThreedQueue();

			if (!m_Minmized)
			{
				RY_PROFILE_SCOPE("Update Render!");
				RY_SCOPE_TIMER(m_LayerTimeFrame);
				for (Layer* layer : m_LayerStack)
					layer->OnUpdate(timestep);
			}

#if RY_ENABLE_IMGUI
			{
				RY_PROFILE_SCOPE("ImGui Render!");
				RY_SCOPE_TIMER(m_ImGuiTimeFrame);
				if(nullptr != m_ImGuiLayer)
				{
					m_ImGuiLayer->Begin();
					for (Layer* layer : m_LayerStack)
						layer->OnImGuiRender();
					m_ImGuiLayer->End();
				}
			}
#endif

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
			m_Minmized = true;
			return false;
		}
		m_Minmized = false;
		Renderer::OnWindowsResize(e.GetWidth(), e.GetHeight());

		return false;
	}

	
	

	void Application::SubmiteToMainThreedQueue(const std::function<void()>& func)
	{
		if (m_Running)
		{
			SubmiteToMainThreedQueuePush(func, nullptr);
		}
		else
		{
			func();
		}
	}

	void Application::SubmiteToMainThreedQueueWait(const std::function<void()>& func)
	{	
		if (!m_Running)
			return;	

		std::mutex mutex;
		std::condition_variable cv;
		std::unique_lock lockWait(mutex);	
		RY_CORE_TRACE("Submit funtion to main thread an wait!");
		SubmiteToMainThreedQueuePush(func, &cv);
		cv.wait(lockWait);
		RY_CORE_TRACE("Resume Exexution to main thread!");
	}

	
	void Application::SubmiteToMainThreedQueuePush(const std::function<void()>& func, std::condition_variable* cvPtr)
	{
		std::scoped_lock<std::mutex> lock(m_MainThreedQueueMutex);
		MainThreadExe mainThreadExe = MainThreadExe(func, cvPtr);
		m_MainThreedQueue.push(mainThreadExe);
	}


	void Application::ExecuteMainThreedQueue()
	{
		std::scoped_lock<std::mutex> lock(m_MainThreedQueueMutex);
		
		m_QueueTimer->Stop();
		m_MaxQueueMainThreadTime = m_QueuePastTime / m_QueueDivedFrameTime;
		m_QueueTimer->Start();
		m_QueueTimer->CurentPastTime();

		while(!m_MainThreedQueue.empty() && m_QueuePastTime < m_MaxQueueMainThreadTime)
		{
			MainThreadExe& mainThreadexe = m_MainThreedQueue.front();
			std::function<void()>& func = mainThreadexe.first;
			std::condition_variable* cvPtr = mainThreadexe.second;
			func();
			if (nullptr != cvPtr)
			{
				RY_CORE_TRACE("Waiting thread resume his Exution Now!");
				cvPtr->notify_one();
			}
			m_MainThreedQueue.pop();
			m_QueueTimer->CurentPastTime();
		}
		
	}

	void Application::ExecuteAllMainThreedQueue()
	{
		std::scoped_lock<std::mutex> lock(m_MainThreedQueueMutex);
		while (!m_MainThreedQueue.empty())
		{
			MainThreadExe& mainThreadexe = m_MainThreedQueue.front();
			std::function<void()>& func = mainThreadexe.first;
			std::condition_variable* cvPtr = mainThreadexe.second;
			func();
			if (nullptr != cvPtr)
			{
				RY_CORE_TRACE("Waiting thread resume his Exution Now!");
				cvPtr->notify_one();
			}
			m_MainThreedQueue.pop();
		}
	}

	void Application::SubmitThreadTask(const std::function<void()>& task, const std::string& taskDiscription)
	{
		m_ThreadPool.SubmitEnqueue(task, taskDiscription);
	}

	bool Application::ExecuteTaskFromThread()
	{
		return m_ThreadPool.ExecuteWorkeOnCurentThread();
	}

	
}