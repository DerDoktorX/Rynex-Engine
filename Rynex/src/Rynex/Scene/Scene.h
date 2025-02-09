#pragma once

#include "Rynex/Core/TimeStep.h"
#include "Rynex/Core/UUID.h"
#include "Rynex/Asset/Base/Asset.h"
#include "Rynex/Renderer/Camera/EditorCamera.h"
#include "Rynex/Renderer/API/Framebuffer.h"
#include "Rynex/Renderer/Objects/Model.h"


#include <entt.hpp>

namespace Rynex {

#pragma region PrototypenComponents

	class Entity;

	struct CameraComponent;
	struct SpriteRendererComponent;
	struct MaterialComponent;
	struct GeomtryComponent;
	struct ScriptComponent;
	struct FrameBufferComponent;
	struct StaticMeshComponent;
	struct DynamicMeshComponent;
	struct RealtionShipComponent;
	
	struct AmbientLigthComponent;
	struct DrirektionleLigthComponent;
	struct PointLigthComponent;
	struct SpotLigthComponent;
	struct ParticelComponente;
	struct TextComponent;
	struct ViewMatrixComponent;
	struct ProjtionViewMatrixComponent;
	struct InverseProjtionViewMatrixComponent;
	struct WorldViewFustrumComponent;

	struct TransformComponent;
	struct ModelMatrixComponent;

	using EnttRender2DView		= entt::basic_view<enum entt::entity, entt::exclude_t<>, ModelMatrixComponent, SpriteRendererComponent>;
	using EnttRenderTextView		= entt::basic_view<enum entt::entity, entt::exclude_t<>, ModelMatrixComponent, TextComponent>;
	using EnttRender3DEditeView = entt::basic_view<enum entt::entity, entt::exclude_t<>, ModelMatrixComponent, MaterialComponent, GeomtryComponent>;
	using EnttRender3DDynamicModelView = entt::basic_view<enum entt::entity, entt::exclude_t<>, ModelMatrixComponent, DynamicMeshComponent>;
	using EnttRender3DStaticModelView = entt::basic_view<enum entt::entity, entt::exclude_t<>, ModelMatrixComponent, StaticMeshComponent>;

	using EnttFrameBufferView	= entt::basic_view<enum entt::entity, entt::exclude_t<>, ModelMatrixComponent, CameraComponent, FrameBufferComponent>;
	using EnttCameraView		= entt::basic_view<enum entt::entity, entt::exclude_t<>, ModelMatrixComponent, CameraComponent>;
	using EnttPartikelView		= entt::basic_view<enum entt::entity, entt::exclude_t<>, ModelMatrixComponent, ParticelComponente>;
	
	// Ligthts
	using EnttAmbientLView		= entt::basic_view<enum entt::entity, entt::exclude_t<>, AmbientLigthComponent>;
	using EnttDrirektionLeLView = entt::basic_view<enum entt::entity, entt::exclude_t<>, ModelMatrixComponent, DrirektionleLigthComponent>;
	using EnttPointLView		= entt::basic_view<enum entt::entity, entt::exclude_t<>, ModelMatrixComponent, PointLigthComponent>;
	using EnttSpotLView			= entt::basic_view<enum entt::entity, entt::exclude_t<>, ModelMatrixComponent, SpotLigthComponent>;

	using EnttScriptView		= entt::basic_view<enum entt::entity, entt::exclude_t<>, ScriptComponent>;
	using EnttEntity			= entt::entity;

	using RenderFunc = std::function<void(const glm::mat4&, const Ref<Mesh>&, const Ref<Material>&, int)>;
#pragma endregion


	struct EnttViewLigths
	{
		EnttAmbientLView AmbientLCV;
		EnttDrirektionLeLView DrirektionLCV;
		EnttPointLView PointLCV;
		EnttSpotLView SpotLCV;

	};

	struct EnttView3D
	{
		EnttRender3DDynamicModelView DynamicModelCV;
		EnttRender3DStaticModelView StaticCV;
		EnttRender3DEditeView EditeCV;

	
	};

	struct EnttView2D
	{
		EnttRender2DView Renderer2DCV;
		EnttRenderTextView RendererTextCV;
		
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

		void OnUpdateEditor(TimeStep ts);
		void OnRenderEditor(const Ref<Framebuffer>& framebuffer, const Ref<EditorCamera>& editorCamera);

		void OnUpdateRuntime(TimeStep ts);
		void OnRenderRuntime(const Ref<Framebuffer>& framebuffer, int camera = 0);

		void OnUpdateSimulation(TimeStep ts);
		void OnRenderSimulation(const Ref<Framebuffer>& framebuffer, const Ref<EditorCamera>& editorCamera);

		void OnViewportResize(uint32_t withe, uint32_t heigth);
		
		void SetBackgroundColor(const glm::vec4& backGound) { m_BackGround = backGound; }
		glm::uvec2 GetViewPortSize() { return { m_ViewPortWithe , m_ViewPortHeigth }; }
		const glm::vec2& GetMousPixelPos() { return m_MausPixlePos; }

		void SetSelectedEntity(Entity entity);

		Entity GetSelectedEntity();
		Entity GetEntitiyByUUID(UUID uuid);
		Entity GetEntityByName(const std::string& tag);
		Entity GetEntityPrimaryCamera();
		

		bool IsTagInScene(const std::string& tag);
		void SetMousPixelPos(const glm::vec2& pos) { m_MausPixlePos = pos; }
		void SetHoverViewPort(bool isHovered) { m_Hovered = isHovered; }
		void SetWindowResize(bool isResized) { m_Resized = isResized; }
		bool IsViewPortHovered() const { return m_Hovered; }

		bool IsWindowResize() const { return m_Resized; }
		bool IsCameraEntityViewFustrum();

		uint32_t GetEntityCount() const { return (uint32_t)m_Registery.size(); }

		template<typename... Components>
		auto GetAllEntitiesWith()
		{
			return m_Registry.view<Components...>();
		}

		void RenderSingleEntityLikeDefault(Entity entity, Camera& camera, const glm::mat4& viewMatrix, const glm::vec4& backGroundColor);
		void RenderSingleEntityLikeDefault(Entity entity, Entity Camera, const glm::vec4& backGroundColor);


		// This Funktion Renderer on the Frambuffer and use Chep Shaders for edge Ditection
		void RenderSingleEntityEdgeDitection(Entity entity, Camera& camera, const glm::mat4& viewMatrix, const glm::vec4& backGroundColor, const Ref<Framebuffer> frameBuffer);

	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);		
		

		static void Render3DSceneViewPortScene(Camera& camera, glm::mat4& viewMatrix, EnttView3D& enttView3D);
		void Render3DSceneFrambufferScene(Camera& camera, glm::mat4& viewMatrix, EnttView3D& enttView3D, uint32_t framebufferIndex);
		static void Render3DSceneShadowScene(EnttView3D& enttView3D);

		static void RenderScene3DDraw(EnttView3D& enttView3D, RenderFunc func);
		static void RenderScene3DSubmit(EnttView3D& enttView3D, RenderFunc func);
		static void RenderScene2D(Camera& camera, glm::mat4& viewMatrix, EnttView2D& enttView3D);
		
#if 0
		void SubmiteScene2D(EnttView2D& enttView2D);
		void SubmiteScene3D(EnttView3D& enttView3D);
		void SubmiteLigthsRuntime(EnttViewLigths& enttViewLigths);
		void SubmiteLigthsEditor(EnttViewLigths& enttViewLigths);
		
		static void RenderScene(Camera& camera, glm::mat4& transform, const Ref<Framebuffer>& frameBuffer);
#endif

		void RenderFrambuffers( EnttView3D& enttView3D, EnttView2D& enttView2D, EnttCameraView& enttCameraView);
		void SetLigthsRuntime(EnttViewLigths& enttViewLigths, EnttView3D& enttView3D, Camera& camera, const glm::mat4& viewMatrix, const glm::vec3& viewCenter);
		void SetLigthsEditor(EnttViewLigths& enttViewLigths, EnttView3D& enttView3D, Camera& camera, const glm::mat4& viewMatrix, const glm::vec3& viewCenter, const glm::uvec2& viewPortSize);

		// void RenderScene3DShadows( EnttView3D& enttView3D);

		void EditorFilterSreene();
		void ClearAll();
	private:
		entt::registry m_Registery;
		uint32_t m_ViewPortWithe = 1, m_ViewPortHeigth = 1;
		glm::vec2 m_MausPixlePos = { -1.0, -1.0 };
		bool m_Hovered = false;
		bool m_Resized = false;


		bool m_IsRunning = false;
		bool m_IsPaused = false;
		int m_StepFrames = 0;
		int m_EntityLeangth = 0;
		// Ref<Framebuffer>	m_RendererFramebuffer;
		int m_RedererDefaultModeFlags = 0;
		//b2World* m_PhysicsWorld = nullptr;

		entt::entity m_ViewPortSelected;

		std::unordered_map<UUID, entt::entity> m_EntityMapID;
		// std::map<std::string, Ref<Framebuffer>> m_RenderFrambuffer;
		std::map<std::string, entt::entity> m_EntityMapTag;
		glm::vec4 m_BackGround = { 0.1,0.1f,0.1f,1.0f };

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierachyPannel;
	};

	

}


