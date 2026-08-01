#pragma once
#include "Rynex/Core/Window.h"
#include "Rynex/Core/LayerStack.h"
#include "Rynex/Events/Event.h"
#include "Rynex/Events/ApplicationEvent.h"

#include "Rynex/Core/TimeStep.h"
#include <Rynex/Core/ThreadPool.h>

#include "Rynex/ImGui/ImGuiLayer.h"
#include <Rynex/Renderer/API/ThreadContext.h>

int main(int argc, char** argv);

namespace Rynex {

	struct RYNEX_API ApplicationCommandLineArgs
	{
		int Count = 0;
		char** Args = nullptr;

		const char* operator[](int index) const
		{
			RY_CORE_ASSERT(index < Count, "Error: ApplicationCommandLineArgs");
			return Args[index];
		}
	};

	struct RYNEX_API ApplicationSpecification
	{
		std::string Name = "Rynex Application";
		std::string WorkingDirectory;
		ApplicationCommandLineArgs CommandLineArgs;
	};

	struct AssetFileWatcherThreadData
	{
		std::function<void(std::filesystem::path path)> Function;
		std::filesystem::path Filepath;

		AssetFileWatcherThreadData(std::function<void(std::filesystem::path path)> function, std::filesystem::path filepath)
			: Function(function)
			, Filepath(filepath)
		{}
	};


	class RYNEX_API Application
	{
	public:
		Application(const ApplicationSpecification& specification);
		virtual ~Application();

		

		void OnEvent(Event& event);


		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		void PopLayer(Layer* layer);
		void Close();

		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

		inline static Application& Get() { return *s_Instance; }
		inline Window& GetWindow() { return *m_Window; }
		int64_t GetImGuiTime() const { return m_ImGuiTimeFrame; }
		int64_t GetLayerTime() const { return m_LayerTimeFrame; }

		ApplicationSpecification& GetSpecification() { return m_Specification; }

		// Treads!
		void SubmiteToMainThreedQueue(const std::function<void()>& func);
		void SubmiteToMainThreedQueueWait(const std::function<void()>& func);
		bool IsRunninig() const { return m_Running; }
		
		void SubmitThreadTask(const std::function<void()>& task, const std::string& taskDiscription);
		bool ExecuteTaskFromThread();

		const ThreadPool& GetThreadPool() const { return m_ThreadPool; };
		std::thread& GetRenderThread() { return m_RenderThread; };
	private:
		void SubmiteToMainThreedQueuePush(const std::function<void()>& func, std::condition_variable* cvPtr);

		void Run();
		
		void ExecuteMainThreedQueue();
		void ExecuteAllMainThreedQueue();

		bool OnWindowCloseEvent(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

	// --- private using types ------------------------------------------------------------------------------------------------
		using MainThreadExe = std::pair<std::function<void()>, std::condition_variable*>;
	// --- private member varibles --------------------------------------------------------------------------------------------
		Ref<Window> m_Window;
		std::thread m_RenderThread;
		std::mutex m_RenderThreadMutex;

		std::vector<Ref<ThreadContext>> m_ThreadContextVec;
		
		const uint32_t m_MaxMainThread;

		float m_LastFrameTime;


		ImGuiLayer* m_ImGuiLayer;
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
		bool m_Running;
		bool m_Minmized;
		
	// --- private static varibles --------------------------------------------------------------------------------------------
		static Application* s_Instance;
	// --- private frind function ---------------------------------------------------------------------------------------------
		friend int ::main(int argc, char** argv);
	};

	RYNEX_API Application* CreateApplication(ApplicationCommandLineArgs spec);

#define RY_SUBMITE_MEMBER_FUNC_TO_MAINTHREED_QUEUE(func) Application::Get().SubmiteToMainThreedQueue( RY_BIND_MEMBER_FUNC(func) )
#define RY_SUBMITE_MEMBER_FUNC_TO_MAINTHREED_QUEUE_WAIT(func) Application::Get().SubmiteToMainThreedQueueWait( RY_BIND_MEMBER_FUNC(func) )
#define RY_SUBMITE_TASK_TO_ASYNC_THREED(disc, ...) Application::Get().SubmitThreadTask( __VA_ARGS__, disc )
}

