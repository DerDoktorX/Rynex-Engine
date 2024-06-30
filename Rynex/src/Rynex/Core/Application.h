#pragma once

#include "Rynex/Core/Base.h"

#include "Rynex/Core/Window.h"
#include "Rynex/Core/LayerStack.h"
#include "Rynex/Events/Event.h"
#include "Rynex/Events/ApplicationEvent.h"

#include "Rynex/Core/TimeStep.h"

#include "Rynex/ImGui/ImGuiLayer.h"

int main(int argc, char** argv);

namespace Rynex {

	class RYNEX_API Application
	{
	public:
		Application(const std::string& = "Rynex App");
		virtual ~Application();

		

		void OnEvent(Event& event);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		void Close();

		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

		inline static Application& Get() { return *s_Instance; }
		inline Window& GetWindow() { return *m_Window; }

		void SubmiteToMainThreedQueue(const std::function<void()>& func);
		
	private:
		void Run();

		bool OnWindowCloseEvent(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

		void ExecuteMainThreedQueue();
	private:
		Ref<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Mineized = false;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.0f;

		std::vector<std::function<void()>> m_MainThreedQueue;
		std::mutex m_MainThreedQueueMutex;
	private:
		static Application* s_Instance;
		friend int ::main(int argc, char** argv);
	};

	Application* CreateApplication();
}

