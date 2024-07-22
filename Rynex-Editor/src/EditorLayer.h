#pragma once

#include "Rynex.h"
#include "Pannel/SceneHierachyPannel.h"
#include "Pannel/ContentBrowserPannel.h"

struct ImVec2;

namespace Rynex{
#define CHECK_FOR_ERRORS 0
// TODO: Make The System Worke CHECK_FOR_ERRORS

	class EditorLayer : public Layer
	{
	public:
		enum class SceneState
		{
			Edit = 0, Play = 1, Simulate = 2
		};
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
		void NewProject();
		bool OpenProject();
		void OpenProject(const std::filesystem::path& path);
		void SaveProject();

		void NewScene();
		void OpenScene();
		void OpenScene(const std::filesystem::path& path);
		void OpenScene(AssetHandle handle);
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
		void ImGuiProject();
		void ImGuiScript();
		void ImGuiEdit();
		void ImGuiView();
		void ImGuiHelp();

	private:
		EditorCamera m_EditorCamera;
		OrthograficCameraController	m_CameraController;
		
		// Scene
		Ref<Scene>				m_AktiveScene;
		Ref<Scene>				m_EditorScene;
		Ref<Framebuffer>		m_Framebuffer;
		// Camera
		Entity					m_CamerEntity;
		Entity					m_SecoundCameraEntity;
		// Selected Entity
		Entity					m_HoveredEntity;

		// Statats
		SceneState m_SceneState = SceneState::Edit;
		

		bool m_Runtime = false;
		bool m_PrimeCamera = false;

		float m_PasTime = 0.0f; // Not Current Finale Iplementet 
		int m_GizmoType = -1;
		// Window
		bool m_ViewPortFocused = false;
		bool m_ViewPortHoverd = false;

		glm::vec2 m_ViewPortSize = { 0.0f,0.0f };
		glm::vec2 m_ViewportBounds[2];

		// Panels
		SceneHierachyPannel m_Scene_HPanel;
		ContentBrowserPannel m_Content_BPannel;
		

		// Paths
		std::filesystem::path m_EditorScenePath;

		// Project
		Ref<Project> m_Project;
		Ref<EditorAssetManager> m_AssetManger;
	};
}

