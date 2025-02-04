#pragma once
#include "Rynex/Core/Window.h"
#include "Rynex/Core/LayerStack.h"
#include "Rynex/Events/Event.h"
#include "Rynex/Events/ApplicationEvent.h"

#include "Rynex/Core/TimeStep.h"

#include "Rynex/ImGui/ImGuiLayer.h"
#include <Rynex/Renderer/API/ThreadContext.h>

int main(int argc, char** argv);

namespace Rynex {

	struct ApplicationCommandLineArgs
	{
		int Count = 0;
		char** Args = nullptr;

		const char* operator[](int index) const
		{
			RY_CORE_ASSERT(index < Count, "Error: ApplicationCommandLineArgs");
			return Args[index];
		}
	};

	struct ApplicationSpecification
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

		// inline Layer* GetLayer(int index = 0) { return m_LayerStack[index]; }
		ApplicationSpecification& GetSpecification() { return m_Specification; }

		// Treads!
		void SubmiteToMainThreedQueue(const std::function<void()>& func);
		void SubmiteToMainThreedQueueCreateObject(const std::function<void()>& func);
		void SubmiteToMainThreedQueueDestroyObject(const std::function<void()>& func);
		void ExecuteMainThreedQueue();

		
		
	private:
		void Run();

		bool OnWindowCloseEvent(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

	private:
		Ref<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Mineized = false;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.0f;
		const uint32_t m_MaxMainThread = 10;

		std::vector<std::function<void()>> m_MainThreedQueue;
		std::vector<std::function<void()>> m_MainThreedQueueWaiting;
		std::vector<std::function<void()>> m_MainThreedQueueWaitingCreateObject;
		std::vector<std::function<void()>> m_MainThreedQueueWaitingDestroyObject;
		std::mutex m_MainThreedQueueMutex;

		ApplicationSpecification m_Specification;
	private:
		static Application* s_Instance;
		friend int ::main(int argc, char** argv);
	};

	Application* CreateApplication(ApplicationCommandLineArgs spec);
}

