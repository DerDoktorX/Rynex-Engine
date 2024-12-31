#pragma once

#include <Rynex/Renderer/Camera/CamerController.h>
#include <Rynex/Renderer/API/Texture.h>
#include <Rynex/Scene/Scene.h>
#include <Rynex/Scene/Entity.h>

#include <glm/gtc/type_ptr.hpp>



namespace Rynex {
	class EditorLayer;
	class ViewPort;

	enum class SceneState;
	

	class ViewPortPannel
	{
	public:
		ViewPortPannel();
		~ViewPortPannel();

		void OnAttache(const std::string& name, EditorLayer* editorLayer);
		void OnDetache();

		void OnUpdate();

		void OnRenderEditor(TimeStep ts);
		void OnRenderRuntime(int camerCount = 0);
		void OnRenderSimultion();

		

		bool OnImGuiRender();
		static void SetEventBlocker(bool blockEvents);

		void OnEventCamera(Event& e);
		bool OnMousPressed(MouseButtenPressedEvent& e);
		

		const glm::vec2& GetWindowSize() const { return m_WindowSize; }
		const glm::vec2* GetWindowBounds() const { return m_WindowBounds; }
		
		void SetNewAktiveSecen(const Ref<Scene>& scene);
		
	private:
		void WindowResize(const glm::vec2& windowSize);
		void DragAndDrop();

		void OpenScene(AssetHandle handle);
		void OpenScene();

		void CalculateMausePos(const glm::vec2& mausOffset);
		void CalculateGizmo();
		void RenderSelectedEntity();
		void HoveredEntity();
		
	private:
		glm::vec2					m_WindowSize = { 0.0f,0.0f };
		glm::vec2					m_WindowBounds[2];

		bool						m_WindowFocused = false;
		bool						m_WindowHoverd = false;
		bool						m_ImGizmoHovered = false;

		Ref<Scene>					m_AktiveScene = nullptr;
		Ref<Texture>				m_Image = nullptr;
		Ref<EditorCamera>			m_Camera = nullptr;
		Ref<Shader>					m_Filtering = nullptr;
		SceneState					m_SceneState;
		

		EditorLayer*				m_EditorLayer = nullptr;

		Ref<Framebuffer>			m_Framebuffer = nullptr;
		Ref<Framebuffer>			m_SelectedFramebuffer = nullptr;
		std::string					m_WindowName;

		int							*m_GizmoType = nullptr;
		Entity						m_HoveredEntity;
		std::vector<Ref<ViewPort>>	m_ViewPorts;
		MaterialComponent			m_MaterialC;
	};


	class ViewPort 
	{
	public:
		ViewPort();
		~ViewPort();

		void OnAttache(const Ref<Framebuffer>& frambuffer, const Ref<Framebuffer>& selectedFrameBuffer, const Ref<EditorCamera>& editorCamera, const Ref<Shader>& shader, EditorLayer* editorLayer, SceneState* sceneState, ViewPortPannel* viewPortPannel, Entity* hoveredEntity,int renderOnAtachment = 0, int runtimCameraIndex = 0, const std::string& name  = "ViewPort");
		void OnDetache();
		void OnUpdate(const glm::vec2& mousePos);

		bool OnImGuiRender();

		void OnRenderEditorFilter();

		void SetNewAktiveSecen(const Ref<Scene>& scene);

		const glm::vec2& GetWindowSize() const { return m_WindowSize; }
		const glm::vec2* GetWindowBounds() const { return m_WindowBounds; }

		bool IsWindowFocused() const { return m_WindowFocused; }
		bool IsWindowHoverd() const { return m_WindowHoverd; }
		bool IsGizmoHoverd() const { return m_ImGizmoHovered; }
	private:
		void CalculateMausePos(const glm::vec2& mausOffset);
		void WindowResize(const glm::vec2& windowSize);
		void DragAndDrop();

		void OpenScene(AssetHandle handle);
		void OpenScene();

		void CalculateGizmo();

		void RenderSelectedEntity();
		void HoveredEntity(const glm::vec2& mousePos);
	private:
		
		Ref<Scene>					m_AktiveScene = nullptr;
		Ref<Texture>				m_Image = nullptr;
		Ref<EditorCamera>			m_Camera = nullptr;
		Ref<Shader>					m_Filtering = nullptr;

		Ref<Framebuffer>			m_Framebuffer = nullptr;
		Ref<Framebuffer>			m_SelectedFramebuffer = nullptr;
		Weak<Texture>				m_ShadowMap;
		SceneState*					m_SceneState = nullptr;
		EditorLayer*				m_EditorLayer = nullptr;
		ViewPortPannel*				m_vViewPortPannel = nullptr;
		Entity*						m_HoveredEntity = nullptr;

		int*						m_GizmoType = nullptr;
		int							m_RenderOnAtachment = 0;
		int							m_RuntimCameraIndex = 0;

		glm::vec2					m_WindowSize = { 1.0f, 1.0f };
		glm::vec2					m_Multypler = { 1.0f, 1.0f };
		glm::vec2					m_ContentSize = { 1.0f, 1.0f };

		glm::vec2					m_WindowBounds[2];
		glm::vec2					m_MauseInViewPixelPos = {-1.5f, -1.5f };
		bool						m_WindowFocused = false;
		bool						m_WindowHoverd = false;
		bool						m_ImGizmoHovered = false;
		std::string					m_WindowName = "";

		
	};
}
