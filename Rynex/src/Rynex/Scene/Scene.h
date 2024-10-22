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
	struct AmbientLigthComponent;
	struct DrirektionleLigthComponent;
	struct PointLigthComponent;
	struct SpotLigthComponent;
	struct ParticelComponente;

	using EnttRender2DView		= entt::basic_view<enum entt::entity, entt::exclude_t<>, Matrix4x4Component, SpriteRendererComponent>;
	using EnttRender3DEditeView = entt::basic_view<enum entt::entity, entt::exclude_t<>, Matrix4x4Component, MaterialComponent, GeomtryComponent>;
	using EnttRender3DDynamicModelView = entt::basic_view<enum entt::entity, entt::exclude_t<>, Matrix4x4Component, MaterialComponent, DynamicMeshComponent>;
	using EnttRender3DStaticModelView = entt::basic_view<enum entt::entity, entt::exclude_t<>, Matrix4x4Component, MaterialComponent, StaticMeshComponent>;

	using EnttFrameBufferView	= entt::basic_view<enum entt::entity, entt::exclude_t<>, Matrix4x4Component, FrameBufferComponent, CameraComponent>;
	using EnttCameraView		= entt::basic_view<enum entt::entity, entt::exclude_t<>, Matrix4x4Component, CameraComponent>;
	using EnttPartikelView		= entt::basic_view<enum entt::entity, entt::exclude_t<>, Matrix4x4Component, ParticelComponente>;
	
	// Ligthts
	using EnttAmbientLView		= entt::basic_view<enum entt::entity, entt::exclude_t<>, AmbientLigthComponent>;
	using EnttDrirektionLeLView = entt::basic_view<enum entt::entity, entt::exclude_t<>, Matrix4x4Component, DrirektionleLigthComponent>;
	using EnttPointLView		= entt::basic_view<enum entt::entity, entt::exclude_t<>, Matrix4x4Component, PointLigthComponent>;
	using EnttSpotLView			= entt::basic_view<enum entt::entity, entt::exclude_t<>, Matrix4x4Component, SpotLigthComponent>;

	using EnttScriptView		= entt::basic_view<enum entt::entity, entt::exclude_t<>, ScriptComponent>;
	using EnttEntity			= entt::entity;
	

	struct EnttViewLigths
	{
		EnttAmbientLView AmbientLCV;
		EnttDrirektionLeLView DrirektionLCV;
		EnttPointLView PointLCV;
		EnttSpotLView SpotLCV;
		EnttViewLigths(EnttAmbientLView& ambientLCV, EnttDrirektionLeLView& drirektionLCV, EnttPointLView& pointLCV, EnttSpotLView& spotLCV)
			: AmbientLCV(ambientLCV), DrirektionLCV(drirektionLCV), PointLCV(pointLCV), SpotLCV(spotLCV) { }
		EnttViewLigths(const EnttViewLigths&) = default;
		EnttViewLigths(EnttViewLigths&&) = default;
	};

	struct EnttView3D
	{
		EnttRender3DDynamicModelView DynamicModelCV;
		EnttRender3DStaticModelView StaticCV;
		EnttRender3DEditeView EditeCV;

		EnttView3D(EnttRender3DDynamicModelView& dynamicModelCV, EnttRender3DStaticModelView& staticCV, EnttRender3DEditeView& editeCV)
			: DynamicModelCV(dynamicModelCV), StaticCV(staticCV), EditeCV(editeCV) {  }
		EnttView3D(const EnttView3D&) = default;
		EnttView3D(EnttView3D&&) = default;
	};

	struct EnttView2D
	{
		EnttRender2DView RendererCV;

		EnttView2D(EnttRender2DView& rendererCV)
			: RendererCV(rendererCV) { }

		EnttView2D(const EnttView2D&) = default;
		EnttView2D(EnttView2D&&) = default;
	};

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
		
		void SetBeckGroundColor(const glm::vec4& backGound) { m_BackGround = backGound; }

		Entity GetEntitiyByUUID(UUID uuid);
		Entity GetEntityByName(const std::string& tag);
		Entity GetEntityPrimaryCamera();

		bool IsTagInScene(const std::string& tag);


		uint32_t GetEntityCount() const { return (uint32_t)m_Registery.size(); }

		// void SetRenderFramBuffer(const std::string& viewPortName, const Ref<Framebuffer>& framebuffer) { m_RenderFrambuffer[viewPortName] = framebuffer; }

		// void SetPaused(bool paused) { m_IsPaused = paused; }

		// void Step(int frames = 1);

		template<typename... Components>
		auto GetAllEntitiesWith()
		{
			return m_Registry.view<Components...>();
		}

		void RenderSingleEntity(Camera& camera, const glm::mat<4, 4, float>& viewMatrix, const glm::vec4& backGroundColor);

	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);		
		
		void RenderScene2D(Camera& camera, glm::mat<4, 4, float>& transform, EnttView2D& enttView2D);
		void RenderScene3D(Camera& camera, glm::mat<4, 4, float>& transform, EnttView3D& enttView3D);
		void RenderFrambuffers( EnttView3D& enttView3D, EnttView2D& enttView2D, EnttCameraView& enttCameraView);
		void SetLigthsRuntime(EnttViewLigths& enttViewLigths);
		void SetLigthsEditor(EnttViewLigths& enttViewLigths, Camera& camera, const glm::mat<4, 4, float>& viewMatrix, const glm::uvec2& viewPortSize);

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
		int m_RedererDefaultModeFlags = 0;
		//b2World* m_PhysicsWorld = nullptr;

		std::unordered_map<UUID, entt::entity> m_EntityMapID;
		// std::map<std::string, Ref<Framebuffer>> m_RenderFrambuffer;
		std::map<std::string, entt::entity> m_EntityMapTag;
		glm::vec4 m_BackGround = { 0.1,0.1f,0.1f,1.0f };
		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierachyPannel;
	};

	

}


