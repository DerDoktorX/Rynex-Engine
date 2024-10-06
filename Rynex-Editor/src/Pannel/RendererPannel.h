#pragma once

#include <Rynex/Events/Event.h>
#include <Rynex/Events/KeyEvent.h>
#include <Rynex/Events/MouseEvent.h>
#include <Rynex/Core/TimeStep.h>

namespace Rynex {
	class EditorLayer;
	class Framebuffer;

	class RendererPannel
	{
	public:
		RendererPannel(const std::string& name = "Renderer");
		~RendererPannel();

		void OnAttache(EditorLayer* editorLayer);
		void OnDetache();

		void OnEvent(Event& e);
		void OpenWindow();
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMousePressed(MouseButtenPressedEvent& e);

		void OnUpdate(TimeStep ts);
		void OnImGuiRender();
	private:
		
		enum RenderMode : int
		{
			CallFace_Nono = BIT(0),
			CallFace_Front = BIT(1),
			CallFace_Back = BIT(2),
			CallFace_FrontBack = BIT(3),
			WireFrame = BIT(4),
			A_Buffer = BIT(5),
			Death_Buffer = BIT(6)
		};
	private:
		EditorLayer* m_EditorLayer = nullptr;
		std::string m_Name = "";
		bool m_WindowOpen = true;
		int m_RendererMode = 0;
		TimeStep m_CurentTS;
		Ref<Framebuffer> m_MainFrameB = nullptr;
	};
}

