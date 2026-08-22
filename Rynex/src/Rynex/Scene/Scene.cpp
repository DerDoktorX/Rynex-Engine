#include "rypch.h"
#include "Scene.h"

#include "Entity.h"
#include "ScriptableEntity.h"
#if defined(RY_PLATFORM_WINDOWS) && RY_PLATFORM_WINDOWS
	#include <Rynex/Scripting/Mono/ScriptingEngine.h>
#endif
#include <Rynex/Asset/Base/AssetMetadata.h>
#include <Rynex/Asset/EditorAssetManager.h>
#include <Rynex/Project/Project.h>

#include <Rynex/Renderer/Rendering/Render2D/Renderer2D.h>
#include <Rynex/Renderer/Rendering/Render3D/Renderer3D.h>
#include <Rynex/Renderer/RenderCommand.h>
#include <Rynex/Core/Application.h>

// TODO: Move toCoreConfig.h
#define RENDERER_2D						1
#define RENDERER_3D						1
#define QUADS_DRAW						1
#define CIRCLE_DRAW						1
#define CIRCLE_DRAW						0
#define TEXT_DRAW						0
#define ENTITY_SCRIPT					1
#define NATIVE_SCRIPT					1
#define SCRIPT_CS						1
#define RY_SCENE_CAMERA_VIEW_FUSTREMS	0
#define RY_USE_3D_STAIC_MODEL			1
#define RY_ENABLE_3D_SUBMIT_DATA		0

namespace Rynex {

	namespace Utils {

		static ViewPassData GenarateViewPassData(Camera& camera, const glm::mat4& view, const glm::vec3& postion, const Ref<Framebuffer>& fb, const glm::vec4& color)
		{
			const glm::uvec2& size = fb->GetFrambufferSize();
			ViewPassData viewPassData = ViewPassData(
				camera.GetProjektion(), view, postion,
				color, glm::vec4{ size.x, size.y, 0, 0 },
				RenderMode::Death_Buffer | RenderMode::A_Buffer | RenderMode::CallFace_Back | RenderMode::Gamma,
				2.2f,
				fb
			);

			return viewPassData;
		}

		static void RenderDataMain(Camera& camera, const glm::mat4& view, const glm::mat4& model, const Ref<Framebuffer>& fb, const glm::vec4& color, Ref<RenderTarget>& renderTarget)
		{
			if (nullptr == renderTarget)
			{
				renderTarget = CreateRef<RenderTarget>(fb);
				renderTarget->SetClearColorAttachment(1, -1);
			}
			const glm::uvec2& size = fb->GetFrambufferSize();
			glm::ivec2 sizeInt = static_cast<glm::ivec2>(size);
			glm::ivec4 viewSize = { sizeInt.x, sizeInt.y, 0, 0, };

			Renderer::SetRenderTargetMain(renderTarget);
			Renderer::SetViewSizeMain(viewSize);
			Renderer::SetRenderCameraMain(model, camera);
			glm::vec4 clearColor = color;
			if (Renderer::GetBackgroundGamma())
			{
				float gammaCorection = Renderer::GetGammaValue();
				glm::vec3 backgroundRGB = glm::vec3(clearColor);
				glm::vec3 scale = glm::vec3(gammaCorection);
				glm::vec3 vecColorPow = glm::pow(backgroundRGB, scale);
				clearColor = glm::vec4(vecColorPow, color.a);
			}
			renderTarget->SetClearColorAttachment(0, clearColor);			
		}


		template<typename... Component>
		static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
		{
			([&]()
				{
					auto view = src.view<Component>();
					for (auto srcEntity : view)
					{
						entt::entity dstEntity = enttMap.at(src.get<IDComponent>(srcEntity).ID);

						auto& srcComponent = src.get<Component>(srcEntity);
						dst.emplace_or_replace<Component>(dstEntity, srcComponent);
					}
				}(), ...);
		}

		template<typename... Component>
		static void CopyComponent(ComponentGroup<Component...>, entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
		{
			CopyComponent<Component...>(dst, src, enttMap);
		}

		template<typename... Component>
		static void CopyComponentIfExists(Entity dst, Entity src)
		{
			([&]()
				{
					if (src.HasComponent<Component>())
						dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
				}(), ...);
		}

		template<typename... Component>
		static void CopyComponentIfExists(ComponentGroup<Component ...>, Entity dst, Entity src)
		{
			CopyComponentIfExists<Component...>(dst, src);
		}

		
		

		template<typename... Component>
		static void SwapComponentIfExists(Entity a, Entity b)
		{
			([&]()
				{
					if (!a.HasComponent<Component>() && !b.HasComponent<Component>())
						return;

					if (a.HasComponent<Component>() && b.HasComponent<Component>())
					{
						Component tempComp = a.GetComponent<Component>();
						a.AddOrReplaceComponent<Component>(b.GetComponent<Component>());
						b.AddOrReplaceComponent<Component>(tempComp);
					}
					else if (a.HasComponent<Component>())
					{
						b.AddOrReplaceComponent<Component>(a.GetComponent<Component>());
						a.RemoveComponent<Component>();
					}
					else if (b.HasComponent<Component>())
					{
						a.AddOrReplaceComponent<Component>(b.GetComponent<Component>());
						b.RemoveComponent<Component>();
					}


				}(), ...);
		}

		template<typename... Component>
		static void SwapComponentIfExists(ComponentGroup<Component ...>, Entity a, Entity b)
		{
			SwapComponentIfExists<Component...>(a, b);
		}

		inline static glm::mat4 CalculateDirectionShadowMapsCamera(const glm::mat4& modelMatrix, float size, float nearClip, float farClip)
		{
			return glm::ortho(-size, size, -size, size, nearClip, farClip) * glm::inverse(modelMatrix);
		}

		inline static glm::mat4 CalculateDirectionShadowMapsCamera(const glm::mat4& modelMatrix)
		{
			

			glm::mat4 porjetionOth = glm::ortho(-25.0f, 25.0f, -25.0f, 25.0f, 0.0f, 100.f);
			glm::mat4 viewIn = glm::inverse(modelMatrix);
			glm::mat4 viewLo = glm::lookAt(glm::vec3(modelMatrix[3]), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
			return porjetionOth * viewLo;
		}

		inline static glm::mat4 CalculateSpotShadowMapsCamera(const glm::mat4& modelMatrix, float outer, float farClip)
		{
			glm::mat4 porjetionPer = glm::perspective(glm::radians(45.0f)  , 2048.0f / 2048.0f, 0.01f, farClip);
			glm::mat4 viewIn = glm::inverse(modelMatrix);
			return porjetionPer * viewIn;
		}

		inline static glm::mat4 CalculateSpotShadowMapsCamera(const glm::mat4& modelMatrix, Entity& e)
		{
			
			glm::mat4 porjetionView;
			if(e.HasComponent<CameraComponent>())
			{
				CameraComponent& camerC = e.GetComponent<CameraComponent>();
				const glm::mat4& porjetion = camerC.Camera.GetProjektion();
				glm::mat4 viewIn = glm::inverse(modelMatrix);
				porjetionView = porjetion * viewIn;
			}
			else
			{
				porjetionView = CalculateSpotShadowMapsCamera(modelMatrix, 0.0f, 0.0f);
			}
			return porjetionView;
		}

		inline static glm::mat4 CalculatePointShadowMapsCamera(const glm::vec3& position, glm::vec3& direction, glm::vec3& up, float farClip)
		{
			glm::mat4 matrix = glm::lookAt((direction * 0.0f) + position, position + direction, up);
			glm::mat4 porjetionPer = glm::perspective(glm::radians(90.0f), 2048.0f / 2048.0f, 0.001f, farClip);
			return porjetionPer * matrix;
		}

		inline static void CalculatePointShadowMapsCameras(const glm::vec3& position, glm::mat4* viewProjtion, float farClip)
		{
			viewProjtion[0] = Utils::CalculatePointShadowMapsCamera(position, glm::vec3(  1.0f,  0.0f,  0.0f ), glm::vec3(  0.0f, -1.0f,  0.0f ), farClip);
			viewProjtion[1] = Utils::CalculatePointShadowMapsCamera(position, glm::vec3( -1.0f,  0.0f,  0.0f ), glm::vec3(  0.0f, -1.0f,  0.0f ), farClip);
			viewProjtion[2] = Utils::CalculatePointShadowMapsCamera(position, glm::vec3(  0.0f,  1.0f,  0.0f ), glm::vec3(  0.0f,  0.0f,  1.0f ), farClip);
			viewProjtion[3] = Utils::CalculatePointShadowMapsCamera(position, glm::vec3(  0.0f, -1.0f,  0.0f ), glm::vec3(  0.0f,  0.0f, -1.0f ), farClip);
			viewProjtion[4] = Utils::CalculatePointShadowMapsCamera(position, glm::vec3(  0.0f,  0.0f, -1.0f ), glm::vec3(  0.0f, -1.0f,  0.0f ), farClip);
			viewProjtion[5] = Utils::CalculatePointShadowMapsCamera(position, glm::vec3(  0.0f,  0.0f,  1.0f ), glm::vec3(  0.0f, -1.0f,  0.0f ), farClip);
		}
	
		template<typename T>
		static const T& OnEventFunc(std::string_view type, entt::registry& registry, entt::entity entity)
		{
			const T& compent = registry.get<T>(entity);
			std::string_view nameEntity = "Unkowne Entitiy";
			if (registry.has<TagComponent>(entity))
				nameEntity = registry.get<TagComponent>(entity).Tag;

			std::string_view nameCompent = typeid(T).name();
			RY_CORE_TRACE("{} has {} on Entity({})", nameCompent.data(), type,  nameEntity.data());
			return compent;
		}

	}

	Scene::Scene()
		: m_Registery()
		, m_LodingPromisVec()
		, m_MainTartget()

		, m_MausPixlePos({ -1.0f, -1.0f })
		, m_BackGround({ 0.45,0.45f, 0.45f,1.0f })
		, m_TimeElpassed3DSubmit(1ull)
		, m_ViewPortWithe(1u)
		, m_ViewPortHeigth(1u)

		, m_StepFrames(0)
		, m_EntityLeangth(0)
		, m_RedererDefaultModeFlags(0)
		, m_Hovered(false)
		, m_Resized(false)
		, m_Running(false)
		, m_Paused(false)
	{
		m_ViewPortSelected = entt::null;
	}

	Scene::~Scene()
	{
#if defined(RY_PLATFORM_WINDOWS) && RY_PLATFORM_WINDOWS
		Scene* scene = ScriptingEngine::GetSceneContext();
		if (scene == this)
		{
			ScriptingEngine::OnRuntimeStop();
		}
		OnDisconectToRenderer();
#endif
	}


#pragma region SceneObject

	Ref<Scene> Scene::Copy(Ref<Scene> other)
	{
		Ref<Scene> newScene = CreateRef<Scene>();
		std::vector<Ref<LodePromis<Scene>>>& lodingPromisOtherVec = other->m_LodingPromisVec;

		

		newScene->m_ViewPortWithe = other->m_ViewPortWithe;
		newScene->m_ViewPortHeigth = other->m_ViewPortHeigth;

		entt::registry& srcSceneRegistry = other->m_Registery;
		entt::registry& dstSceneRegistry = newScene->m_Registery;
		std::unordered_map<UUID, entt::entity> enttMap;

		// Create entities in new scene
		using IDComponentView = entt::basic_view<enum entt::entity, entt::exclude_t<>, IDComponent>;
		IDComponentView idView = srcSceneRegistry.view<IDComponent>();
		IDComponentView::iterator itEnde = idView.end() - 1;
		IDComponentView::iterator itBeginn = idView.begin() - 1;
	
		for (IDComponentView::iterator it = itEnde; it != itBeginn; it--)
		{
			const entt::entity& e = *it;
			UUID uuid = srcSceneRegistry.get<IDComponent>(e).ID;
			const std::string& name = srcSceneRegistry.get<TagComponent>(e).Tag;
			Entity newEntity = newScene->CreateEntityWitheUUID(uuid, name);
			enttMap[uuid] = (entt::entity)newEntity;
		}

		
		// Copy components (except IDComponent and TagComponent)
		newScene->Handle = other->Handle;


		for (Ref<LodePromis<Scene>>& promis : lodingPromisOtherVec)
		{
			promis->AddRefObject(newScene);
		}

		Utils::CopyComponent(AllComponents{}, dstSceneRegistry, srcSceneRegistry, enttMap);


		return newScene;
	}

	void Scene::CopyComponentToEntity(Entity dst, Entity src)
	{
		TagComponent tagCcopy = src.GetComponent<TagComponent>();
		IDComponent idCcopy = src.GetComponent<IDComponent>();
		Utils::CopyComponentIfExists(AllComponents{}, dst, src);
		TagComponent& tagC = src.GetComponent<TagComponent>();
		IDComponent& idC = src.GetComponent<IDComponent>();
		tagC = tagCcopy;
		idC = idCcopy;
	}

	void Scene::SwapComponentFromEntitys(Entity a, Entity b)
	{
		Ref<Scene> scene = a.GetScene();
		if (b.GetScene() != scene || nullptr != scene || a.IsNotVaild() || b.IsNotVaild())
			return;


		for (Ref<LodePromis<Scene>>& promis  : scene->m_LodingPromisVec)
		{
			if(promis != nullptr && !promis->IsTransferComplet())
			{
				RY_CORE_FATAL("We have befor doing some swaping first Loade All Assets!");
				return;
			}
		}
		scene->m_LodingPromisVec.clear();

		TagComponent tagCompTemp = a.GetComponent<TagComponent>();
		IDComponent idCompTemp = a.GetComponent<IDComponent>();


		TagComponent& aTagComp = a.GetComponent<TagComponent>();
		IDComponent& aIDComp = a.GetComponent<IDComponent>();

		TagComponent& bTagComp = a.GetComponent<TagComponent>();
		IDComponent& bIDComp = a.GetComponent<IDComponent>();
		aTagComp = bTagComp;
		aIDComp = bIDComp;

		bTagComp = tagCompTemp;
		bIDComp = idCompTemp;
		

		Utils::SwapComponentIfExists(AllComponents{}, a, b);
		
	}

	

	void Scene::SetFuncSubmit3DSceneDrawListToFrame(const std::function<void()>& func)
	{
		Renderer3D::ResetTargetRenderPtr();
	}

	void Scene::OnConectToRenderer()
	{
		OnDisconectToRenderer();

		
		RY_CORE_INFO("Conect Scene to Renderer!");
		m_Registery.on_construct<ModelMatrixComponent>().connect<&Scene::OnEntityModelMatrixCreate>();
		m_Registery.on_update<ModelMatrixComponent>().connect<&Scene::OnEntityModelMatrixChanged>();
		m_Registery.on_destroy<ModelMatrixComponent>().connect<&Scene::OnEntityModelMatrixDestroy>();

		m_Registery.on_construct<ModelMangerComponent>().connect<&Scene::OnEntityStaticMeshCreate>();
		m_Registery.on_update<ModelMangerComponent>().connect<&Scene::OnEntityStaticMeshChanged>();
		m_Registery.on_destroy<ModelMangerComponent>().connect<&Scene::OnEntityStaticMeshDestroy>();

		m_Registery.on_construct<StaticMeshComponent>().connect<&Scene::OnEntityStaticSingleMeshCreate>();
		m_Registery.on_update<StaticMeshComponent>().connect<&Scene::OnEntityStaticSingleMeshChanged>();
		m_Registery.on_destroy<StaticMeshComponent>().connect<&Scene::OnEntityStaticSingleMeshDestroy>();
		
		Submit3DStaticeEntitysRenderProxy(m_Registery.view<ModelMatrixComponent, ModelMangerComponent>());
		
	}

	void Scene::OnDisconectToRenderer()
	{
		RY_CORE_INFO("Diconect Scene from Renderer!");
		entt::sink sinkCreateModelMatrixC = m_Registery.on_construct<ModelMatrixComponent>();
		if (!sinkCreateModelMatrixC.empty())
			sinkCreateModelMatrixC.disconnect();

		entt::sink sinkUpdateModelMatrixC = m_Registery.on_update<ModelMatrixComponent>();
		if (!sinkUpdateModelMatrixC.empty())
			sinkUpdateModelMatrixC.disconnect();

		entt::sink sinkDestroyModelMatrixC = m_Registery.on_destroy<ModelMatrixComponent>();
		if (!sinkDestroyModelMatrixC.empty())
			sinkDestroyModelMatrixC.disconnect();


		entt::sink sinkCreateStaticMeshC = m_Registery.on_construct<ModelMangerComponent>();
		if (!sinkCreateStaticMeshC.empty())
			sinkCreateStaticMeshC.disconnect();

		entt::sink sinkUpdateStaticMeshC = m_Registery.on_update<ModelMangerComponent>();
		if (!sinkUpdateStaticMeshC.empty())
			sinkUpdateStaticMeshC.disconnect();
		
		entt::sink sinkDestroyStaticMeshC = m_Registery.on_destroy<ModelMangerComponent>();
		if (!sinkDestroyStaticMeshC.empty())
		{
			sinkDestroyStaticMeshC.disconnect();
			Renderer3D::ClearRenderProxy();
		}


		entt::sink sinkCreateStaticSingleMeshC = m_Registery.on_construct<StaticMeshComponent>();
		if (!sinkCreateStaticSingleMeshC.empty())
			sinkCreateStaticSingleMeshC.disconnect();

		entt::sink sinkUpdateStaticSingleMeshC = m_Registery.on_update<StaticMeshComponent>();
		if (!sinkUpdateStaticSingleMeshC.empty())
			sinkUpdateStaticSingleMeshC.disconnect();

		entt::sink sinkDestroyStaticSingleMeshC = m_Registery.on_destroy<StaticMeshComponent>();
		if (!sinkDestroyStaticSingleMeshC.empty())
			sinkDestroyStaticSingleMeshC.disconnect();

		
	}

	

	void Scene::ClearAll()
	{
		m_Registery.clear();
		m_ViewPortWithe = 1;
		m_ViewPortHeigth = 1;
		m_Running = false;
		m_Paused = false;
		m_StepFrames = 0;
		m_EntityLeangth = 0;
	}

#pragma endregion


#pragma region Entity

	Entity Scene::CreateEntity(const std::string& name)
	{
		return CreateEntityWitheUUID(UUID(), name);
	}

	Entity Scene::CreateEntityWitheUUID(UUID uuid, const std::string& name, int index)
	{
		Entity entity = index == -1 ? Entity(m_Registery.create(), this) : Entity(m_Registery.create((entt::entity)index), this);

		entity.AddComponent<IDComponent>(uuid);
		entity.AddComponent<TransformComponent>();


		entity.AddComponent<RealtionShipUUIDComponent>();

		entity.AddComponent<ModelMatrixComponent>();
		entity.AddComponent<VisibleComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;

		m_EntityLeangth++;
		return entity;
	}
	
	void Scene::DestroyEntity(Entity entity)
	{
		std::string name = entity.GetTagName();
		UUID id = entity.GetUUID();
		uint64_t idUint = id;

		entity.DestroyEntity();
		

		RY_CORE_INFO("Destroy Entity: {} {}", name.c_str(), idUint);
		if (entity == Entity{ m_ViewPortSelected, this })
			m_ViewPortSelected = entt::null;
		m_Registery.destroy(entity);
		
		m_EntityLeangth--;
	}

	void Scene::SetSelectedEntity(Entity entity)
	{
		m_ViewPortSelected = (entt::entity)entity;
	}

	Entity Scene::GetSelectedEntity()
	{
		return Entity{ m_ViewPortSelected, this };
	}

	Entity Scene::GetEntitiyByUUID(UUID uuid)
	{
		auto viewTag = m_Registery.view<IDComponent>();
		for (auto& entityTag : viewTag)
		{
			IDComponent& tagC = viewTag.get<IDComponent>(entityTag);
			if (tagC.ID == uuid)
			{
				return Entity{ entityTag, this };
			}
		}
		return Entity();
	}

	Entity Scene::GetEntityByName(const std::string& tag)
	{
	

		auto viewTag = m_Registery.view<TagComponent>();
		for (auto& entityTag : viewTag)
		{
			const auto& tagC = viewTag.get<TagComponent>(entityTag);
			if (tagC.Tag == tag)
			{
				
				return Entity{ entityTag, this };
			}
		}
		return Entity();
	}

	Entity Scene::GetEntityPrimaryCamera()
	{
		auto view = m_Registery.view<CameraComponent>();
		for (auto entity : view)
		{
			const auto& camera = view.get<CameraComponent>(entity);
			if (camera.Primary)
				return Entity{ entity, this };
		}
		return {};
	}

	bool Scene::IsTagInScene(const std::string& tag)
	{
		auto viewTag = m_Registery.view<TagComponent>();
		for (auto& entityTag : viewTag)
		{
			const auto& tagC = viewTag.get<TagComponent>(entityTag);
			if (tagC.Tag == tag)
				return true;
		}
		return false;
	}

	bool Scene::IsCameraEntityViewFustrum()
	{
		auto cameraView = m_Registery.view<CameraComponent>();
		for (auto camerE : cameraView)
		{
			auto& camerC = cameraView.get<CameraComponent>(camerE);
			if (camerC.ViewFustrum)
				return true;
		}
		return false;
	}
	
	

#pragma endregion


#pragma region Runtime

	void Scene::OnRuntimStart()
	{
#if defined(RY_PLATFORM_WINDOWS) && RY_PLATFORM_WINDOWS
		ScriptingEngine::OnRuntimeStart(this);
		// Instandiat

		auto view = m_Registery.view<ScriptComponent>();
		
		for (auto e : view)
		{
			Entity entity = { e, this };
			ScriptingEngine::OnCreatEntity(entity);
				
		}
#endif
	}

	void Scene::OnRuntimStop()
	{
#if defined(RY_PLATFORM_WINDOWS) && RY_PLATFORM_WINDOWS
		auto view = m_Registery.view<ScriptComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			RY_CORE_ASSERT(entity);
			ScriptingEngine::OnDestroyEntity(entity);
		}
		ScriptingEngine::OnRuntimeStop();
#endif

	}


	void Scene::OnUpdateRuntime(TimeStep ts)
	{
		RY_PROFILE_SCOPE("Scene-OnUpdateRuntime");
		auto scriptView = m_Registery.view<ScriptComponent>();

		m_Registery.view<NativeSripteComponent>().each([=](auto entity, auto& nsc)
			{
				if (!nsc.Instance)
				{
					nsc.Instance = nsc.InstantiateScript();
					nsc.Instance->m_Entity = Entity{ entity , this };
					nsc.Instance->OnCreate();
				}
				nsc.Instance->OnUpdate(ts.GetSecounds());
			});
		
#if defined(RY_PLATFORM_WINDOWS) && RY_PLATFORM_WINDOWS
		for (entt::entity e : scriptView)
		{
			Entity entity = { e, this };
			ScriptingEngine::OnUpdateEntity(entity, ts.GetSecounds());
		}
#endif


	}

	void Scene::OnRenderRuntime(const Ref<Framebuffer>& framebuffer, int camera)
	{
		RY_PROFILE_SCOPE("Scene-OnRenderRuntime");

		EnttView3D enttView3D = {
			m_Registery.view<ModelMatrixComponent, DynamicMeshComponent>(),
			m_Registery.view<ModelMatrixComponent, ModelMangerComponent>(),
			m_Registery.view<ModelMatrixComponent, StaticMeshComponent>()
		};
		EnttView2D enttView2D = {
			m_Registery.view<ModelMatrixComponent, SpriteRendererComponent>(),
			m_Registery.view<ModelMatrixComponent, TextComponent>()
		};
		EnttViewLigths enttViewLigths = {
			m_Registery.view<ModelMatrixComponent, DrirectionleLigthComponent>(),
			m_Registery.view<ModelMatrixComponent, PointLigthComponent>(),
			m_Registery.view<ModelMatrixComponent, SpotLigthComponent>()
		};

		EnttCameraView cameraView = m_Registery.view<ModelMatrixComponent, CameraComponent>();
		glm::mat4* modelMatrixPtr = nullptr;
		Camera* cameraPtr = nullptr;

		for (entt::entity camerE : cameraView)
		{
			auto& [modelC, camerC] = cameraView.get<ModelMatrixComponent, CameraComponent>(camerE);
			if (camerC.Primary)
			{
				cameraPtr = &camerC.Camera;
				modelMatrixPtr = &modelC.Globle;
				break;
			}
		}

		if (cameraPtr == nullptr)
			return;

		glm::mat4& modelMatrixRef = *modelMatrixPtr;
		glm::mat4 viewMatrix = glm::inverse(modelMatrixRef);
		glm::vec3 postion = modelMatrixRef[3];
		Utils::RenderDataMain(*cameraPtr, viewMatrix, modelMatrixRef, framebuffer, m_BackGround, m_MainTartget);
		

		EnttRenderTextView& enttRenderTextView = enttView2D.rendererTextCV;
		Submit2DTextEntitys(enttRenderTextView);
		

		EnttRender2DView& enttRender2DView = enttView2D.renderer2DCV;
		Submit2DEntitys(enttRender2DView);

		EnttRender3DStaticModelView& enttRender3DStaticModelView = enttView3D.staticCV;
		Submit3DStaticeEntitysMain(enttRender3DStaticModelView, &m_TimeElpassed3DSubmit);


		RenderNowMain();
		ResetRenderTaregtMain();
	}

#pragma endregion

#pragma region Editor

	void Scene::OnUpdateEditor(TimeStep ts)
	{
		RY_PROFILE_SCOPE("Scene-OnUpdateEditor");
	}

	void Scene::OnRenderEditor(const Ref<Framebuffer>& framebuffer, const Ref<EditorCamera>& editorCamera)
	{
		RY_PROFILE_SCOPE("Scene-OnRenderEditor");
		Renderer3D::UpdateEventProxys();

		EnttView3D enttView3D = {
			m_Registery.view<ModelMatrixComponent, DynamicMeshComponent>(),
			m_Registery.view<ModelMatrixComponent, ModelMangerComponent>(),
			m_Registery.view<ModelMatrixComponent, StaticMeshComponent>(),
		};
		EnttView2D enttView2D = {
			m_Registery.view<ModelMatrixComponent, SpriteRendererComponent>(),
			m_Registery.view<ModelMatrixComponent, TextComponent>()
		};
		EnttViewLigths enttViewLigths = {
			m_Registery.view<ModelMatrixComponent, DrirectionleLigthComponent>(),
			m_Registery.view<ModelMatrixComponent, PointLigthComponent>(),
			m_Registery.view<ModelMatrixComponent, SpotLigthComponent>()
		};
		
		Camera& mainCamera = static_cast<Camera>(editorCamera->GetProjektion());
		const glm::mat4& viewMatrix = editorCamera->GetViewMatrix();
		const glm::vec3& worldPostionCenterView = editorCamera->GetWorldPostionCenterView();

		const glm::uvec2& size = framebuffer->GetFrambufferSize();
		glm::mat4 modelCamera = glm::inverse(viewMatrix);
		Utils::RenderDataMain(mainCamera, viewMatrix, modelCamera, framebuffer, m_BackGround, m_MainTartget);
		EnttRender3DStaticModelView& enttRender3DStaticModelView = enttView3D.staticCV;
		EnttRenderTargetView& renderTargetView = m_Registery.view<RenderTargetComponent, CameraComponent, ModelMatrixComponent>();
		RenderRenderTaregtView(renderTargetView, enttRender3DStaticModelView);


		EnttCameraView cameraView = m_Registery.view<ModelMatrixComponent, CameraComponent>();
		Submit2DCamerIcons(cameraView);

		EnttDrirektionLigthView drirektionLigthView = m_Registery.view<ModelMatrixComponent, DrirectionleLigthComponent>();
		Submit2DDrirectionLigthIcons(drirektionLigthView);


		EnttRenderTextView& enttRenderTextView = enttView2D.rendererTextCV;
		Submit2DTextEntitys(enttRenderTextView);

		EnttRender2DView& enttRender2DView = enttView2D.renderer2DCV;		
		Submit2DEntitys(enttRender2DView);


		
		
		


		if(Renderer::IsSceneSubmite3DAktive())
		{
			Submit3DStaticeEntitysMain(enttRender3DStaticModelView, &m_TimeElpassed3DSubmit);
		}
		RenderNowMain();
		ResetRenderTaregtMain();
	}

	

#pragma endregion

#pragma region Simulation

	void Scene::OnUpdateSimulation(TimeStep ts)
	{
		OnUpdateRuntime(ts);
	}

	void Scene::OnRenderSimulation(const Ref<Framebuffer>& framebuffer, const Ref<EditorCamera>& camera)
	{
		OnRenderEditor(framebuffer, camera);
	}

#pragma endregion

	Ref<Scene> Scene::GetRefInPlace(Scene* scenePtr)
	{
		Ref<Scene> sceneRef = Asset::GetRefInPlaceType<Scene>(scenePtr);
		return sceneRef;
	}

#pragma region Renderer

#pragma region Render2D_SubmitFunc

	void Scene::Submit2DEntitys(EnttRender2DView& view2dQuads)
	{
		view2dQuads.each([](entt::entity e, ModelMatrixComponent& transformC, SpriteRendererComponent& spriteC)
		{
			uint32_t uEnitityID = entt::to_integral(e);
			int32_t enitityID = static_cast<int32_t>(uEnitityID);

			Renderer2D::SubmitSprite(transformC.Globle, spriteC, enitityID);
		});
	}

	void Scene::Submit2DTextEntitys(EnttRenderTextView& view2dText)
	{
		view2dText.each([](entt::entity e, ModelMatrixComponent& transformC, TextComponent& textC)
		{
			uint32_t uEnitityID = entt::to_integral(e);
			int32_t enitityID = static_cast<int32_t>(uEnitityID);

			Renderer2D::SubmitStringCom(transformC.Globle, textC, enitityID);
		});
	}

#pragma region IconSubmitionFunc

	void Scene::Submit2DCamerIcons(EnttCameraView& cameraView)
	{	

		cameraView.each([](entt::entity e, ModelMatrixComponent& modelC, CameraComponent& camerC)
			{
				uint32_t uEnitityID = entt::to_integral(e);
				int32_t enitityID = static_cast<int32_t>(uEnitityID);

				Renderer2D::SubmitCameraIcon(modelC.Globle, enitityID);
			});
	}

	void Scene::Submit2DDrirectionLigthIcons(EnttDrirektionLigthView& drirektionLigthView)
	{
		drirektionLigthView.each([](entt::entity e, ModelMatrixComponent& modelC, DrirectionleLigthComponent& dirctionLigthC)
			{
				uint32_t uEnitityID = entt::to_integral(e);
				int32_t enitityID = static_cast<int32_t>(uEnitityID);

				Renderer2D::SubmitLigthDirctionelIcon(modelC.Globle, enitityID);
			});
	}

	void Scene::Submit2DPointLigthIcons(EnttPointLigthView& pointLigthView)
	{
		pointLigthView.each([](entt::entity e, ModelMatrixComponent& modelC, PointLigthComponent& pointLigthC)
			{
				uint32_t uEnitityID = entt::to_integral(e);
				int32_t enitityID = static_cast<int32_t>(uEnitityID);

				Renderer2D::SubmitLigthPointIcon(modelC.Globle, enitityID);
			});

	}

	void Scene::Submit2DSpotLigthIcons(EnttSpotLigthView& spotLigthView)
	{
		spotLigthView.each([](entt::entity e, ModelMatrixComponent& modelC, SpotLigthComponent& spotLigthC)
		{
			uint32_t uEnitityID = entt::to_integral(e);
			int32_t enitityID = static_cast<int32_t>(uEnitityID);

			Renderer2D::SubmitLigthSpotIcon(modelC.Globle, enitityID);
		});
	}

#pragma endregion

#pragma endregion



#pragma region Render3D_SubmitFunc

#pragma region LigthSubmitionFunc

	void Scene::SubmitLigtheViews(EnttViewLigths& ligths)
	{
		RY_REMBER_FUNC_CHANGE("Implemten Function Sumbmit Call to Render3D");
		Submit3DDrirectionLigth(ligths.drirektionLCV);
		Submit3DPointLigth(ligths.pointLCV);
		Submit3DSpotLigth(ligths.spotLCV);
	}



	void Scene::Submit3DPointLigth(EnttPointLigthView& pointLigthView)
	{
		RY_REMBER_FUNC_CHANGE("Impation Finaly");
		pointLigthView.each([](entt::entity e, ModelMatrixComponent& transformC, PointLigthComponent& pointLigthC)
		{
		});

	}

	void Scene::Submit3DSpotLigth(EnttSpotLigthView& spotLigthView)
	{
		RY_REMBER_FUNC_CHANGE("Impation Finaly");
		spotLigthView.each([](entt::entity e, ModelMatrixComponent& renderTargetC, SpotLigthComponent& spotLigthC)
		{
		});
	}

	void Scene::Submit3DDrirectionLigth(EnttDrirektionLigthView& drirektionLigthView)
	{
		RY_REMBER_FUNC_CHANGE("Impation Finaly");
		drirektionLigthView.each([](entt::entity e, ModelMatrixComponent& transformC, DrirectionleLigthComponent& drirektionLigthC)
		{
		});
	}

#pragma endregion

#pragma region RenderTarget
	void Scene::RenderRenderTaregtView(EnttRenderTargetView& renderTargetView, EnttRender3DStaticModelView& view3dStaticMesh)
	{
		Renderer::ResetCurentRenderPassFrame();
		renderTargetView.each([&view3dStaticMesh](entt::entity e, RenderTargetComponent& renderTargetC, CameraComponent& cameraC, ModelMatrixComponent& modelC)
			{
				if (cameraC.Primary || nullptr == renderTargetC.Target || nullptr == renderTargetC.Target->GetFramebuffer())
				{
					return;
				}
				SubmitRenderTaregtCurent(cameraC.Camera, modelC.Globle, renderTargetC);
				if (Renderer::IsSceneSubmite3DAktive())
				{
					Submit3DStaticeEntitysCurent(view3dStaticMesh);
				}
				RenderNowCurent();
				ResetRenderTaregtCurent();

				Renderer::IncromentCurentRenderPass();
			});
	}

	void Scene::SubmitRenderTaregtCurent(Camera& camera, const glm::mat4& model, RenderTargetComponent& targetC)
	{
		Renderer::SetNextCurentRenderPass(targetC.StroeIndex);
		Renderer::SetRenderPassNameCurent(targetC.RenderPassName);

		Renderer::SetRenderCameraCurent(model, camera);
		
		Renderer::SetRenderCameraCurentUB();
		Renderer::SetRenderTargetCurent(targetC.Target);
		const glm::ivec4& viewSize = targetC.Target->GetRenderViewSize();


		Renderer::SetViewSizeCurent(viewSize);
	}

	void Scene::SubmitRenderTaregtMain(Camera& camera, const glm::mat4& model, RenderTargetComponent& targetC)
	{
		Renderer::SetRenderPassNameMain(targetC.RenderPassName);
		Renderer::SetRenderCameraMain(model, camera);
	}

	void Scene::RenderNowMain()
	{
		Renderer2D::SubmitRenderDrawList();
		Renderer::RenderSubmitSceneMain();
	}

	void Scene::RenderNowCurent()
	{
		Renderer::RenderingPassCurent();
	}

	void Scene::ResetRenderTaregtMain()
	{
		Renderer::ClearMainPiplines();
		Renderer3D::ResetMeshObject();
	}

	void Scene::ResetRenderTaregtCurent()
	{
		Renderer::ClearCurentPiplines();

		Renderer3D::ResetMeshObject();
	}

	

#pragma endregion

#pragma region SubmitTo3DRenderPiplinesFuncs

	void Scene::Submit3DStaticeEntitysRenderProxy(EnttRender3DStaticModelView& view3dStaticMesh)
	{
		Renderer3D::ClearRenderProxy();

		view3dStaticMesh.each([](entt::entity e, ModelMatrixComponent& transformC, ModelMangerComponent& meshCompC)
		{
			uint32_t uEnitityID = entt::to_integral(e);
			int32_t enitityID = static_cast<int32_t>(uEnitityID);

			Renderer3D::AddMeshComponentRenderProxy(enitityID, meshCompC, transformC.Globle);
		});
	}

	void Scene::Submit3DStaticeEntitys(EnttRender3DStaticModelView& view3dStaticMesh, int64_t* timerPtr)
	{
		Ref<PlatformTimer> timer = PlatformTimer::Create(timerPtr);
		view3dStaticMesh.each([](entt::entity e, ModelMatrixComponent& transformC, ModelMangerComponent& meshCompC)
			{
				uint32_t uEnitityID = entt::to_integral(e);
				int32_t enitityID = static_cast<int32_t>(uEnitityID);

				Renderer3D::MeshCompont(transformC.Globle, meshCompC, enitityID);
			});
	}

	void Scene::Submit3DStaticeEntitysMain(EnttRender3DStaticModelView& view3dStaticMesh, int64_t* timerPtr)
	{
		Ref<PlatformTimer> timer = PlatformTimer::Create(timerPtr);
		view3dStaticMesh.each([](entt::entity e, ModelMatrixComponent& transformC, ModelMangerComponent& meshCompC)
			{
				uint32_t uEnitityID = entt::to_integral(e);
				int32_t enitityID = static_cast<int32_t>(uEnitityID);

				Renderer3D::MeshCompontMain(transformC.Globle, meshCompC, enitityID);
			});
	}


	void Scene::Submit3DStaticeEntitysCurent(EnttRender3DStaticModelView& view3dStaticMesh)
	{
		view3dStaticMesh.each([](entt::entity e, ModelMatrixComponent& transformC, ModelMangerComponent& meshCompC)
			{
				uint32_t uEnitityID = entt::to_integral(e);
				int32_t enitityID = static_cast<int32_t>(uEnitityID);

				Renderer3D::MeshCompontCurent(transformC.Globle, meshCompC, enitityID);
			});
	}

	void Scene::Submit3DSingleStaticeEntitys(EnttRender3DSingleStaticModelView& view3dSingleStaticMesh, int64_t* timerPtr)
	{
		Ref<PlatformTimer> timer = PlatformTimer::Create(timerPtr);
		view3dSingleStaticMesh.each([](entt::entity e, ModelMatrixComponent& transformC, StaticMeshComponent& meshCompC)
			{
				uint32_t uEnitityID = entt::to_integral(e);
				int32_t enitityID = static_cast<int32_t>(uEnitityID);


				Renderer3D::MeshCompont(transformC.Globle, meshCompC, enitityID);
			});
	}


#pragma region OnEventFunc

#pragma region ModelMatrix

	void Scene::OnEntityModelMatrixCreate(entt::registry& registry, entt::entity entity)
	{
		const ModelMatrixComponent& modelMatrixC = Utils::OnEventFunc<ModelMatrixComponent>("create", registry, entity);
		if (registry.has<ModelMangerComponent>(entity))
		{
			const ModelMangerComponent& staticMeshC = registry.get<ModelMangerComponent>(entity);

			uint32_t uEnitityID = entt::to_integral(entity);
			int32_t enitityID = static_cast<int32_t>(uEnitityID);

			Renderer3D::AddMeshComponentRenderProxy(enitityID, staticMeshC, modelMatrixC.Globle);
		}
	}
	
	
	void Scene::OnEntityModelMatrixChanged(entt::registry& registry, entt::entity entity)
	{
		const ModelMatrixComponent& modelMatrixC = Utils::OnEventFunc<ModelMatrixComponent>("changed", registry, entity);
		if (registry.has<ModelMangerComponent>(entity))
		{
			const ModelMangerComponent& staticMeshC = registry.get<ModelMangerComponent>(entity);

			uint32_t uEnitityID = entt::to_integral(entity);
			int32_t enitityID = static_cast<int32_t>(uEnitityID);

			Renderer3D::UpdateTransformMeshComponentRenderProxy(enitityID, staticMeshC, modelMatrixC.Globle);
		}

	}

	void Scene::OnEntityModelMatrixDestroy(entt::registry& registry, entt::entity entity)
	{
		const ModelMatrixComponent& modelMatrixC = Utils::OnEventFunc<ModelMatrixComponent>("destroy", registry, entity);
		if (registry.has<ModelMangerComponent>(entity))
		{
			const ModelMangerComponent& staticMeshC = registry.get<ModelMangerComponent>(entity);

			uint32_t uEnitityID = entt::to_integral(entity);
			int32_t enitityID = static_cast<int32_t>(uEnitityID);

			Renderer3D::RemoveMeshComponentRenderProxy(enitityID);
		}

	}

#pragma endregion

	
#pragma region StaticMesh

	void Scene::OnEntityStaticMeshCreate(entt::registry& registry, entt::entity entity)
	{
		const ModelMangerComponent& staticMeshC = Utils::OnEventFunc<ModelMangerComponent>("create", registry, entity);
		if (registry.has<ModelMatrixComponent>(entity))
		{
			const ModelMatrixComponent& modelMatrixC = registry.get<ModelMatrixComponent>(entity);

			uint32_t uEnitityID = entt::to_integral(entity);
			int32_t enitityID = static_cast<int32_t>(uEnitityID);

			Renderer3D::AddMeshComponentRenderProxy(enitityID, staticMeshC, modelMatrixC.Globle);
		}

	}

	void Scene::OnEntityStaticMeshChanged(entt::registry& registry, entt::entity entity)
	{
		if (!Asset::CurrentOnMainThread())
		{
			
			Application::Get().SubmiteToMainThreedQueueWait(
				[&registry, entity]() 
				{
					Scene::OnEntityStaticMeshChanged(registry, entity); 
				}
			);
			return;
		}

		const ModelMangerComponent& staticMeshC = Utils::OnEventFunc<ModelMangerComponent>("changed", registry, entity);
		if (registry.has<ModelMatrixComponent>(entity))
		{
			const ModelMatrixComponent& modelMatrixC = registry.get<ModelMatrixComponent>(entity);

			uint32_t uEnitityID = entt::to_integral(entity);
			int32_t enitityID = static_cast<int32_t>(uEnitityID);

			Renderer3D::RemoveMeshComponentRenderProxy(enitityID);
			Renderer3D::AddMeshComponentRenderProxy(enitityID, staticMeshC, modelMatrixC.Globle);

		}
	}

	void Scene::OnEntityStaticMeshDestroy(entt::registry& registry, entt::entity entity)
	{
		const ModelMangerComponent& staticMeshC = Utils::OnEventFunc<ModelMangerComponent>("destroy", registry, entity);
		if (registry.has<ModelMatrixComponent>(entity))
		{
			const ModelMatrixComponent& modelMatrixC = registry.get<ModelMatrixComponent>(entity);

			uint32_t uEnitityID = entt::to_integral(entity);
			int32_t enitityID = static_cast<int32_t>(uEnitityID);

			Renderer3D::RemoveMeshComponentRenderProxy(enitityID);
		}
	}

#pragma endregion

#pragma region StaticSingleMesh

	void Scene::OnEntityStaticSingleMeshCreate(entt::registry& registry, entt::entity entity)
	{
		const StaticMeshComponent& staticSingleMeshC = Utils::OnEventFunc<StaticMeshComponent>("create", registry, entity);

	}

	void Scene::OnEntityStaticSingleMeshChanged(entt::registry& registry, entt::entity entity)
	{
		const StaticMeshComponent& staticSingleMeshC = Utils::OnEventFunc<StaticMeshComponent>("changed", registry, entity);

	}

	void Scene::OnEntityStaticSingleMeshDestroy(entt::registry& registry, entt::entity entity)
	{
		const StaticMeshComponent& staticSingleMeshC = Utils::OnEventFunc<StaticMeshComponent>("destroy", registry, entity);

	}

#pragma endregion


#pragma endregion

#pragma endregion



#pragma region Submit3DObjectsDataFuncs

	void Scene::Submit3DDataStaticeEntitys(EnttRender3DStaticModelView& view3dStaticMesh, int64_t* timerPtr)
	{
		Ref<PlatformTimer> timer = PlatformTimer::Create(timerPtr);
		for (entt::entity render3dE : view3dStaticMesh)
		{
			auto& [transformC, meshC] = view3dStaticMesh.get<ModelMatrixComponent, ModelMangerComponent>(render3dE);
			Renderer3D::MeshCompontSetData(transformC.Globle, meshC, static_cast<int>(render3dE));

		}
		Renderer3D::SubmitShadeDataMeshObjectToPipline();
		Renderer3D::SubmitDepthDataMeshObjectToPipline();
	}

	void Scene::Submit3DDataSingleStaticeEntitys(EnttRender3DSingleStaticModelView& view3dSingleStaticMesh, int64_t* timerPtr)
	{
		Ref<PlatformTimer> timer = PlatformTimer::Create(timerPtr);
		for (entt::entity render3dE : view3dSingleStaticMesh)
		{
			auto& [transformC, meshC] = view3dSingleStaticMesh.get<ModelMatrixComponent, StaticMeshComponent>(render3dE);
			Renderer3D::MeshCompontSetData(transformC.Globle, meshC, static_cast<int>(render3dE));
		}
		Renderer3D::SubmitShadeDataMeshObjectToPipline();
		Renderer3D::SubmitDepthDataMeshObjectToPipline();
	}

#pragma endregion

#pragma endregion


	void Scene::OnViewportResize(uint32_t withe, uint32_t heigth)
	{
		m_ViewPortWithe = withe;
		m_ViewPortHeigth = heigth;

		auto view = m_Registery.view<CameraComponent>();
		for (entt::entity entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);

			if (cameraComponent.Primary && !cameraComponent.FixedAspectRotaion)
				cameraComponent.Camera.SetViewPortSize(withe, heigth);

		}
	}

#pragma endregion

	
#pragma region ComponentAddFunktions


	// Default
	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component)
	{
		static_assert(false);
	}

	

	// Template Component
	template<>
	void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
	{
	}


	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		component.Camera.SetViewPortSize(m_ViewPortWithe, m_ViewPortHeigth);
		if (!entity.HasComponent<ViewMatrixComponent>())
			entity.AddComponent<ViewMatrixComponent>();
		if (!entity.HasComponent<WorldViewFustrumComponent>())
			entity.AddComponent<WorldViewFustrumComponent>();
	}

	template<>
	void Scene::OnComponentAdded<RenderTargetComponent>(Entity entity, RenderTargetComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<MaterialComponent>(Entity entity, MaterialComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<GeomtryComponent>(Entity entity, GeomtryComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<NativeSripteComponent>(Entity entity, NativeSripteComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<ScriptComponent>(Entity entity, ScriptComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<FrameBufferComponent>(Entity entity, FrameBufferComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<Matrix3x3Component>(Entity entity, Matrix3x3Component& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<ModelMatrixComponent>(Entity entity, ModelMatrixComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<ViewMatrixComponent>(Entity entity, ViewMatrixComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<VisibleComponent>(Entity entity, VisibleComponent& component)
	{
		component.isVisable = true;
	}

	template<>
	void Scene::OnComponentAdded<StaticMeshComponent>(Entity entity, StaticMeshComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<RealtionShipUUIDComponent>(Entity entity, RealtionShipUUIDComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<ModelMangerComponent>(Entity entity, ModelMangerComponent& component)
	{
	}


	template<>
	void Scene::OnComponentAdded<DynamicMeshComponent>(Entity entity, DynamicMeshComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<MeshComponent>(Entity entity, MeshComponent& component)
	{
	}


	template<>
	void Scene::OnComponentAdded<DrirectionleLigthComponent>(Entity entity, DrirectionleLigthComponent& component)
	{
		if (!entity.HasComponent<ViewMatrixComponent>())
			entity.AddComponent<ViewMatrixComponent>();
	}

	template<>
	void Scene::OnComponentAdded<PointLigthComponent>(Entity entity, PointLigthComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<SpotLigthComponent>(Entity entity, SpotLigthComponent& component)
	{
		if (!entity.HasComponent<ViewMatrixComponent>())
			entity.AddComponent<ViewMatrixComponent>();
		
	}

	template<>
	void Scene::OnComponentAdded<ParticelComponente>(Entity entity, ParticelComponente& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<TextComponent>(Entity enitity, TextComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<WorldViewFustrumComponent>(Entity enitity, WorldViewFustrumComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<InverseProjtionViewMatrixComponent>(Entity enitity, InverseProjtionViewMatrixComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<ProjtionViewMatrixComponent>(Entity enitity, ProjtionViewMatrixComponent& component)
	{
	}

#pragma endregion

}