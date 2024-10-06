#pragma once

#include "Rynex/Core/TimeStep.h"
#include "Rynex/Core/UUID.h"
#include "Rynex/Asset/Base/Asset.h"
#include "Rynex/Renderer/Camera/EditorCamera.h"
#include "Rynex/Asset/Base/Asset.h"
#include "Rynex/Renderer/API/Framebuffer.h"
#include "Rynex/Renderer/Objects/Model.h"

#include <entt.hpp>

namespace Rynex {

	class Entity;

	struct TransformComponent;
	struct CameraComponent;
	struct SpriteRendererComponent;
	struct MaterialComponent;
	struct GeomtryComponent;
	struct ScriptComponent;
	struct FrameBufferComponent;
	struct StaticMeshComponent;
	struct DynamicMeshComponent;
	struct RealtionShipComponent;
	struct Matrix4x4Component;

	using EnttRender2DView		= entt::basic_view<enum entt::entity, entt::exclude_t<>, Matrix4x4Component, SpriteRendererComponent>;
	using EnttRender3DEditeView = entt::basic_view<enum entt::entity, entt::exclude_t<>, Matrix4x4Component, MaterialComponent, GeomtryComponent>;
	using EnttRender3DDynamicModelView = entt::basic_view<enum entt::entity, entt::exclude_t<>, Matrix4x4Component, RealtionShipComponent, DynamicMeshComponent>;
	using EnttRender3DStaticModelView = entt::basic_view<enum entt::entity, entt::exclude_t<>, Matrix4x4Component, MaterialComponent, StaticMeshComponent>;

	using EnttFrameBufferView	= entt::basic_view<enum entt::entity, entt::exclude_t<>, Matrix4x4Component, FrameBufferComponent, CameraComponent>;
	using EnttCameraView		= entt::basic_view<enum entt::entity, entt::exclude_t<>, Matrix4x4Component, CameraComponent>;
	using EnttScriptView		= entt::basic_view<enum entt::entity, entt::exclude_t<>, ScriptComponent>;
	using EnttEntity			= entt::entity;
	
	class RYNEX_API Scene : public Asset
	{
	public:
		Scene();
		~Scene();

		static Ref<Scene> Copy(Ref<Scene> other);

		virtual AssetType GetType() const { return AssetType::Scene; }

		Entity CreateEntity(const std::string& name = std::string(""));
		Entity CreateEntityWitheUUID(UUID uuid, const std::string& name = std::string(""), int index = -1);

		void OnRuntimStart();
		void OnRuntimStop();

		void DestroyEntity(Entity entity);
		//entt::registry& Reg() { return m_Registery; }

		void OnUpdateEditor(TimeStep ts);
		void OnRenderEditor(const Ref<Framebuffer>& framebuffer, const Ref<EditorCamera>& editorCamera);

		void OnUpdateRuntime(TimeStep ts);
		void OnRenderRuntime(const Ref<Framebuffer>& framebuffer, int camera = 0);

		void OnUpdateSimulation(TimeStep ts);
		void OnRenderSimulation(const Ref<Framebuffer>& framebuffer, const Ref<EditorCamera>& editorCamera);

		void OnViewportResize(uint32_t withe, uint32_t heigth);
		

		Entity GetEntitiyByUUID(UUID uuid);

		bool IsTagInScene(const std::string& tag);

		Entity GetPrimaryCameraEntity();
		uint32_t GetEntityCount() const { return (uint32_t)m_Registery.size(); }

		// void SetRenderFramBuffer(const std::string& viewPortName, const Ref<Framebuffer>& framebuffer) { m_RenderFrambuffer[viewPortName] = framebuffer; }

		//void SetPaused(bool paused) { m_IsPaused = paused; }

		//void Step(int frames = 1);

		template<typename... Components>
		auto GetAllEntitiesWith()
		{
			return m_Registry.view<Components...>();
		}
		

	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);		
		
		void RenderScene2D(Camera& camera, glm::mat<4, 4, float>& transform, EnttRender2DView& enttRender2DView);
		void RenderScene3D(Camera& camera, glm::mat<4, 4, float>& transform, EnttRender3DEditeView& enttRender3DEditeView, EnttRender3DStaticModelView& enttRender3DStaticModelView, EnttRender3DDynamicModelView& enttRender3DDynamicModelView);
		void RenderFrambuffers(EnttRender3DEditeView& enttRender3DEditeView, EnttRender3DStaticModelView& enttRender3DStaticModelView, EnttRender3DDynamicModelView& enttRender3DDynamicModelView, EnttRender2DView& enttRender2DView);

		void SceneRendering(Camera& camera, glm::mat<4, 4, float>& viewMatrix);

		void EditorFilterSreene();
		void ClearAll();
	private:
		entt::registry m_Registery;
		uint32_t m_ViewPortWithe = 1, m_ViewPortHeigth = 1;
		bool m_IsRunning = false;
		bool m_IsPaused = false;
		int m_StepFrames = 0;
		int m_EntityLeangth = 0;
		// Ref<Framebuffer>	m_RendererFramebuffer;
		
		//b2World* m_PhysicsWorld = nullptr;

		std::unordered_map<UUID, entt::entity> m_EntityMapID;
		// std::map<std::string, Ref<Framebuffer>> m_RenderFrambuffer;
		std::map<std::string, entt::entity> m_EntityMapTag;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierachyPannel;
	};

	

}


