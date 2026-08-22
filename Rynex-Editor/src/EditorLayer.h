#pragma once

#include <Rynex/Core/Layer.h>
#include <Rynex/Events/KeyEvent.h>
#include <Rynex/Renderer/Camera/CamerController.h>
#include <Rynex/Renderer/API/Framebuffer.h>
#include <Rynex/Renderer/RenderCommand.h>

#include "Pannel/SceneHierachyPannel.h"
#include "Pannel/ContentBrowserPannel.h"
#include "Pannel/ViewPortPannel.h"
#include "Pannel/RendererPannel.h"
#include "Pannel/MenuBarPannel.h"
#include "Pannel/ProjectPannel.h"
#include "Pannel/MeshPannel.h"



struct ImVec2;
#define CHECK_FOR_ERRORS 0
#define RY_VIEW_PORT_FUNKTION 0

namespace Rynex{
	// TODO: Make The System Worke CHECK_FOR_ERRORS
	
	enum class SceneState
	{
		Edit = 0, Play = 1, Simulate = 2
	};

	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;
		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(TimeStep ts) override;

		//-- Events -----------------
		virtual void OnEvent(Event& e) override;

		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMousePressed(MouseButtenPressedEvent& e);

		

		
		//-- Open Pannel -----------------
		void OpenRenderPannel();
		void OpenAssetPannel();
		void OpenRegestriyPannel();
		void OpenSceneHierachyPannel();
		void OpenPropertiesPannel();
		void OpenProjectPannel();
		void OpenViewPortTexture();
		void OpenMeshPannel();

		//-- Task -------------------
		void NewProject();
		bool OpenProject();
		void OpenProject(const std::filesystem::path& path);
		void SaveProject();

		void NewScene();
		void OpenScene();
		void OpenScene(const std::filesystem::path& path);
		void OpenScene(AssetHandle handle);
		void OpenSceneAsync(AssetHandle handle);

		void SaveSceneAs();
		void SaveCurentScene();

		void SaveImagViewPort();
		//-- ImGui ------------------
		virtual void OnImGuiRender() override;

		Entity GetSelectedEntity() { return m_Scene_HPanel.GetSelectedEntity(); }
		SceneState GetSceneState() { return m_SceneState; }

		int* GetPtrGizmoType();

		const Ref<Scene>& GetAktivScene() { return m_AktiveScene; }
		const Ref<EditorCamera>& GetEditorCamera() const { return m_EditorCamera; }
		void SetSelectedEntity(Entity enitiy) { return m_Scene_HPanel.SetSelectedEntity(enitiy); }

		//--- Layoute ----------------

		// ViewPort 

		void ImGuiSettings(bool renderPannnel);
		void ImGuiPlayButten();
		void ImGuiRenderInfo();
		void ImGuiPannels();
		

		//--- Top Taskbar -------------
		void ImGuiTopTaskBar();
		void ImGuiFile();
		void ImGuiProject();
		void ImGuiScript();
		void ImGuiEdit();
		void ImGuiView();
		void ImGuiHelp();

		int64_t GetTimerUpdateViewPort() const { return m_ViewPortUpdateTime; }
		int64_t GetTimerRenderViewPort() const { return m_ViewPortRenderTime; }
		int64_t GetTimerScene3DSubmit() const { return m_AktiveScene->Get3DSubmitTime(); }
	private:
		RendererPannel* GetRendererPannel() { return &m_RendererPannel; }
		SceneHierachyPannel* GetSceneHierachyPannel() { return &m_Scene_HPanel; }
		ContentBrowserPannel* GetContentBrowserPannel() { return &m_Content_BPannel; }
		MenuBarPannel* GetMenuBarPannel() { return &m_MenuBarPannel; }
		ProjectPannel* GetProjectPannel() { return &m_ProjectPannel; }
		ViewPortPannel* GetViewPortPannel() { return &m_ViewPortPannel; }
		MeshPannel* GetMeshPannel() { return &m_MeshPannel; }

	private:

		Ref<EditorCamera> m_EditorCamera;
		OrthograficCameraController	m_CameraController;
		
		// Scene
		Ref<Scene>				m_AktiveScene;
		Ref<Scene>				m_EditorScene;
		Ref<Scene>				m_NextScene = nullptr;

		Ref<Shader>				m_Filtering;
		Ref<Texture>			m_FinaleImage;
		Ref<Texture>			m_FrameImage;

		// Selected Entity
		Entity					m_HoveredEntity;

		// Statats
		SceneState m_SceneState = SceneState::Edit;
		bool m_Runtime = false;
		bool m_PrimeCamera = false;

		// Renderer
		bool m_RendereWirframe = false;
		bool m_RendereDepthe = true;
		CallFace m_CallFace;

		float m_PasTime = 0.0f; // Not Current Finale Iplementet 
		int m_GizmoType = -1;
		// Window
		bool m_ViewPortFocused = false;
		bool m_ViewPortHoverd = false;

		glm::vec2 m_ViewPortSize = { 0.0f,0.0f };
		glm::vec2 m_ViewportBounds[2];

		// Panels
		RendererPannel							m_RendererPannel;
		SceneHierachyPannel						m_Scene_HPanel;
		ContentBrowserPannel					m_Content_BPannel;
		MenuBarPannel							m_MenuBarPannel;
		ProjectPannel							m_ProjectPannel;
		ViewPortPannel							m_ViewPortPannel;
		MeshPannel								m_MeshPannel;
		// Paths
		std::filesystem::path	m_EditorScenePath;

		// Project
		Ref<Project> m_Project;

		std::mutex						s_WorkingThreadMutex;
		std::vector<std::future<void>>	s_WorkingThread;

		Ref<EditorAssetManegerThreade> m_AssetManger;

		int64_t m_ViewPortUpdateTime;
		int64_t m_ViewPortRenderTime;

	private:
		friend class RendererPannel;
		friend class MenuBarPannel;
	};
}

