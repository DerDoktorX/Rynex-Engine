#pragma once

#include <Rynex/Core/TimeStep.h>
#include <Rynex/Core/UUID.h>
#include <Rynex/Asset/Base/Asset.h>
#include <Rynex/Renderer/Camera/EditorCamera.h>
#include <Rynex/Renderer/API/Framebuffer.h>
#include <Rynex/Scene/ScenePrototyps.h>
#include <Rynex/Core/LodePromis.h>

#include <entt.hpp>

#include <Rynex/Renderer/Rendering/Renderer.h>
// #define RY_RELATION_SHIPS_ENTITY_BASED
namespace Rynex {
	
#pragma region PrototypenComponents
	template<typename ...Args>
	using EnttViewComponents = entt::basic_view< enum entt::entity, entt::exclude_t<>, Args...>;

	using EnttRender2DView				= EnttViewComponents<ModelMatrixComponent, SpriteRendererComponent>;
	using EnttRenderTextView			= EnttViewComponents<ModelMatrixComponent, TextComponent>;
	using EnttRender3DDynamicModelView	= EnttViewComponents<ModelMatrixComponent, DynamicMeshComponent>;
	using EnttRender3DStaticModelView	= EnttViewComponents<ModelMatrixComponent, ModelMangerComponent>;
	using EnttRender3DSingleStaticModelView	= EnttViewComponents<ModelMatrixComponent, StaticMeshComponent>;

	using EnttFrameBufferView	= EnttViewComponents<ModelMatrixComponent, CameraComponent, FrameBufferComponent>;
	using EnttCameraView		= EnttViewComponents<ModelMatrixComponent, CameraComponent>;
	using EnttPartikelView		= EnttViewComponents<ModelMatrixComponent, ParticelComponente>;
	
	// Ligthts
	using EnttDrirektionLigthView	= EnttViewComponents<ModelMatrixComponent, DrirectionleLigthComponent>;
	using EnttPointLigthView		= EnttViewComponents<ModelMatrixComponent, PointLigthComponent>;
	using EnttSpotLigthView			= EnttViewComponents<ModelMatrixComponent, SpotLigthComponent>;
	using EnttScriptView			= EnttViewComponents<ScriptComponent>;
	using EnttRenderTargetView		= EnttViewComponents<RenderTargetComponent, CameraComponent, ModelMatrixComponent>;

#pragma endregion

	struct EnttViewLigths
	{
		EnttDrirektionLigthView drirektionLCV;
		EnttPointLigthView pointLCV;
		EnttSpotLigthView spotLCV;
	};

	struct EnttView3D
	{
		EnttRender3DDynamicModelView dynamicModelCV;
		EnttRender3DStaticModelView staticCV;
		EnttRender3DSingleStaticModelView singleStaticCV;
	};

	struct EnttView2D
	{
		EnttRender2DView renderer2DCV;
		EnttRenderTextView rendererTextCV;
	};
	
	class SceneRenderer;

	class RYNEX_API Scene : public Asset
	{
		
	public:
		Scene();
		~Scene();

		static Ref<Scene> Copy(Ref<Scene> other);
		static void CopyComponentToEntity(Entity dst, Entity src);
		static void SwapComponentFromEntitys(Entity a, Entity b);
		virtual AssetType GetType() const override { return AssetType::Scene; }

		Entity CreateEntity(const std::string& name = std::string(""));
		Entity CreateEntityWitheUUID(UUID uuid, const std::string& name = std::string(""), int index = -1);
		int64_t Get3DSubmitTime() const { return m_TimeElpassed3DSubmit; }
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

		uint32_t GetEntityCount() const { return static_cast<uint32_t>(m_Registery.size()); }

		template<typename... Components>
		auto GetAllEntitiesWith()
		{
			return m_Registry.view<Components...>();
		}
		
		void SetFuncSubmit3DSceneDrawListToFrame(const std::function<void()>& func);


		void OnConectToRenderer();
		void OnDisconectToRenderer();

		static Ref<Scene> GetRefInPlace(Scene* scenePtr);
	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);		
		
		static void Submit2DCamerIcons(EnttCameraView& cameraView);
		static void Submit2DDrirectionLigthIcons(EnttDrirektionLigthView& drirektionLigthView);
		static void Submit2DPointLigthIcons(EnttPointLigthView& pointLigthView);
		static void Submit2DSpotLigthIcons(EnttSpotLigthView& spotLigthView);

		static void Submit2DEntitys(EnttRender2DView& view2dQuads);
		static void Submit2DTextEntitys(EnttRenderTextView& view2dText);

		static void Submit3DPointLigth(EnttPointLigthView& pointLigthView);
		static void Submit3DSpotLigth(EnttSpotLigthView& spotLigthView);
		static void Submit3DDrirectionLigth(EnttDrirektionLigthView& drirektionLigthView);

		static void SubmitLigtheViews(EnttViewLigths& ligths);
		static void RenderRenderTaregtView(EnttRenderTargetView& renderTargetView, EnttRender3DStaticModelView& view3dStaticMesh);
		static void SubmitRenderTaregtCurent(Camera& camera, const glm::mat4& model, RenderTargetComponent& targetC);
		static void SubmitRenderTaregtMain(Camera& camera, const glm::mat4& model, RenderTargetComponent& targetC);

		void RenderNowMain();
		static void RenderNowCurent();
		void ResetRenderTaregtMain();
		static void ResetRenderTaregtCurent();


		static void Submit3DStaticeEntitysRenderProxy(EnttRender3DStaticModelView& view3dStaticMesh);


		static void Submit3DStaticeEntitys(EnttRender3DStaticModelView& view3dStaticMesh, int64_t* timerPtr);
		static void Submit3DStaticeEntitysMain(EnttRender3DStaticModelView& view3dStaticMesh, int64_t* timerPtr);
		static void Submit3DStaticeEntitysCurent(EnttRender3DStaticModelView& view3dStaticMesh);

		static void Submit3DSingleStaticeEntitys(EnttRender3DSingleStaticModelView& view3dSingleStaticMesh, int64_t* timerPtr);

		static void Submit3DDataStaticeEntitys(EnttRender3DStaticModelView& view3dStaticMesh, int64_t* timerPtr);
		static void Submit3DDataSingleStaticeEntitys(EnttRender3DSingleStaticModelView& view3dSingleStaticMesh, int64_t* timerPtr);

		static void Submit3DDynamicEntitys(EnttRender3DDynamicModelView& view3dDynamicMesh);


		void ClearAll();
		

		static void OnEntityModelMatrixCreate(entt::registry& registry, entt::entity entity);
		static void OnEntityModelMatrixChanged(entt::registry& registry, entt::entity entity);
		static void OnEntityModelMatrixDestroy(entt::registry& registry, entt::entity entity);

		static void OnEntityStaticMeshCreate(entt::registry& registry, entt::entity entity);
		static void OnEntityStaticMeshChanged(entt::registry& registry, entt::entity entity);
		static void OnEntityStaticMeshDestroy(entt::registry& registry, entt::entity entity);

		static void OnEntityStaticSingleMeshCreate(entt::registry& registry, entt::entity entity);
		static void OnEntityStaticSingleMeshChanged(entt::registry& registry, entt::entity entity);
		static void OnEntityStaticSingleMeshDestroy(entt::registry& registry, entt::entity entity);

	private:
		entt::registry m_Registery;

		std::vector<Ref<LodePromis<Scene>>> m_LodingPromisVec;
		Ref<RenderTarget> m_MainTartget;

		
		glm::vec2 m_MausPixlePos = { -1.0, -1.0 };
		glm::vec4 m_BackGround = { 0.45,0.45f, 0.45f,1.0f };

		int64_t m_TimeElpassed3DSubmit;
		uint32_t m_ViewPortWithe;
		uint32_t m_ViewPortHeigth;

		int m_StepFrames;
		int m_EntityLeangth;
		int m_RedererDefaultModeFlags;

		entt::entity m_ViewPortSelected;
		



		bool m_Hovered;
		bool m_Resized;


		bool m_Running;
		bool m_Paused;
	private:

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierachyPannel;
		friend class SceneRenderer;
	};

	// template<typename T>
	// using SceneLodePromis = LodePromis<T, Scene, int>;
	

}


