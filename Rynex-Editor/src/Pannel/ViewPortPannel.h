#pragma once
#include <Rynex/Core/TimeStep.h>
#include <Rynex/Events/Event.h>
#include <Rynex/Scene/Scene.h>
#include <Rynex/Renderer/API/Framebuffer.h>
#include <Rynex/Core/CamerController.h>

namespace Rynex {

	enum class ViewPortFlages
	{
		None = 0,
		HoverFocuse = BIT(1),
		MainViewPort = BIT(2),
		ImGizmo = BIT(3) | HoverFocuse,
		DragAndDrop = BIT(4) | HoverFocuse,
		Defaulte = HoverFocuse | MainViewPort,
	};

	class ViewPortPannel
	{
	public:
		ViewPortPannel(Ref<Framebuffer>& frambuffer, const std::string& name = "ViewPort", ViewPortFlages flags = ViewPortFlages::Defaulte, uint32_t renderID = 0);

		void OnAttach(const Ref<Framebuffer>& framebuffer);
		void OnDetach();
		void OnUpdate(TimeStep ts);

		void OnEvent(Event& e);

		void OnImGuiRender();
		void Resize();
		bool GetMousePos(glm::vec<2, float>& tempMouse);
	private:
		glm::vec2 m_ViewPortSize = { 0.0f,0.0f };
		glm::vec2 m_ViewportBounds[2];

		Ref<Scene>							m_AktiveScene;
		Ref<Scene>							m_EditorScene;
		Ref<Framebuffer>					m_Framebuffer;

		std::string							m_Name;
		
		
									
		bool								m_ViewPortFocused = false;
		bool								m_ViewPortHoverd = false;
		uint32_t							m_RenderID;
		ViewPortFlages						m_AktivFlags;

		Ref<EditorCamera>					m_EditorCamera;
		Ref<OrthograficCameraController>	m_CameraController;
	};

}


