#include <rypch.h>
#include "Application.h"

#include <Rynex/Core/Log.h>
#include <Rynex/Core/Input.h>
#include <Rynex/Renderer/Rendering/Renderer.h>
#include <Rynex/Scripting/Mono/ScriptingEngine.h>

#include <GLFW/glfw3.h>



#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

namespace Rynex {
    namespace {
        static constexpr bool IsNotCarcaterNumber(char charcater)
        {
            constexpr char zeroChar = '0';
            constexpr char nineChar = '9';

            return zeroChar > charcater && charcater < nineChar;

        }


        static int GetNumberInt(std::string_view view, int min = 0)
        {
            constexpr char zeroChar = '0';
            constexpr char nineChar = '9';
            constexpr char spaceChar = ' ';

            int number = 0;
            size_t pos = 0;


            while (pos < view.size())
            {

                char charecter = view.at(pos);
                if (IsNotCarcaterNumber(charecter))
                    return number;

                number = number * 10 + (charecter - zeroChar);
                pos++;
            }
            return std::max(min, number);
        }
    }
	Application* Application::s_Instance = nullptr;




	Application::Application(const ApplicationSpecification& specification)
		: m_RenderThread()
		, m_RenderThreadMutex()
		, m_MaxMainThread(RY_MAX_QUEUE_PER_FRAME)
		, m_LastFrameTime(0.0f)
		, m_ImGuiLayer(nullptr)
		, m_LayerStack()
		, m_Specification(specification)
		, m_ThreadPool(SearchStartArgsForThreadCount(specification))
		, m_MainThreedQueueMutex()
		, m_MaxQueueMainThreadTime(0ull)
		, m_QueueDivedFrameTime(8ull)
		, m_QueuePastTime(0ull)
		, m_QueueTimer(nullptr)
		, m_ImGuiTimeFrame(1ull)
        , m_MainThreadId(std::this_thread::get_id())
		, m_Running(true)
		, m_Minimized(false)
	{
		m_QueueTimer = PlatformTimer::Create(&m_QueuePastTime);
		CalculateMaxQueueMainThread();
		

		RY_CORE_INFO("Application::Application Start!");
		RY_CORE_ASSERT(nullptr == s_Instance, "Applications already exists!");

		s_Instance = this;

		if (!m_Specification.WorkingDirectory.empty())
			std::filesystem::current_path(m_Specification.WorkingDirectory);

        WindowProps prop = SearchStartArgsForWindow(m_Specification);
		m_Window = Window::Create(prop);
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));
		m_ImGuiLayer = CreateRef<ImGuiLayer>();
		PushOverlay(m_ImGuiLayer);
		RY_CORE_INFO("Application::Application Finished!");


	}

	Application::~Application()
	{		
		ExecuteAllMainThreedQueue();
		m_ThreadPool.~ThreadPool();
		ExecuteAllMainThreedQueue();

		for (Ref<Layer>& layer : m_LayerStack)
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

		for ( std::vector<Ref<Layer>>::iterator it = m_LayerStack.end(); it != m_LayerStack.begin(); )
		{
			
			if (e.Handled)
				break;
			(*--it)->OnEvent(e);
		}
	}

	void Application::PushLayer(Ref<Layer> layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Ref<Layer> layer)
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
		RY_CORE_INFO("Application::Run Start!");
		while (m_Running) 
		{	
			RY_PROFILE_SCOPE("Main UpdateLoop");
			double time = glfwGetTime();
			
			TimeStep timestep(static_cast<float>(time) - m_LastFrameTime, static_cast<float>(time));
			m_LastFrameTime = static_cast<float>(time);

			// Thread!
			ExecuteMainThreedQueue();

			if (!m_Minimized)
			{
				RY_PROFILE_SCOPE("Update Render!");
				RY_SCOPE_TIMER(m_LayerTimeFrame);
				for (Ref<Layer>& layer : m_LayerStack)
					layer->OnUpdate(timestep);
			}

#if RY_ENABLE_IMGUI
			{
				RY_PROFILE_SCOPE("ImGui Render!");
				RY_SCOPE_TIMER(m_ImGuiTimeFrame);
				if(nullptr != m_ImGuiLayer)
				{
					m_ImGuiLayer->Begin();
					for (Ref<Layer>& layer : m_LayerStack)
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

		RY_CORE_INFO("Application::Run Finished! -> Next Shutdown From APP are Executed!");
	}
	
	bool Application::OnWindowCloseEvent(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		float width = e.GetWidth();
		float height = e.GetHeight();
		if (0 == height || 0 == width)
		{
			m_Minimized = true;
			return false;
		}

		m_Minimized = false;
		Renderer::OnWindowsResize(width, height);

		return false;
	}

    uint32_t Application::SearchStartArgsForThreadCount(const ApplicationSpecification& specification)
    {
	    constexpr const char* fieldThreadCount = "ThreadCount";
	    const ApplicationCommandLineArgs& args = specification.CommandLineArgs;
	    int i = 0;
	    uint32_t threadCount = RY_MAX_THREAD_IN_USE;
	    while (i < args.Count)
	    {
	        const int next = i + 1;
	        if (args.Count == next)
	            break;

	        const std::string_view filed{ args[i] };
	        if (fieldThreadCount == filed)
	        {
	            i++;
	            threadCount = GetNumberInt(args[i]);
	        }
	        i++;
	    }
        return threadCount;
    }


    void Application::SubmitToMainThreedQueue(const std::function<void()>& func)
	{
		if (m_Running)
		{
			SubmitToMainThreedQueuePush(func, nullptr);
		}
		else
		{
			func();
		}
	}

	void Application::SubmitToMainThreedQueueWait(const std::function<void()>& func)
	{	
		if (!m_Running)
			return;	

		std::mutex mutex;
		std::condition_variable cv;
		std::unique_lock lockWait(mutex);	
		RY_CORE_TRACE("Submit funtion to main thread an wait!");
		SubmitToMainThreedQueuePush(func, &cv);
		cv.wait(lockWait);
		RY_CORE_TRACE("Resume Exexution to main thread!");
	}

	
	void Application::SubmitToMainThreedQueuePush(const std::function<void()>& func, std::condition_variable* cvPtr)
	{
		std::scoped_lock<std::mutex> lock(m_MainThreedQueueMutex);
		MainThreadExe mainThreadExe = MainThreadExe(func, cvPtr);
		m_MainThreedQueue.push(mainThreadExe);
	}

    WindowProps Application::SearchStartArgsForWindow(const ApplicationSpecification& spec)
    {
	    constexpr const char* markerWithe = "--WindowWidth";
	    constexpr const char* markerHeight = "--WindowHeight";
	    constexpr const char* markerVisibility = "--WindowVisibility";
	    constexpr const char* markerVisibilityTrue = "true";
	    constexpr const char* markerVisibilityFalse = "false";
	    WindowProps props(spec.Name);
	    const ApplicationCommandLineArgs& args = spec.CommandLineArgs;
	    int i = 0;
	    while (i < args.Count)
	    {
	        const int next = i + 1;
	        if (args.Count == next)
	            break;

	        const std::string_view filed{args[i]};
	        if (markerVisibility == filed)
	        {
	            i++;
	            if (markerVisibilityTrue == args[i])
	                props.m_Visible = true;
	            else if (markerVisibilityFalse == args[i])
	                props.m_Visible = false;
	            else
	                RY_CORE_ERROR("No valid reading input! {}", args[i--]);
	        }
	        else if (markerWithe == filed)
	        {
	            i++;
	            const int width = GetNumberInt(args[i]);
	            if (0 < width)
	                props.m_Width = width;
	            else
	                i--;
	        }
	        else if (markerHeight == filed)
	        {
	            i++;
	            const int height = GetNumberInt(args[i]);
	            if (0 < height)
	                props.m_Height = height;
	            else
	                i--;
	        }

	        i++;
	    }
        return props;
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

    void Application::CalculateMaxQueueMainThread()
    {
        int64_t multyply = 1000ll;	// move from sec in millisecond
        multyply *= 1000ll;			// move from millisecond in microsecond
        multyply *= 1000ll;			// move from microsecond in nanosecond
        multyply *= 10ll;			// move from 1/60 in 10/6

        int64_t frameTime60up = 1ll * multyply;
        int64_t frameTime60down = 6ll;
        int64_t up = 1;
        int64_t down = m_QueueDivedFrameTime;
        int64_t frameTime60By1DivedBy8up = frameTime60up * up;
        int64_t frameTime60By1DivedBy8down = frameTime60down * down;
        m_MaxQueueMainThreadTime = frameTime60By1DivedBy8up / frameTime60By1DivedBy8down;
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
				RY_CORE_TRACE("Waiting thread resume his Execution Now!");
				cvPtr->notify_one();
			}
			m_MainThreedQueue.pop();
		}
	}

    bool Application::IsMainThread() const
    {
	    const std::thread::id threadId = std::this_thread::get_id();
	    return threadId == m_MainThreadId;
    }

	void Application::SubmitThreadTask(const std::function<void()>& task, const std::string& taskDescription)
	{
		m_ThreadPool.SubmitEnqueue(task, taskDescription);
	}

	bool Application::ExecuteTaskFromThread()
	{
		return m_ThreadPool.ExecuteWorkeOnCurentThread();
	}

	
}