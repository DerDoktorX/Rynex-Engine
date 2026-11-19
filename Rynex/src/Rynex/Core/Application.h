#pragma once
#include <Rynex/Core/Window.h>
#include <Rynex/Core/LayerStack.h>
#include <Rynex/Events/Event.h>
#include <Rynex/Events/ApplicationEvent.h>

#include <Rynex/Core/TimeStep.h>
#include <Rynex/Core/ThreadPool.h>

#include <Rynex/ImGui/ImGuiLayer.h>
#include <Rynex/Renderer/API/ThreadContext.h>

int main(int argc, char** argv);

namespace Rynex {

	struct RYNEX_API ApplicationCommandLineArgs
	{
		int m_Count;
		const char** m_Args;

	    ApplicationCommandLineArgs()
	        : m_Count(0)
            , m_Args(nullptr)
	    {
	    }

	    ApplicationCommandLineArgs(const int count, const char** args)
	        : m_Count(count)
	        , m_Args(args)
	    {
	    }

	    ApplicationCommandLineArgs(const ApplicationCommandLineArgs&) = default;

		const char* operator[](const int index) const
		{
			RY_CORE_ASSERT(index < m_Count, "Error: ApplicationCommandLineArgs");
			return m_Args[index];
		}
	};

	struct RYNEX_API ApplicationSpecification
	{
		std::string m_Name;
		std::string m_WorkingDirectory;
		ApplicationCommandLineArgs m_CommandLineArgs;

	    ApplicationSpecification()
	        : m_Name("Rynex Engine")
	        , m_WorkingDirectory()
	        , m_CommandLineArgs()
	    {
	    }

	    ApplicationSpecification(const ApplicationSpecification&) = default;

	    ApplicationSpecification(const std::string_view name, const std::string_view workingDirectory, const ApplicationCommandLineArgs& commandLineArgs)
	        : m_Name(name)
	        , m_WorkingDirectory(workingDirectory)
	        , m_CommandLineArgs(commandLineArgs)
	    {
	    }
	};

	struct AssetFileWatcherThreadData
	{
		std::function<void(const FileSystem::Path& path)> m_Function;
		 FileSystem::Path m_Filepath;

		AssetFileWatcherThreadData(const std::function<void(const FileSystem::Path& path)>& function, const FileSystem::Path& filepath)
			: m_Function(function)
			, m_Filepath(filepath)
		{}
	};


	class RYNEX_API Application
	{
	public:
        explicit Application(const ApplicationSpecification& specification);
		virtual ~Application();

		

		void OnEvent(Event& event);


		void PushLayer(Ref<Layer> layer);
		void PushOverlay(Ref<Layer> layer);
		void PopLayer(Layer* layer);

		void Close();

		ImGuiLayer& GetImGuiLayer() {  RY_CORE_ASSERT(nullptr != m_ImGuiLayer, "ImGui Layer is nullptr!");  return *m_ImGuiLayer; }

		inline static Application& Get() { RY_CORE_ASSERT(nullptr != s_Instance, "Application Instance is nullptr!"); return *s_Instance; }
		inline Window& GetWindow() { RY_CORE_ASSERT(nullptr != m_Window, "Window is nullptr!"); return *m_Window; }
		inline Weak<Window> GetWindowWeak() { RY_CORE_ASSERT(nullptr != m_Window, "Window is nullptr!"); return m_Window; }
		int64_t GetImGuiTime() const { return m_ImGuiTimeFrame; }
		int64_t GetLayerTime() const { return m_LayerTimeFrame; }

		ApplicationSpecification& GetSpecification() { return m_Specification; }

		// Treads!
		void SubmitToMainThreedQueue(const std::function<void()>& func);
		void SubmitToMainThreedQueueWait(const std::function<void()>& func);
		bool IsRunning() const { return m_Running; }
		bool IsMainThread() const;
		void SubmitThreadTask(const std::function<void()>& task, const std::string& taskDescription);
		bool ExecuteTaskFromThread();

		const ThreadPool& GetThreadPool() const { return m_ThreadPool; };
		std::thread& GetRenderThread() { return m_RenderThread; };
	private:
	    void CalculateMaxQueueMainThread();
		void SubmitToMainThreedQueuePush(const std::function<void()>& func, std::condition_variable* cvPtr);
        WindowProps SearchStartArgsForWindow(const ApplicationSpecification& args);

		void Run();
		
		void ExecuteMainThreedQueue();
		void ExecuteAllMainThreedQueue();

		bool OnWindowCloseEvent(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	// --- private static methode ---------------------------------------------------------------------------------------------

	    static uint32_t SearchStartArgsForThreadCount(const ApplicationSpecification& specification);
	// --- private using types ------------------------------------------------------------------------------------------------
		using MainThreadExe = std::pair<std::function<void()>, std::condition_variable*>;
	// --- private member variables --------------------------------------------------------------------------------------------
		Ref<Window> m_Window;
		std::thread m_RenderThread;
		std::mutex m_RenderThreadMutex;

		std::vector<Ref<ThreadContext>> m_ThreadContextVec;
		
		const uint32_t m_MaxMainThread;

		float m_LastFrameTime;


		Ref<ImGuiLayer> m_ImGuiLayer;
		LayerStack m_LayerStack;

		ApplicationSpecification m_Specification;
		
		ThreadPool m_ThreadPool;
		
		
		std::queue<MainThreadExe>  m_MainThreedQueue;
		std::mutex m_MainThreedQueueMutex;

		int64_t m_MaxQueueMainThreadTime;
		int64_t m_QueueDivedFrameTime;
		int64_t m_QueuePastTime;
		Ref<PlatformTimer> m_QueueTimer;

		int64_t m_ImGuiTimeFrame;
		int64_t m_LayerTimeFrame;
	    std::thread::id m_MainThreadId;
		bool m_Running;
		bool m_Minimized;

	// --- private static variables -------------------------------------------------------------------------------------------
		static Application* s_Instance;
	// --- private friend function --------------------------------------------------------------------------------------------
		friend int ::main(int argc, char** argv);
	};

	RYNEX_API Scope<Application> CreateApplication(ApplicationCommandLineArgs spec);

#define RY_SUBMIT_MEMBER_FUNC_TO_MAINTAINED_QUEUE(func) Application::Get().SubmiteToMainThreedQueue( RY_BIND_MEMBER_FUNC(func) )
#define RY_SUBMIT_MEMBER_FUNC_TO_MAINTAINED_QUEUE_WAIT(func) Application::Get().SubmiteToMainThreedQueueWait( RY_BIND_MEMBER_FUNC(func) )
#define RY_SUBMIT_TASK_TO_ASYNC_THREED(disc, ...) Application::Get().SubmitThreadTask( __VA_ARGS__, disc )
}

