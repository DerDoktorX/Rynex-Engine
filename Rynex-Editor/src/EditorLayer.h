#pragma once

#include "Rynex.h"
#include "Pannel/SceneHierachyPannel.h"
#include "Pannel/ContentBrowserPannel.h"

struct ImVec2;

namespace Rynex{

	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;
		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(Rynex::TimeStep ts) override;

		//-- Events -----------------
		virtual void OnEvent(Rynex::Event& e) override;

		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMousePressed(MouseButtenPressedEvent& e);


		//-- Task -------------------
		void NewScene();
		void OpenScene();
		void OpenScene(const std::filesystem::path& path);
		void SaveSceneAs();
		void SaveScene();


		//-- ImGui ------------------
		virtual void OnImGuiRender() override;

		//--- Layoute ----------------
		void ImGuiViewPort();
		void ImGuiSecundaryViewPort(const Ref<Framebuffer>& framebuffer, uint32_t id = 0, float width = 12.8f, float height = 7.2f);
		void ImGuiSettings();
		void ImGuiScenePannel();
		void ImGuiContentBrowserViewPort();
		void ImGuiSetMausPosInViewPort( ImVec2 vpOffset);
		void ImGizmoInViewPort();
		void ImGuiViewPortResize( ImVec2 vPSize);


		//--- Top Taskbar -------------
		void ImGuiTopTaskBar();
		void ImGuiFile();
		void ImGuiEdit();
		void ImGuiView();
		void ImGuiHelp();

	private:
		EditorCamera m_EditorCamera;
		OrthograficCameraController	m_CameraController;

		Ref<Shader>				m_BlueShader;
		Ref<VertexArray>		m_SquareVA;
		Ref<Framebuffer>		m_Framebuffer;

		//Scene
		Ref<Scene>				m_AktiveScene;

		//Camera
		Entity					m_CamerEntity;
		Entity					m_SecoundCameraEntity;

		//Selected Entity
		Entity					m_HoveredEntity;


		bool m_PrimeCamera = false;

		//SceneEntity
		Entity					m_SqareEntity;

		//Assets
		Ref<Texture2D>			m_ChekbordTex;
		
		float m_ChekebordRotaion;
		glm::vec3 m_SquareColor = { 0.2f,0.3f,0.8f };
		glm::vec2 m_ViewPortSize = { 0.0f,0.0f };
		glm::vec2 m_ViewportBounds[2];

		int m_GizmoType = -1;

		bool m_ViewPortFocused = false;
		bool m_ViewPortHoverd = false;
		//Panels
		SceneHierachyPannel m_Scene_HPanel;
		ContentBrowserPannel m_Content_BPannel;
	};
}

