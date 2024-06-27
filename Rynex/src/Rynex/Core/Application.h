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

		void Run();

		void OnEvent(Event& event);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		void Close();

		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

		inline static Application& Get() { return *s_Instance; }
		inline Window& GetWindow() { return *m_Window; }
		
	private:
		bool OnWindowCloseEvent(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	private:
		Ref<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Mineized = false;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.0f;

	private:
		static Application* s_Instance;
		friend int ::main(int argc, char** argv);
	};

	Application* CreateApplication();
}

