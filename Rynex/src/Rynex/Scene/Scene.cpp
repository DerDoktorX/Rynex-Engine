#include "rypch.h"
#include "Scene.h"

#include "Entity.h"
#include "ScriptableEntity.h"
#include "Rynex/Renderer/Rendering/Renderer2D.h"
#include "Rynex/Renderer/Rendering/Renderer3D.h"
#if RY_SCRIPTING_HAZEL
	#include "Rynex/Scripting/HazelScripting/ScriptEngine.h"
#else
	#include "Rynex/Scripting/ScriptingEngine.h"
#endif
#include "Rynex/Serializers/VertexArraySerialzer.h"
#include "Rynex/Asset/Base/AssetMetadata.h"
#include "Rynex/Asset/EditorAssetManager.h"
#include "Rynex/Project/Project.h"
#include "Rynex/Renderer/Rendering/RendererIcons.h"

// TODO: Move toCoreConfig.h
#define RENDERER_2D		1
#define RENDERER_3D		1
#define QUADS_DRAW		1
#define CIRCLE_DRAW		1
#define CIRCLE_DRAW		0
#define TEXT_DRAW		0
#define ENTITY_SCRIPT	1
#define NATIVE_SCRIPT	1
#define SCRIPT_CS		1


namespace Rynex {

	using ScenePrimitvRenderFunctions = std::function<void(const Ref<VertexArray>&)>;

	static std::map<VertexArray::Primitv, ScenePrimitvRenderFunctions> s_SceneRenderFuncs = {
		{ VertexArray::Primitv::Traingle,		Renderer3D::DrawMesh		},
		{ VertexArray::Primitv::TraingleStrips, Renderer3D::DrawMeshStrips	},
		{ VertexArray::Primitv::Line,			Renderer3D::DrawLine		},
		{ VertexArray::Primitv::LineLoop,		Renderer3D::DrawLineLoop	},
		{ VertexArray::Primitv::Points,			Renderer3D::DrawPoints		},
		{ VertexArray::Primitv::Patches,		Renderer3D::DrawPatches		},
	};



	Scene::Scene()
	{

	}

	Scene::~Scene()
	{

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
	static void CopyComponentIfExists(ComponentGroup<Component...>, Entity dst, Entity src)
	{
		CopyComponentIfExists<Component...>(dst, src);
	}

	Ref<Scene> Scene::Copy(Ref<Scene> other)
	{
		Ref<Scene> newScene = CreateRef<Scene>();

		newScene->m_ViewPortWithe = other->m_ViewPortWithe;
		newScene->m_ViewPortHeigth = other->m_ViewPortHeigth;

		auto& srcSceneRegistry = other->m_Registery;
		auto& dstSceneRegistry = newScene->m_Registery;
		std::unordered_map<UUID, entt::entity> enttMap;

		// Create entities in new scene
		auto idView = srcSceneRegistry.view<IDComponent>();
		for (auto e : idView)
		{
			UUID uuid = srcSceneRegistry.get<IDComponent>(e).ID;
			const auto& name = srcSceneRegistry.get<TagComponent>(e).Tag;
			Entity newEntity = newScene->CreateEntityWitheUUID(uuid, name);
			enttMap[uuid] = (entt::entity)newEntity;
		}

		// Copy components (except IDComponent and TagComponent)
		CopyComponent(AllComponents{}, dstSceneRegistry, srcSceneRegistry, enttMap);

		return newScene;
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		return CreateEntityWitheUUID(UUID(), name);
	}



	Entity Scene::CreateEntityWitheUUID(UUID uuid, const std::string& name, int index)
	{
		Entity entity = index == -1 ?
			Entity(m_Registery.create(), this) :
			Entity(m_Registery.create((entt::entity)index), this);

		entity.AddComponent<IDComponent>(uuid);
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<RealtionShipComponent>();
		entity.AddComponent<Matrix4x4Component>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;

		m_EntityMapID[uuid] = entity;
		m_EntityMapTag[name] = entity;
		m_EntityLeangth++;
		return entity;
	}

	void Scene::OnRuntimStart()
	{
#if RY_SCRIPTING_HAZEL
		ScriptEngine::OnRuntimeStart(this);
#else
		ScriptingEngine::OnRuntimeStart(this);
#endif
		// Instandiat

		auto view = m_Registery.view<ScriptComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
#if RY_SCRIPTING_HAZEL
			ScriptEngine::OnCreateEntity(entity);
#else
			ScriptingEngine::OnCreatEntity(entity);
#endif
		}
	}

	void Scene::OnRuntimStop()
	{
#if RY_SCRIPTING_HAZEL
		ScriptEngine::OnRuntimeStop();
#else
		
#endif
		auto view = m_Registery.view<ScriptComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			RY_CORE_ASSERT(entity);
			ScriptingEngine::OnDestroyEntity(entity);
		}
		ScriptingEngine::OnRuntimeStop();
	}

	void Scene::DestroyEntity(Entity entity)
	{
		entity.DestroyEntity();
		m_EntityMapID.erase(entity.GetUUID());
		m_EntityMapTag.erase(entity.GetComponent<TagComponent>().Tag);
		m_Registery.destroy(entity);
		m_EntityLeangth--;
	}

	void Scene::OnUpdateEditor(TimeStep ts)
	{
#if RY_SCRIPTING_HAZEL
		// ScriptEngine::ReloadAssembly();
		// ScriptEngine::OnRuntimeStart(this);
		auto scriptView = m_Registery.view<ScriptComponent>();
		for (EnttEntity scriptViewE : scriptView)
		{
			Entity entity = { scriptViewE, this };
		}
		// ScriptEngine::OnRuntimeStop();
#else

		// if (!ScriptingEngine::ReloadeScriptAvaible()) return;

		// ScriptingEngine::OnRuntimeStart(this);
		auto scriptView = m_Registery.view<ScriptComponent>();
		for (EnttEntity scriptViewE : scriptView)
		{
			Entity entity = { scriptViewE, this };
			ScriptingEngine::OnDrawEntity(entity);
		}
		// ScriptingEngine::OnRuntimeStop();
#endif

	}

	void Scene::OnRenderEditor(const Ref<Framebuffer>& framebuffer, const Ref<EditorCamera>& editorCamera)
	{
		EnttCameraView cameraView = m_Registery.view<Matrix4x4Component, CameraComponent>();

		EnttView3D enttView3D = EnttView3D(
			m_Registery.view<Matrix4x4Component, MaterialComponent, DynamicMeshComponent>(),
			m_Registery.view<Matrix4x4Component, MaterialComponent, StaticMeshComponent>(),
			m_Registery.view<Matrix4x4Component, MaterialComponent, GeomtryComponent>()
		);
		EnttViewLigths enttViewLigths(
			m_Registery.view<AmbientLigthComponent>(),
			m_Registery.view<Matrix4x4Component, DrirektionleLigthComponent>(),
			m_Registery.view<Matrix4x4Component, PointLigthComponent>(),
			m_Registery.view<Matrix4x4Component, SpotLigthComponent>()
		);
		Camera& mainCamera = (Camera)editorCamera->GetProjektion();
		glm::mat4 viewMatrix = editorCamera->GetViewMatrix();
		EnttView2D enttView2D = EnttView2D(m_Registery.view<Matrix4x4Component, SpriteRendererComponent>());

		RenderFrambuffers(enttView3D, enttView2D, cameraView);	

		framebuffer->Bind();
		RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		RenderCommand::Clear();
		framebuffer->ClearAttachment(1, -1);
		m_RedererDefaultModeFlags = Renderer::GetMode(); 
		const glm::uvec2& size = framebuffer->GetFrambufferSize();
		SetLigthsEditor(enttViewLigths, mainCamera, viewMatrix, size);
		RenderScene3D(mainCamera, viewMatrix, enttView3D);
		RenderScene2D(mainCamera, viewMatrix, enttView2D);
		Renderer2D::EndSceneIcon();
		framebuffer->Unbind();
	}

	void Scene::EditorFilterSreene()
	{

	}

	void Scene::ClearAll()
	{
		m_Registery.clear();
		m_ViewPortWithe = 1;
		m_ViewPortHeigth = 1;
		m_IsRunning = false;
		m_IsPaused = false;
		m_StepFrames = 0;
		m_EntityLeangth = 0;
		m_EntityMapID.clear();
		m_EntityMapTag.clear();
	}

	void Scene::OnUpdateRuntime(TimeStep ts)
	{
		auto scriptView = m_Registery.view<ScriptComponent>();

		m_Registery.view<NativeSripteComponent>().each([=](auto entity, auto& nsc)
			{
				if (!nsc.Instance)
				{
					nsc.Instance = nsc.InstantiateScript();
					nsc.Instance->m_Entity = Entity{ entity , this };
					nsc.Instance->OnCreate();
				}
				nsc.Instance->OnUpdate(ts);
			});

		for (EnttEntity e : scriptView)
		{
			Entity entity = { e, this };
#if RY_SCRIPTING_HAZEL
			ScriptEngine::OnUpdateEntity(entity, ts);
#else
			ScriptingEngine::OnUpdateEntity(entity, ts);
#endif
		}


	}

	void Scene::OnRenderRuntime(const Ref<Framebuffer>& framebuffer, int camera)
	{
		EnttCameraView cameraView = m_Registery.view<Matrix4x4Component, CameraComponent>();
		EnttView3D enttView3D = EnttView3D(
			m_Registery.view<Matrix4x4Component, MaterialComponent, DynamicMeshComponent>(),
			m_Registery.view<Matrix4x4Component, MaterialComponent, StaticMeshComponent>(),
			m_Registery.view<Matrix4x4Component, MaterialComponent, GeomtryComponent>());
		EnttViewLigths enttViewLigths(
			m_Registery.view<AmbientLigthComponent>(),
			m_Registery.view<Matrix4x4Component, DrirektionleLigthComponent>(),
			m_Registery.view<Matrix4x4Component, PointLigthComponent>(),
			m_Registery.view<Matrix4x4Component, SpotLigthComponent>());
		SetLigthsRuntime(enttViewLigths);
		EnttView2D enttView2D = EnttView2D(m_Registery.view<Matrix4x4Component, SpriteRendererComponent>());
		m_RedererDefaultModeFlags = Renderer::GetMode();
		RenderFrambuffers(enttView3D, enttView2D, cameraView);

		Camera* mainCamera = nullptr;
		const glm::mat4* mainTransform;
		int count = 0;
		for (EnttEntity camerE : cameraView)
		{
			auto& [tranformC, cameraC] = cameraView.get<Matrix4x4Component, CameraComponent>(camerE);
			if (cameraC.Primary)
			{
				if (count == camera)
				{

					mainCamera = &cameraC.Camera;
					mainTransform = &tranformC.GlobleMatrix4x4;
					break;
				}
				count++;
			}
		}

		if (mainCamera)
		{
			
			framebuffer->Bind();
			RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
			RenderCommand::Clear();
			framebuffer->ClearAttachment(1, -1);
			RenderScene3D(*mainCamera, glm::inverse(*mainTransform), enttView3D);
			RenderScene2D(*mainCamera, glm::inverse(*mainTransform), enttView2D);
			framebuffer->Unbind();
		}
		else
		{
			framebuffer->Bind();
			RenderCommand::SetClearColor({ 1.0f, 0.0f, 0.0f, 1.0f });
			RenderCommand::Clear();
			framebuffer->ClearAttachment(1, -1);
			framebuffer->Unbind();
		}

	}

	void Scene::OnUpdateSimulation(TimeStep ts)
	{
		OnUpdateEditor(ts);
	}

	void Scene::OnRenderSimulation(const Ref<Framebuffer>& framebuffer, const Ref<EditorCamera>& camera)
	{
		OnRenderEditor(framebuffer, camera);
	}


	void Scene::RenderScene2D(Camera& camera, glm::mat<4, 4, float>& viewMatrix, EnttView2D& enttView2D)
	{
		EnttRender2DView& enttRender2DView = enttView2D.RendererCV;
		RY_PROFILE_SCOPE("Scene Draw 2D");
		Renderer2D::BeginSceneQuade(camera, viewMatrix);
		for (EnttEntity render2dE : enttRender2DView)
		{
			auto& [transformC, spriteC] = enttRender2DView.get< Matrix4x4Component, SpriteRendererComponent>(render2dE);
			Renderer2D::DrawSprite(transformC.GlobleMatrix4x4, spriteC, (int)render2dE);
		}
		Renderer2D::EndSceneQuade();
	}

	void Scene::RenderScene3D(Camera& camera, glm::mat<4, 4, float>& viewMatrix, EnttView3D& enttView3D)
	{
		RY_PROFILE_SCOPE("Scene Draw 3D");
		EnttRender3DEditeView& enttRender3DEditeView = enttView3D.EditeCV;
		Renderer3D::BeginScene(camera, viewMatrix);
		for (EnttEntity render3dE : enttRender3DEditeView)
		{
			auto& [tranformC, materialC, geomtryC] = enttRender3DEditeView.get< Matrix4x4Component, MaterialComponent, GeomtryComponent>(render3dE);
			Ref<VertexArray> vertexArray = geomtryC.Geometry;
			if (vertexArray == nullptr)
				continue;
			Renderer3D::BeforDrawEntity(materialC, tranformC.GlobleMatrix4x4, (int)render3dE);
			Renderer3D::DrawObjectRender3D(vertexArray);
			Renderer3D::AfterDrawEntity(materialC);
		}
		EnttRender3DStaticModelView& enttRender3DStaticModelView = enttView3D.StaticCV;
		for (EnttEntity render3dE : enttRender3DStaticModelView)
		{
			auto& [tranformC, materialC, geomtryC] = enttRender3DStaticModelView.get<Matrix4x4Component, MaterialComponent, StaticMeshComponent>(render3dE);

			if (geomtryC.ModelR == nullptr)
				continue;
			Renderer3D::DrawModdel(materialC, tranformC.GlobleMatrix4x4, geomtryC, (int)render3dE);

		}
		EnttRender3DDynamicModelView& enttRender3DDynamicModelView = enttView3D.DynamicModelCV;
		for (EnttEntity render3dE : enttRender3DDynamicModelView)
		{
			auto& [tranformC, materialC, geomtryC] = enttRender3DDynamicModelView.get<Matrix4x4Component, MaterialComponent, DynamicMeshComponent>(render3dE);
			if (geomtryC.MeshR == nullptr)
				continue;
			Renderer3D::DrawLineBoxAABB(geomtryC.MeshR->GetVertexArray()->GetBoxAABB(), tranformC.GlobleMatrix4x4, (int)render3dE);
			Renderer3D::DrawModdel(materialC, tranformC.GlobleMatrix4x4, geomtryC, (int)render3dE);

		}
		Renderer3D::EndScene();

	}

	void Scene::RenderFrambuffers( EnttView3D& enttView3D, EnttView2D& enttView2D, EnttCameraView& enttCameraView)
	{
		EnttFrameBufferView frambufferView = m_Registery.view<Matrix4x4Component, FrameBufferComponent, CameraComponent>();
		for (EnttEntity frameE : frambufferView)
		{
			auto& [transformC, frambufferC, camerC] = frambufferView.get<Matrix4x4Component, FrameBufferComponent, CameraComponent>(frameE);
			if (frambufferC.FrameBuffer)
			{
				frambufferC.FrameBuffer->Bind();
				RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
				RenderCommand::Clear();
				RenderScene3D(camerC.Camera, glm::inverse(transformC.GlobleMatrix4x4), enttView3D);
				RenderScene2D(camerC.Camera, glm::inverse(transformC.GlobleMatrix4x4), enttView2D);
				frambufferC.FrameBuffer->Unbind();
			}
		}
	}


	// 
	void Scene::SetLigthsRuntime(EnttViewLigths& enttViewLigths)
	{
		EnttAmbientLView& ambientView = enttViewLigths.AmbientLCV;
		EnttDrirektionLeLView& directionelView = enttViewLigths.DrirektionLCV;
		EnttPointLView& pointView = enttViewLigths.PointLCV;
		EnttSpotLView& spotView = enttViewLigths.SpotLCV;

		uint32_t shaderCount = 1;
		for (EnttEntity ambientE : ambientView)
		{
			auto& ambientC = ambientView.get<AmbientLigthComponent>(ambientE);
			Renderer3D::SetLigthUniform(ambientC, shaderCount);
			shaderCount++;
		}
		shaderCount = 1;
		for (EnttEntity directionelE : directionelView)
		{
			auto& [matrixC, directionelC] = directionelView.get<Matrix4x4Component, DrirektionleLigthComponent>(directionelE);
			Renderer3D::SetLigthUniform(directionelC, matrixC.GlobleMatrix4x4, shaderCount);

			shaderCount++;
		}

		shaderCount = 1;
		for (EnttEntity pointE : pointView)
		{
			auto& [matrixC, pointC] = pointView.get<Matrix4x4Component, PointLigthComponent>(pointE);
			Renderer3D::SetLigthUniform(pointC, matrixC.GlobleMatrix4x4, shaderCount);
			shaderCount++;
		}

		shaderCount = 1;
		for (EnttEntity spotE : spotView)
		{
			auto& [matrixC, spotC] = spotView.get<Matrix4x4Component, SpotLigthComponent>(spotE);
			Renderer3D::SetLigthUniform(spotC, matrixC.GlobleMatrix4x4, shaderCount);
			shaderCount++;
		}
		
	}

	void Scene::SetLigthsEditor(EnttViewLigths& enttViewLigths, Camera& camera, const glm::mat<4, 4, float>& viewMatrix, const glm::uvec2& viewPortSize)
	{
		EnttAmbientLView& ambientView = enttViewLigths.AmbientLCV;
		EnttDrirektionLeLView& directionelView = enttViewLigths.DrirektionLCV;
		EnttPointLView& pointView = enttViewLigths.PointLCV;
		EnttSpotLView& spotView = enttViewLigths.SpotLCV;

		uint32_t shaderCount = 1;
		
		for (EnttEntity ambientE : ambientView)
		{
			auto& ambientC = ambientView.get<AmbientLigthComponent>(ambientE);
			Renderer3D::SetLigthUniform(ambientC, shaderCount);
			shaderCount++;
		}
#if RY_ANABLE_RENDERER_ICONE
		RendererIcons::BeginScene(camera, viewMatrix, viewPortSize);
#else
		Renderer2D::BeginSceneIcon(camera, viewMatrix, viewPortSize);
#endif
		for (EnttEntity directionelE : directionelView)
		{
			auto& [matrixC, directionelC] = directionelView.get<Matrix4x4Component, DrirektionleLigthComponent>(directionelE);
			Renderer3D::SetLigthUniform(directionelC, matrixC.GlobleMatrix4x4, shaderCount);
#if RY_ANABLE_RENDERER_ICONE
			RendererIcons::DrawLigthDirctionelIcon(matrixC.GlobleMatrix4x4, (int)directionelE);
#else
			Renderer2D::DrawLigthDirctionelIcon(matrixC.GlobleMatrix4x4, (int)directionelE);
#endif

		}

		for (EnttEntity pointE : pointView)
		{
			auto& [matrixC, pointC] = pointView.get<Matrix4x4Component, PointLigthComponent>(pointE);
			Renderer3D::SetLigthUniform(pointC, matrixC.GlobleMatrix4x4, shaderCount);
#if RY_ANABLE_RENDERER_ICONE
			RendererIcons::DrawLigthPointIcon(matrixC.GlobleMatrix4x4, (int)pointE);
#else
			Renderer2D::DrawLigthPointIcon(matrixC.GlobleMatrix4x4, (int)pointE);
#endif
		}

		for (EnttEntity spotE : spotView)
		{
			auto& [matrixC, spotC] = spotView.get<Matrix4x4Component, SpotLigthComponent>(spotE);
			Renderer3D::SetLigthUniform(spotC, matrixC.GlobleMatrix4x4, shaderCount);
#if RY_ANABLE_RENDERER_ICONE
			RendererIcons::DrawLigthSpotIcon(matrixC.GlobleMatrix4x4, (int)spotE);
#else
			Renderer2D::DrawLigthSpotIcon(matrixC.GlobleMatrix4x4, (int)spotE);
#endif
		}
		
	}




	void Scene::OnViewportResize(uint32_t withe, uint32_t heigth)
	{
		m_ViewPortWithe = withe;
		m_ViewPortHeigth = heigth;

		//resize
		auto view = m_Registery.view<CameraComponent>();
		for (EnttEntity entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);

			if (!cameraComponent.FixedAspectRotaion)
				cameraComponent.Camera.SetViewPortSize(withe, heigth);

		}
	}

	Entity Scene::GetEntitiyByUUID(UUID uuid)
	{
		if (m_EntityMapID.find(uuid) != m_EntityMapID.end())
			return { m_EntityMapID.at(uuid), this };

		return {};
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
		return (m_EntityMapTag.find(tag) != m_EntityMapTag.end());
	}

	

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
	void Scene::OnComponentAdded<Matrix4x4Component>(Entity entity, Matrix4x4Component& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<MainViewPortComponent>(Entity entity, MainViewPortComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<RealtionShipComponent>(Entity entity, RealtionShipComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<StaticMeshComponent>(Entity entity, StaticMeshComponent& component)
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
	void Scene::OnComponentAdded<MaterialTransperent>(Entity entity, MaterialTransperent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<AmbientLigthComponent>(Entity entity, AmbientLigthComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<DrirektionleLigthComponent>(Entity entity, DrirektionleLigthComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<PointLigthComponent>(Entity entity, PointLigthComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<SpotLigthComponent>(Entity entity, SpotLigthComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<ParticelComponente>(Entity entity, ParticelComponente& component)
	{
	}
}