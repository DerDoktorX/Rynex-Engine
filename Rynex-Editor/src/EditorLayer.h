#pragma once

#include <Rynex/Core/Layer.h>
#include <Rynex/Events/KeyEvent.h>
#include <Rynex/Core/CamerController.h>
#include <Rynex/Renderer/API/Framebuffer.h>
#include <Rynex/Renderer/RenderCommand.h>

#include "Pannel/SceneHierachyPannel.h"
#include "Pannel/ContentBrowserPannel.h"
#include "Pannel/ViewPortPannel.h"
#include "Pannel/RendererPannel.h"

struct ImVec2;

namespace Rynex{
#define CHECK_FOR_ERRORS 0

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

		Entity GetSelectedEntity() { return m_Scene_HPanel.GetSelectedEntity(); }
		SceneState GetSceneState() { return m_SceneState; }
		int* GetPtrGizmoType();

		const Ref<Scene>& GetAktivScene() { return m_AktiveScene; }

		void SetSelectedEntity(Entity enitiy) { return m_Scene_HPanel.SetSelectedEntity(enitiy); }

		//--- Layoute ----------------

		// ViewPort 
		void ImGuiViewPort();
		void ImGuiSecundaryViewPort(const Ref<Framebuffer>& framebuffer, uint32_t id = 0, float width = 12.8f, float height = 7.2f);
		void ImGuiContentBrowserViewPort();
		void ImGuiViewPortResize( ImVec2 vPSize);
		void ImGuiSetMausPosInViewPort( ImVec2 vpOffset);
		void ImGizmoInViewPort();

		void RenderSelectedEntity(Entity slelcted);
		void RenderHoveredEntity(Entity hovered);
		void FinaleImgaeFilterEditor();

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

	private:
		Ref<EditorCamera> m_EditorCamera;
		OrthograficCameraController	m_CameraController;
		
		// Scene
		Ref<Scene>				m_AktiveScene;
		Ref<Scene>				m_EditorScene;
		Ref<Framebuffer>		m_Framebuffer;
		Ref<Framebuffer>		m_SelectedFramebuffer;
		Ref<Framebuffer>		m_HoveredFramebuffer;

		Ref<Shader>				m_Filtering;
		Ref<Texture>			m_FinaleImage;
		Ref<Texture>			m_FrameImage;
#if 0
		// Camera
		Entity					m_CamerEntity;
		Entity					m_SecoundCameraEntity;
#endif
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
		std::vector<Ref<ViewPortPannel>>		m_ViewPortPannel;

		// Paths
		std::filesystem::path m_EditorScenePath;

		// Project
		Ref<Project> m_Project;
		Ref<EditorAssetManager> m_AssetManger;
	};
}

