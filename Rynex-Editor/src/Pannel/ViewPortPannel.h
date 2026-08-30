#pragma once

#include <Rynex/Renderer/Camera/CamerController.h>
#include <Rynex/Renderer/API/Texture.h>
#include <Rynex/Renderer/Mesh/MeshStatic.h>
#include <Rynex/Scene/Scene.h>
#include <Rynex/Scene/Entity.h>

// #include <Rynex/Renderer/Rendering/Render3D/SingleMultyRender.h>

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

		void OpenTextureWindows();

		bool OnImGuiRender();
		static void SetEventBlocker(bool blockEvents);

		void OnEventCamera(Event& e);
		bool OnMousPressed(MouseButtenPressedEvent& e);
		const std::vector<Ref<ViewPort>>& GetViewPorts() const { return m_ViewPorts; }

		const glm::vec2& GetWindowSize() const { return m_WindowSize; }
		const glm::vec2* GetWindowBounds() const { return m_WindowBounds; }
		glm::ivec2* GetStaicResultionSizePtr() { return &m_StaticGameReslution; }

		void SetNewAktiveSecen(const Ref<Scene>& scene);
		bool* GetStaicResultionState() { return &m_StaticReslution; }
		
		void OnStaicResultion();

		void SetComputeInvocation(const glm::vec3& invocation);
		int64_t GetSceneRenderTime() const { return m_SceneRenderUpdateTime; };
		Ref<Texture> GetFinaleImage() const;
		static void RenderEntityAndParentRecusiv(int* drawCount, Entity e);
	private:
		void WindowResize(const glm::vec2& windowSize);
		void DragAndDrop();

		void OpenScene(AssetHandle handle);
		void OpenScene();

		void CalculateMausePos(const glm::vec2& mausOffset);
		void CalculateGizmo();
		bool RenderSelectedEntity();
		void HoveredEntity();
	private:
		uint32_t					m_TextureSampleCount = 1u;
		
		glm::vec2					m_WindowSize = { 1.0f,1.0f };
		glm::ivec2					m_StaticGameReslution = { 48, 48 };
		glm::vec2					m_WindowBounds[2];

		bool						m_WindowFocused = false;
		bool						m_WindowHoverd = false;
		bool						m_ImGizmoHovered = false;
		bool						m_StaticReslution = false;
		bool						m_BeforSelected = false;

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
		int64_t						m_SceneRenderUpdateTime;
		RenderTarget				m_SelctedTarget;

	};


	class ViewPort 
	{
	private:
		struct AttachmentsPannel
		{
			int Index = 0;
			bool WindowAktive = true;
			std::string Name = "";
		};
	public:
		ViewPort();
		~ViewPort();

		void OnAttache(const Ref<Framebuffer>& frambuffer, const Ref<Framebuffer>& selectedFrameBuffer, const Ref<EditorCamera>& editorCamera, const Ref<Shader>& shader
			, EditorLayer* editorLayer, SceneState* sceneState, ViewPortPannel* viewPortPannel, Entity* hoveredEntity, glm::ivec2* reslutionPtr
			,int renderOnAtachment = 0, int runtimCameraIndex = 0, const std::string& name  = "ViewPort");
		void OnDetache();
		void OnUpdate();
		void OnUpdate(const glm::vec2& mousePos);

		bool OnImGuiRender();
		void OpenTextureWindows();
		void OnRenderEditorFilterOff();
		void OnRenderEditorFilter();

		void SetNewAktiveSecen(const Ref<Scene>& scene);
		void SetComputeInvocation(const glm::vec3& invocation);
		const glm::vec2& GetWindowSize() const { return m_WindowSize; }
		const glm::vec2* GetWindowBounds() const { return m_WindowBounds; }

		bool IsWindowFocused() const { return m_WindowFocused; }
		bool IsWindowHoverd() const { return m_WindowHoverd; }
		bool IsGizmoHoverd() const { return m_ImGizmoHovered; }
		void OnStaicResultion(bool state);
		const glm::vec2& GetRenderSize() const { return m_Framebuffer->GetFrambufferSize(); }
		Ref<Texture> GetFinalImag() const;
	private:
		void ResizeGameResultion(const glm::vec2& size);
		bool IsCurrentWindowMoving();
		void CalculateMausePos(const glm::vec2& mausOffset);
		void WindowResize(const glm::vec2& windowSize);
		void DragAndDrop();
		
		void OpenScene(AssetHandle handle);
		void OpenScene();

		void CalculateGizmo();

		void RenderSelectedEntity();

		void HoveredEntity();
		void HoveredEntity(const glm::vec2& mousePos);
		glm::vec2 CalculateImageWindowSize(const Ref<Texture>& texture);
	
	private:
		
		Ref<Scene>						m_AktiveScene = nullptr;
		Ref<Texture>					m_Image = nullptr;
		Ref<Texture>					m_FinaleImage = nullptr;

		Ref<EditorCamera>				m_Camera = nullptr;
		Ref<Shader>						m_Filtering = nullptr;
		

		Ref<Framebuffer>				m_Framebuffer = nullptr;
		Ref<Framebuffer>				m_SelectedFramebuffer = nullptr;
		Weak<Texture>					m_ShadowMap;
		Ref<Texture>					m_FontTex;
		SceneState*						m_SceneState = nullptr;
		EditorLayer*					m_EditorLayer = nullptr;
		ViewPortPannel*					m_vViewPortPannel = nullptr;
		Entity*							m_HoveredEntity = nullptr;

		int*							m_GizmoType = nullptr;
		int								m_RenderOnAtachment = 0;
		int								m_RuntimCameraIndex = 0;
		uint32_t						m_TextureSampleCount = 1u;
		float							m_TextureScale = 1.0f;

		glm::vec2						m_WindowSize = { 1.0f, 1.0f };
		glm::vec2						m_Multypler = { 1.0f, 1.0f };
		glm::vec2						m_ContentSize = { 1.0f, 1.0f };
		glm::ivec2*						m_StaticGameReslutionPtr = nullptr;
		glm::vec2						m_StaticResultionScreenOfset = { 1, 1 };

		glm::vec2						m_WindowBounds[2];
		glm::vec2						m_MauseInViewPixelPos = { -1.5f, -1.5f };
		glm::vec2						m_MousePos = { -1.5f, -1.5f };
		glm::vec3						m_ComputeGrups = { 8.0f, 16.0f, 1.0f };

		bool							m_WindowFocused = false;
		bool							m_WindowHoverd = false;
		bool							m_ImGizmoHovered = false;
		bool							m_WindowMoving = false;
		bool							m_StaticReslution = false;
		bool							m_MousButtenLeftPressed = false;

		bool							m_WindowShadow = true;
		bool							m_WindowText = true;

		std::string						m_WindowName = "";
		std::vector<AttachmentsPannel>	m_ShadowBuffersAttechements;
	};
}
