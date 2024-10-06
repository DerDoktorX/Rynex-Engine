#include "rypch.h"
#include "Scene.h"

#include "Entity.h"
#include "ScriptableEntity.h"
#include "Rynex/Renderer/Rendering/Renderer2D.h"
#include "Rynex/Renderer/Rendering/Renderer3D.h"
#include "Rynex/Scripting/ScriptingEngine.h"
#include "Rynex/Serializers/VertexArraySerialzer.h"
#include "Rynex/Asset/Base/AssetMetadata.h"
#include "Rynex/Asset/EditorAssetManager.h"
#include "Rynex/Project/Project.h"


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
		ScriptingEngine::OnRuntimeStart(this);
		// Instandiat

		auto view = m_Registery.view<ScriptComponent>();
		for(auto e : view)
		{
			Entity entity = { e, this };
			ScriptingEngine::OnCreatEntity(entity);
		}
	}

	void Scene::OnRuntimStop()
	{
		ScriptingEngine::OnRuntimeStop();
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_EntityMapID.erase(entity.GetUUID());
		m_EntityMapTag.erase(entity.GetComponent<TagComponent>().Tag);
		m_Registery.destroy(entity);
		m_EntityLeangth--;
	}

	void Scene::OnUpdateEditor(TimeStep ts)
	{
		if (!ScriptingEngine::ReloadeScriptAvaible()) return;

		ScriptingEngine::OnRuntimeStart(this);
		auto scriptView = m_Registery.view<ScriptComponent>();
		for (EnttEntity scriptViewE : scriptView)
		{
			Entity entity = { scriptViewE, this };
			ScriptingEngine::OnDrawEntity(entity);
		}
		ScriptingEngine::OnRuntimeStop();
#if 0
		{
			auto entiy = GetEntitiyByUUID(8976786);
			if(entiy.HasComponent<GeomtryComponent>())
			{
				auto& geomtryC = entiy.GetComponent<GeomtryComponent>();
				auto& vertexArray = geomtryC.Geometry;
				vertexArray->Handle = AssetHandle();
				AssetMetadata metadata;
				metadata.FilePath = "Assets/VertexArrayDefault.ryarray";
				metadata.Type = vertexArray->GetType();
				Ref<EditorAssetManager> editorAssetManager = Project::GetActive()->GetEditorAssetManger();
				editorAssetManager->CreateAsset(metadata.FilePath, (Ref<Asset>)vertexArray, metadata);
				VertexArraySerialzer::Serialzer("Assets/VertexArrayDefault.ryarray", vertexArray);
			}
		}
#endif

	}

	void Scene::OnRenderEditor(const Ref<Framebuffer>& framebuffer, const Ref<EditorCamera>& editorCamera)
	{	
		EnttRender3DEditeView render3dEditeView = m_Registery.view<Matrix4x4Component, MaterialComponent, GeomtryComponent>();
		EnttRender3DStaticModelView render3DStaticModelView = m_Registery.view<Matrix4x4Component, MaterialComponent, StaticMeshComponent>();
		EnttRender3DDynamicModelView render3DDynamicModelView = m_Registery.view<Matrix4x4Component, RealtionShipComponent, DynamicMeshComponent>();
		EnttRender2DView render2dView = m_Registery.view<Matrix4x4Component, SpriteRendererComponent>();

		RenderFrambuffers(render3dEditeView, render3DStaticModelView, render3DDynamicModelView, render2dView);

		Camera& mainCamera = (Camera)editorCamera->GetProjektion();
		glm::mat4 viewMatrix = editorCamera->GetViewMatrix();

		framebuffer->Bind();
		RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		RenderCommand::Clear();
		framebuffer->ClearAttachment(2, -1);
		RenderScene2D(mainCamera, viewMatrix, render2dView);
		RenderScene3D(mainCamera, viewMatrix, render3dEditeView, render3DStaticModelView, render3DDynamicModelView);
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
			ScriptingEngine::OnUpdateEntity(entity, ts);
		}

		
	}

	void Scene::OnRenderRuntime(const Ref<Framebuffer>& framebuffer, int camera)
	{
		EnttCameraView cameraView = m_Registery.view<Matrix4x4Component, CameraComponent>();
		EnttRender3DEditeView render3dEditeView = m_Registery.view<Matrix4x4Component, MaterialComponent, GeomtryComponent>();
		EnttRender3DStaticModelView render3DStaticModelView = m_Registery.view<Matrix4x4Component, MaterialComponent, StaticMeshComponent>();
		EnttRender3DDynamicModelView render3DDynamicModelView = m_Registery.view<Matrix4x4Component, RealtionShipComponent, DynamicMeshComponent>();
		EnttRender2DView render2dView = m_Registery.view<Matrix4x4Component, SpriteRendererComponent>();
		
		RenderFrambuffers(render3dEditeView, render3DStaticModelView, render3DDynamicModelView, render2dView);

		Camera* mainCamera = nullptr;
		const glm::mat4* mainTransform;
		int count = 0;
		for (EnttEntity camerE : cameraView)
		{
			auto& [tranformC, cameraC] = cameraView.get<Matrix4x4Component, CameraComponent>(camerE);
			if (cameraC.Primary)
			{
				if(count == camera)
				{

					mainCamera = &cameraC.Camera;
					mainTransform = &tranformC.GlobleMatrix4x4;
					break;
				}
				count++;
			}
		}

		if(mainCamera)
		{
			framebuffer->Bind();
			RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
			RenderCommand::Clear();
			framebuffer->ClearAttachment(2, -1);
			RenderScene2D(*mainCamera, glm::inverse(*mainTransform), render2dView);
			RenderScene3D(*mainCamera, glm::inverse(*mainTransform), render3dEditeView, render3DStaticModelView, render3DDynamicModelView);
			framebuffer->Unbind();
		}
		else
		{
			framebuffer->Bind();
			RenderCommand::SetClearColor({ 1.0f, 0.0f, 0.0f, 1.0f });
			RenderCommand::Clear();
			framebuffer->ClearAttachment(2, -1);
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


	void Scene::RenderScene2D(Camera& camera, glm::mat<4, 4, float>& viewMatrix, EnttRender2DView& enttRender2DView)
	{
#if 0
#if RENDERER_2D
		Renderer2D::BeginScene(camera, viewMatrix);
#if QUADS_DRAW
		{
			auto framebuffer = m_Registery.view<CameraComponent, FrameBufferComponent, TransformComponent>();
			auto mainViewPort = m_Registery.view<MainViewPortComponent>();
			auto view2D = m_Registery.view<SpriteRendererComponent, TransformComponent>();
			for (auto frame : framebuffer)
			{
				auto& [cameraC, frambufferC, transformC] = framebuffer.get<CameraComponent, FrameBufferComponent, TransformComponent>(frame);
				frambufferC.FrameBuffer->Bind();
				//frambufferC.FrameBuffer->ClearAttachment(2, -1);
				RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
				RenderCommand::Clear();
				Renderer2D::BeginScene(cameraC.Camera, transformC.GetTransform());
				for (auto entity : view2D)
				{
					auto& [sprite, transform] = view2D.get<SpriteRendererComponent, TransformComponent>(entity);
					Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
				}
				Renderer2D::EndScene();
				frambufferC.FrameBuffer->Unbind();
			}

			for (auto viewPort : mainViewPort)
			{
				auto& viewPortC = mainViewPort.get<MainViewPortComponent>(viewPort);
				viewPortC.FrameBuffer->Bind();
				viewPortC.FrameBuffer->ClearAttachment(2, -1);
				RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
				RenderCommand::Clear();
				Renderer2D::BeginScene(camera, viewMatrix);

				for (auto entity : view2D)
				{
					auto& [sprite, transform] = view2D.get<SpriteRendererComponent, TransformComponent>(entity);
					Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
				}
				Renderer2D::EndScene();
				viewPortC.FrameBuffer->Unbind();
			}

		}
		
#endif
#if CIRCLE_DRAW
		{
			auto view = m_Registery.view<TransformComponent, CircleRendererComponent>();
			for (auto entity : view)
			{
				auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);

				Renderer2D::DrawCircle(transform.GetTransform(), circle.Color, circle.Thickness, circle.Fade, (int)entity);
			}
		}
#endif
#if TEXT_DRAW
		// Draw text
		{
			auto view = m_Registry.view<TransformComponent, TextComponent>();
			for (auto entity : view)
			{
				auto [transform, text] = view.get<TransformComponent, TextComponent>(entity);

				Renderer2D::DrawString(text.TextString, transform.GetTransform(), text, (int)entity);
			}
		}
#endif
		//Renderer2D::EndScene();
#endif
#else		
		RY_PROFILE_SCOPE("Scene Draw 2D");
		Renderer2D::BeginScene(camera, viewMatrix);
		for (EnttEntity render2dE : enttRender2DView)
		{
			auto& [transformC, spriteC] = enttRender2DView.get< Matrix4x4Component, SpriteRendererComponent>(render2dE);
			Renderer2D::DrawSprite(transformC.GlobleMatrix4x4, spriteC, (int)render2dE);
		}
		Renderer2D::EndScene();
#endif
	}

	void Scene::RenderScene3D(Camera& camera, glm::mat<4, 4, float>& viewMatrix, EnttRender3DEditeView& enttRender3DEditeView, EnttRender3DStaticModelView& enttRender3DStaticModelView, EnttRender3DDynamicModelView& enttRender3DDynamicModelView)
	{
		RY_PROFILE_SCOPE("Scene Draw 3D")
#if 0
#if RENDERER_3D			
		{			
			auto mainViewPort = m_Registery.view<MainViewPortComponent>();
			auto view3D = m_Registery.view<TransformComponent, MaterialComponent, GeomtryComponent>();
#if 1
			////////////////////////////////////////////////////////////
			/// Renderer 3D ////////////////////////////////////////////
			////////////////////////////////////////////////////////////
			auto framebuffer = m_Registery.view<CameraComponent, FrameBufferComponent, TransformComponent>();

			
			for(auto frame : framebuffer)
			{
				
				auto& [cameraC, frambufferC, transformC] = framebuffer.get<CameraComponent, FrameBufferComponent, TransformComponent>(frame);
				frambufferC.FrameBuffer->Bind();
				//frambufferC.FrameBuffer->ClearAttachment(2, -1);
				RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
				RenderCommand::Clear();
				Renderer3D::BeginScene(cameraC.Camera, transformC.GetTransform());
				
				for (auto entity3D : view3D)
				{
					auto& [tranform, material, geomtry] = view3D.get<TransformComponent, MaterialComponent, GeomtryComponent>(entity3D);
					Ref<VertexArray> vertexArray = geomtry.Geometry;
					if (material.Shader == nullptr || vertexArray == nullptr)
						continue;
					Renderer3D::BeforDrawEntity(material, tranform.GetTransform());
					// DrawObjectRender3D(vertexArray);
					s_SceneRenderFuncs.at(vertexArray->GetPrimitv())(vertexArray);
					Renderer3D::AfterDrawEntity(material);
				}
				Renderer3D::EndScene();
				frambufferC.FrameBuffer->Unbind();
			}
#endif
			////////////////////////////////////////////////////////////
			/// Main Renderer3D ////////////////////////////////////////
			////////////////////////////////////////////////////////////


			
			for (auto viewPort : mainViewPort)
			{
				
				auto& viewPortC = mainViewPort.get<MainViewPortComponent>(viewPort);
				
				
				viewPortC.FrameBuffer->Bind();
				viewPortC.FrameBuffer->ClearAttachment(2, -1);
				RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
				RenderCommand::Clear();
				Renderer3D::BeginScene(camera, viewMatrix);
				for (auto entity3D : view3D)
				{
					auto& [tranform, material, geomtry] = view3D.get<TransformComponent, MaterialComponent, GeomtryComponent>(entity3D);
					Ref<VertexArray> vertexArray = geomtry.Geometry;
					if (material.Shader == nullptr || vertexArray == nullptr)
						continue;
					Renderer3D::BeforDrawEntity(material, tranform.GetTransform());
					// DrawObjectRender3D(vertexArray);
					s_SceneRenderFuncs.at(vertexArray->GetPrimitv())(vertexArray);
					Renderer3D::AfterDrawEntity(material);
				}
				Renderer3D::EndScene();
				viewPortC.FrameBuffer->Unbind();
			}
		}
#endif
#else
		Renderer3D::BeginScene(camera, viewMatrix);
		for (EnttEntity render3dE : enttRender3DEditeView)
		{
			auto& [tranformC, materialC, geomtryC] = enttRender3DEditeView.get< Matrix4x4Component, MaterialComponent, GeomtryComponent>(render3dE);
			Ref<VertexArray> vertexArray = geomtryC.Geometry;
			if (vertexArray == nullptr)
				continue;
			Renderer3D::BeforDrawEntity(materialC, tranformC.GlobleMatrix4x4, (int)render3dE);
			
			//s_SceneRenderFuncs.at(vertexArray->GetPrimitv())(vertexArray);
			Renderer3D::DrawObjectRender3D(vertexArray);
			Renderer3D::AfterDrawEntity(materialC);
		}
		for (EnttEntity render3dE : enttRender3DStaticModelView)
		{
			auto& [tranformC, materialC, geomtryC] = enttRender3DStaticModelView.get<Matrix4x4Component, MaterialComponent, StaticMeshComponent>(render3dE);
			
			if (geomtryC.ModelR == nullptr)
				continue;
			Renderer3D::DrawModdel(materialC, tranformC.GlobleMatrix4x4, geomtryC, (int)render3dE );
			
		}
		for (EnttEntity render3dE : enttRender3DDynamicModelView)
		{
			auto& [tranformC, geomtryC, realtionC] = enttRender3DDynamicModelView.get<Matrix4x4Component, DynamicMeshComponent, RealtionShipComponent>(render3dE);
			if (realtionC.ParentID == 0)
				continue;
			MaterialComponent& materialC = GetEntitiyByUUID(realtionC.ParentID).GetComponent<MaterialComponent>();
			if (geomtryC.MeshR == nullptr)
				continue;
			Renderer3D::DrawModdel(materialC, tranformC.GlobleMatrix4x4, geomtryC, (int)render3dE);

		}
		Renderer3D::EndScene();
#endif	

	}

	void Scene::RenderFrambuffers(EnttRender3DEditeView& enttRender3DEditeView, EnttRender3DStaticModelView& enttRender3DStaticModelView, EnttRender3DDynamicModelView& enttRender3DDynamicModelView, EnttRender2DView& enttRender2DView)
	{
		EnttFrameBufferView frambufferView = m_Registery.view<Matrix4x4Component, FrameBufferComponent, CameraComponent>();
		for (EnttEntity frameE : frambufferView)
		{
			auto& [transformC, frambufferC, camerC] = frambufferView.get<Matrix4x4Component, FrameBufferComponent, CameraComponent>(frameE);
			if(frambufferC.FrameBuffer)
			{
				frambufferC.FrameBuffer->Bind();
				RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
				RenderCommand::Clear();
				RenderScene2D(camerC.Camera, glm::inverse(transformC.GlobleMatrix4x4), enttRender2DView);
				RenderScene3D(camerC.Camera, glm::inverse(transformC.GlobleMatrix4x4), enttRender3DEditeView, enttRender3DStaticModelView, enttRender3DDynamicModelView);

				frambufferC.FrameBuffer->Unbind();
			}
		}
	}

	void Scene::SceneRendering(Camera& camera, glm::mat<4, 4, float>& viewMatrix)
	{

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
		if(m_EntityMapID.find(uuid) != m_EntityMapID.end()) 
			return { m_EntityMapID.at(uuid), this};

		return {};
	}

	bool Scene::IsTagInScene(const std::string& tag)
	{
		return (m_EntityMapTag.find(tag) != m_EntityMapTag.end());
	}

	Entity Scene::GetPrimaryCameraEntity()
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
}