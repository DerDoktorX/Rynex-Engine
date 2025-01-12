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

	namespace Utils {
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
	}

	Scene::Scene()
	{

	}

	Scene::~Scene()
	{

	}


#pragma region SceneObject

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
		Utils::CopyComponent(AllComponents{}, dstSceneRegistry, srcSceneRegistry, enttMap);
		newScene->Handle = other->Handle;
		return newScene;
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

#if RY_REALTION_SCHIP_ID_COMP
		entity.AddComponent<RealtionShipComponent>();
#elif RY_REALTION_SCHIP_ARRAY_COMP
		entity.AddComponent<RealtionShipsComponent>();
#endif

		entity.AddComponent<ModelMatrixComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;

		m_EntityMapID[uuid] = entity;
		m_EntityMapTag[name] = entity;
		m_EntityLeangth++;
		return entity;
	}
	
	void Scene::DestroyEntity(Entity entity)
	{
		entity.DestroyEntity();
		m_EntityMapID.erase(entity.GetUUID());
		m_EntityMapTag.erase(entity.GetComponent<TagComponent>().Tag);
		m_Registery.destroy(entity);
		m_EntityLeangth--;
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


#pragma endregion


#pragma region Runtime

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
				nsc.Instance->OnUpdate(ts.GetFPS());
			});
		
		for (EnttEntity e : scriptView)
		{
			Entity entity = { e, this };
			ScriptingEngine::OnUpdateEntity(entity, ts.GetSecounds());
		}


	}

	void Scene::OnRenderRuntime(const Ref<Framebuffer>& framebuffer, int camera)
	{
		Renderer3D::BeginFrame();
		EnttCameraView cameraView = m_Registery.view<ViewMatrixComponent, ModelMatrixComponent, CameraComponent>();
		EnttView3D enttView3D = {
			m_Registery.view<ModelMatrixComponent, DynamicMeshComponent>(),
			m_Registery.view<ModelMatrixComponent, StaticMeshComponent>(),
			m_Registery.view<ModelMatrixComponent, MaterialComponent, GeomtryComponent>()
		};
		EnttViewLigths enttViewLigths = {
			m_Registery.view<AmbientLigthComponent>(),
			m_Registery.view<ViewMatrixComponent, ModelMatrixComponent, DrirektionleLigthComponent>(),
			m_Registery.view<ModelMatrixComponent, PointLigthComponent>(),
			m_Registery.view<ViewMatrixComponent, ModelMatrixComponent, SpotLigthComponent>()
		};
		SetLigthsRuntime(enttViewLigths, enttView3D);
		EnttView2D enttView2D = { m_Registery.view<ModelMatrixComponent, SpriteRendererComponent>(),
			m_Registery.view<ModelMatrixComponent, TextComponent>()
		};
		m_RedererDefaultModeFlags = Renderer::GetMode();
		RenderFrambuffers(enttView3D, enttView2D, cameraView);

		Camera* mainCamera = nullptr;
		glm::mat4* mainTransform = nullptr;
		int count = 0;
		for (EnttEntity camerE : cameraView)
		{
			auto& [viewC, modelC, cameraC] = cameraView.get<ViewMatrixComponent, ModelMatrixComponent, CameraComponent>(camerE);
			if (cameraC.Primary)
			{
				if (count == camera)
				{

					mainCamera = &cameraC.Camera;
					mainTransform = &viewC.Globle;
					break;
				}
				count++;
			}
		}

		if (mainCamera && mainTransform)
		{
			
			framebuffer->Bind();
			RenderCommand::SetClearColor(m_BackGround);
			RenderCommand::Clear();
			framebuffer->ClearAttachment(1, -1);
			RenderScene3D(*mainCamera, *mainTransform, enttView3D);
			RenderScene2D(*mainCamera, *mainTransform, enttView2D);
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
		Renderer3D::EndFrame();
	}

	void Scene::SetLigthsRuntime(EnttViewLigths& enttViewLigths, EnttView3D& enttView3D)
	{
		EnttAmbientLView& ambientView = enttViewLigths.AmbientLCV;
		EnttDrirektionLeLView& directionelView = enttViewLigths.DrirektionLCV;
		EnttPointLView& pointView = enttViewLigths.PointLCV;
		EnttSpotLView& spotView = enttViewLigths.SpotLCV;

		int directionCount = 0;
		for (EnttEntity directionelE : directionelView)
		{
			auto& [viewC, modelC, directionelC] = directionelView.get<ViewMatrixComponent, ModelMatrixComponent, DrirektionleLigthComponent>(directionelE);
			Renderer3D::SetLigthUniform(directionelC, modelC.Globle, directionCount);

			directionCount++;
		}

		int pointCount = 0;
		for (EnttEntity pointE : pointView)
		{
			auto& [matrixC, pointC] = pointView.get<ModelMatrixComponent, PointLigthComponent>(pointE);
			Renderer3D::SetLigthUniform(pointC, matrixC.Globle, pointCount);
			pointCount++;
		}

		int spotCount = 0;
		for (EnttEntity spotE : spotView)
		{
			auto& [viewC, modelC, spotC] = spotView.get<ViewMatrixComponent, ModelMatrixComponent, SpotLigthComponent>(spotE);
			Renderer3D::SetLigthUniform(spotC, modelC.Globle, spotCount);
			spotCount++;
		}

		int ambientCount = 0;
		AmbientLigthComponent* useAmbientC = nullptr;
		for (EnttEntity ambientE : ambientView)
		{
			useAmbientC = &ambientView.get<AmbientLigthComponent>(ambientE);
			ambientCount++;
			break;
		}
		Renderer3D::SetLigthUniform(useAmbientC, ambientCount, directionCount, spotCount, pointCount);

	}

#pragma endregion

#pragma region Editor

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
#if 0
		auto scriptView = m_Registery.view<ScriptComponent>();
		for (EnttEntity scriptViewE : scriptView)
		{
			Entity entity = { scriptViewE, this };
			ScriptingEngine::OnDrawEntity(entity);
		}
#endif
		// ScriptingEngine::OnRuntimeStop();
#endif

	}

	void Scene::OnRenderEditor(const Ref<Framebuffer>& framebuffer, const Ref<EditorCamera>& editorCamera)
	{
		Renderer3D::BeginFrame();
		EnttCameraView cameraView = m_Registery.view<ViewMatrixComponent, ModelMatrixComponent, CameraComponent>();

		EnttView3D enttView3D = {
			m_Registery.view<ModelMatrixComponent, DynamicMeshComponent>(),
			m_Registery.view<ModelMatrixComponent, StaticMeshComponent>(),
			m_Registery.view<ModelMatrixComponent, MaterialComponent, GeomtryComponent>()
	};
		EnttViewLigths enttViewLigths = {
			m_Registery.view<AmbientLigthComponent>(),
			m_Registery.view<ViewMatrixComponent, ModelMatrixComponent, DrirektionleLigthComponent>(),
			m_Registery.view<ModelMatrixComponent, PointLigthComponent>(),
			m_Registery.view<ViewMatrixComponent, ModelMatrixComponent, SpotLigthComponent>()
		};
		Camera& mainCamera = (Camera)editorCamera->GetProjektion();
		glm::mat4 viewMatrix = editorCamera->GetViewMatrix();
		EnttView2D enttView2D = {
			m_Registery.view<ModelMatrixComponent, SpriteRendererComponent>(),
			m_Registery.view<ModelMatrixComponent, TextComponent>()
		};

		RenderFrambuffers(enttView3D, enttView2D, cameraView);	

		const glm::uvec2& size = framebuffer->GetFrambufferSize();
		SetLigthsEditor(enttViewLigths, enttView3D, mainCamera, viewMatrix, size);
	
		framebuffer->Bind();
		RenderCommand::SetClearColor(m_BackGround);
		RenderCommand::Clear();
		framebuffer->ClearAttachment(1, -1);
		m_RedererDefaultModeFlags = Renderer::GetMode(); 
		
		for (EnttEntity camerE : cameraView)
		{
			auto& [viewC, modelC, camerC] = cameraView.get<ViewMatrixComponent, ModelMatrixComponent, CameraComponent>(camerE);
			Renderer2D::DrawCameraIcon(modelC.Globle, (int)camerE);
			glm::mat4 viewCamera = viewC.Globle;
			if (camerC.ViewFustrum)
			{
				
				std::array<glm::vec4, 8> fustemWorldCamera = camerC.Camera.GetViewFustrumWorld(viewCamera);
				glm::mat4 viewProjetionCamera = (camerC.Camera.GetProjektion() * viewCamera);
				glm::mat4 inverseViewProjetionCamera = glm::inverse(viewProjetionCamera);
				glm::vec3 centerCamerViewFustrum = camerC.Camera.GetWorldCameraCenter(viewCamera);

				for (EnttEntity dirLigth : enttViewLigths.DrirektionLCV)
				{
					auto& [viewC, ligthModelC, ligthDirectionC] = enttViewLigths.DrirektionLCV.get<ViewMatrixComponent, ModelMatrixComponent, DrirektionleLigthComponent>(dirLigth);
					
					glm::mat4 viewLigth = SceneCamera::GetShadowViewMatrix(centerCamerViewFustrum, ligthModelC.Globle[3]);
					auto[min, max] = SceneCamera::GetMinMaxViewFustrumInSpace(viewLigth, fustemWorldCamera);
					
					glm::mat4 ViewProjetionLigth = Renderer3D::CalculateShadowDirectionelMatrix(viewLigth, min, max);
					glm::mat4 inverseViewProjetionLigth = glm::inverse(ViewProjetionLigth);
					Renderer3D::DrawLineBoxAABB(SceneCamera::GetViewFustrum(), inverseViewProjetionLigth, glm::vec3(ligthModelC.Globle[3]), (int)dirLigth);

				}
				Renderer3D::DrawLineBoxAABB(SceneCamera::GetViewFustrum(), inverseViewProjetionCamera, glm::vec3(modelC.Globle[3]), (int)camerE);
			}
		}
		RenderScene3D(mainCamera, viewMatrix, enttView3D);
		RenderScene2D(mainCamera, viewMatrix, enttView2D);
		Renderer2D::EndSceneIcon();
		framebuffer->Unbind();
		Renderer3D::EndFrame();
	}

	void Scene::EditorFilterSreene()
	{

	}

	void Scene::SetLigthsEditor(EnttViewLigths& enttViewLigths, EnttView3D& enttView3D, Camera& camera, const glm::mat4& viewMatrix, const glm::uvec2& viewPortSize)
	{
		EnttAmbientLView& ambientView = enttViewLigths.AmbientLCV;
		EnttDrirektionLeLView& directionelView = enttViewLigths.DrirektionLCV;
		EnttPointLView& pointView = enttViewLigths.PointLCV;
		EnttSpotLView& spotView = enttViewLigths.SpotLCV;

		int ligthIndex = 0;
		int directionCount = 0;

		Renderer2D::BeginSceneIcon(camera, viewMatrix, viewPortSize);
		for (EnttEntity directionelE : directionelView)
		{
			auto& [viewC, modelC, directionelC] = directionelView.get<ViewMatrixComponent, ModelMatrixComponent, DrirektionleLigthComponent>(directionelE);
			// glm::mat4 matrix = glm::lookAt(glm::vec3(matrixC.GlobleMatrix4x4[3]), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
			Renderer3D::SetLigthUniform(directionelC, modelC.Globle, directionCount);
			directionCount++;
			Renderer2D::DrawLigthDirctionelIcon(modelC.Globle, (int)directionelE);

			
			// RenderScene3DShadows(matrixC.Globle, enttView3D, ligthIndex);
			ligthIndex++;

		}

		int pointCount = 0;
		for (EnttEntity pointE : pointView)
		{
			auto& [matrixC, pointC] = pointView.get<ModelMatrixComponent, PointLigthComponent>(pointE);
			
			Renderer3D::SetLigthUniform(pointC, matrixC.Globle, pointCount);
			pointCount++;
			Renderer2D::DrawLigthPointIcon(matrixC.Globle, (int)pointE);
			ligthIndex++;
		}

		int spotCount = 0;
		for (EnttEntity spotE : spotView)
		{
			auto& [viewC, modelC, spotC] = spotView.get<ViewMatrixComponent, ModelMatrixComponent, SpotLigthComponent>(spotE);
			
			Renderer3D::SetLigthUniform(spotC, modelC.Globle, spotCount);
			spotCount++;
			Renderer2D::DrawLigthSpotIcon(modelC.Globle, (int)spotE);
			ligthIndex++;
		}
		
		int ambientCount = 0;
		AmbientLigthComponent* useAmbientC = nullptr;
		for (EnttEntity ambientE : ambientView)
		{
			useAmbientC = &ambientView.get<AmbientLigthComponent>(ambientE);
			ambientCount++;
			break;
		}
		Renderer3D::SetLigthUniform(useAmbientC, ambientCount, directionCount, spotCount, pointCount);
		Renderer3D::SetShadowsUniform();
	}


#pragma endregion

#pragma region Simulation

	void Scene::OnUpdateSimulation(TimeStep ts)
	{
		OnUpdateEditor(ts);
	}

	void Scene::OnRenderSimulation(const Ref<Framebuffer>& framebuffer, const Ref<EditorCamera>& camera)
	{
		OnRenderEditor(framebuffer, camera);
	}

#pragma endregion


#pragma region Renderer

#if 0
	void Scene::RenderSingleEntityEdgeDitection(Entity entity, Camera& camera, const glm::mat4& viewMatrix, const glm::vec4& backGroundColor, const Ref<Framebuffer> frameBuffer)
	{
		

		frameBuffer->Bind();
		RenderCommand::Clear();
		RenderCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });

		if (!entity)
		{
			frameBuffer->Unbind();
			return;
		}

		

		if ((entity != Entity() || entity != 0) && entity.HasComponent<ModelMatrixComponent>())
		{
			ModelMatrixComponent& mat4C = entity.GetComponent<ModelMatrixComponent>();
			if (entity.HasComponent<SpriteRendererComponent>())
			{
				Renderer2D::BeginSceneQuade(camera, viewMatrix);
				SpriteRendererComponent& spriteC = entity.GetComponent<SpriteRendererComponent>();
				Renderer2D::DrawSprite(mat4C.Globle, spriteC, entity.GetEntityHandle());
				Renderer2D::EndSceneQuade();
			}

			if (entity.HasComponent<CameraComponent>())
			{
				Renderer2D::BeginSceneIcon(camera, viewMatrix, frameBuffer->GetFrambufferSize());
				Renderer2D::DrawCameraIcon(mat4C.Globle, entity.GetEntityHandle());
				Renderer2D::EndSceneIcon();
			}
			if (entity.HasComponent<Rynex::DrirektionleLigthComponent>())
			{
				Renderer2D::BeginSceneIcon(camera, viewMatrix, frameBuffer->GetFrambufferSize());
				Renderer2D::DrawLigthDirctionelIcon(mat4C.Globle, entity.GetEntityHandle());
				Renderer2D::EndSceneIcon();
			}
			if (entity.HasComponent<Rynex::SpotLigthComponent>())
			{
				Renderer2D::BeginSceneIcon(camera, viewMatrix, frameBuffer->GetFrambufferSize());
				Renderer2D::DrawLigthSpotIcon(mat4C.Globle, entity.GetEntityHandle());
				Renderer2D::EndSceneIcon();
			}
			if (entity.HasComponent<Rynex::PointLigthComponent>())
			{
				Renderer2D::BeginSceneIcon(camera, viewMatrix, frameBuffer->GetFrambufferSize());
				Renderer2D::DrawLigthPointIcon(mat4C.Globle, entity.GetEntityHandle());
				Renderer2D::EndSceneIcon();
			}
#if 0
			else  if (slelcted.HasComponent<GeomtryComponent>())
			{
				Renderer3D::BeginScene(mainCamera, viewMatrix);

				if (geomtryC.Geometry != nullptr)
				{

					Renderer3D::BeforDrawEntity(m_MaterialC, mat4C.Globle, slelcted.GetEntityHandle());
					Renderer3D::DrawObjectRender3D(geomtryC.Geometry);
					Renderer3D::AfterDrawEntity(m_MaterialC);
				}
				Renderer3D::EndScene();
			}
#endif
			if (entity.HasComponent<StaticMeshComponent>())
			{
				Renderer3D::BeginScene(camera, viewMatrix);
				StaticMeshComponent& staticMeshC = entity.GetComponent<StaticMeshComponent>();


				if (staticMeshC.ModelR != nullptr)
				{

					const std::vector<Ref<Mesh>>& meshes = staticMeshC.ModelR->GetMeshes();
#if 0
					const std::vector<glm::mat4>& globels = staticMeshC.GlobleMeshMatrix;
					for (uint32_t i = 0, length = globels.size(); i < length; i++)
					{
						Renderer3D::DrawModdelSeclection(globels[i], meshes[i], m_MaterialC.Materiel, slelcted.GetEntityHandle());
					}
#elif 0
					for (const Ref<Mesh>& mesh : meshes)
					{
						Renderer3D::DrawModdel(mat4C.Globle, staticMeshC, slelcted.GetEntityHandle());
					}
#else
					for (const Ref<Mesh>& mesh : meshes)
					{
						Renderer3D::DrawModdelSeclection(mat4C.Globle, mesh, m_MaterialC.Materiel, entity.GetEntityHandle());
					}
#endif
				}
				Renderer3D::EndScene();
			}
			if (entity.HasComponent<DynamicMeshComponent>())
			{
				DynamicMeshComponent& dynamicMeshC = entity.GetComponent<DynamicMeshComponent>();
				UUID parent = entity.GetComponent<RealtionShipComponent>().ParentID;
				Renderer3D::BeginScene(camera, viewMatrix);
				if (parent)
				{
					if (dynamicMeshC.MeshR != nullptr)
					{
						m_MaterialC.Materiel->SetMatrix(mat4C.Globle);
						Renderer3D::DrawModdelSeclection(mat4C.Globle, dynamicMeshC.MeshR, m_MaterialC.Materiel, entity.GetEntityHandle());

					}
				}
				Renderer3D::EndScene();
			}
		}

		frameBuffer->Unbind();
	}
#endif

	void Scene::RenderScene2D(Camera& camera, glm::mat4& viewMatrix, EnttView2D& enttView2D)
	{
		EnttRender2DView& enttRender2DView = enttView2D.Renderer2DCV;
		EnttRenderTextView& enttRenderTextView = enttView2D.RendererTextCV;
		RY_PROFILE_SCOPE("Scene Draw 2D");
		Renderer2D::BeginSceneQuade(camera, viewMatrix);
		for (EnttEntity render2dE : enttRender2DView)
		{
			auto& [transformC, spriteC] = enttRender2DView.get< ModelMatrixComponent, SpriteRendererComponent>(render2dE);
			Renderer2D::DrawSprite(transformC.Globle, spriteC, (int)render2dE);
		}
		for (EnttEntity render2dE : enttRenderTextView)
		{
			auto& [transformC, textC] = enttRenderTextView.get< ModelMatrixComponent, TextComponent>(render2dE);
			Renderer2D::DrawString(transformC.Globle, textC, (int)render2dE);
		}
		Renderer2D::EndSceneQuade();
	}
	
	void Scene::RenderScene3D(Camera& camera, glm::mat4& viewMatrix, EnttView3D& enttView3D)
	{
		RY_PROFILE_SCOPE("Scene Draw 3D");
		EnttRender3DEditeView& enttRender3DEditeView = enttView3D.EditeCV;
		Renderer3D::BeginScene(camera, viewMatrix);

		for (EnttEntity render3dE : enttRender3DEditeView)
		{
			auto& [tranformC, materialC, geomtryC] = enttRender3DEditeView.get<ModelMatrixComponent, MaterialComponent, GeomtryComponent>(render3dE);
			Ref<VertexArray> vertexArray = geomtryC.Geometry;
			if (vertexArray == nullptr)
				continue;
			Renderer3D::BeforDrawEntity(materialC, tranformC.Globle, (int)render3dE);
			Renderer3D::DrawObjectRender3D(vertexArray);
			Renderer3D::AfterDrawEntity(materialC);
		}

		EnttRender3DStaticModelView& enttRender3DStaticModelView = enttView3D.StaticCV;

		for (EnttEntity render3dE : enttRender3DStaticModelView)
		{
			auto& [tranformC,  geomtryC] = enttRender3DStaticModelView.get<ModelMatrixComponent, StaticMeshComponent>(render3dE);

			if (geomtryC.ModelR == nullptr)
				continue;
			Renderer3D::DrawModdel( tranformC.Globle, geomtryC, (int)render3dE);
		}
		EnttRender3DDynamicModelView& enttRender3DDynamicModelView = enttView3D.DynamicModelCV;

		for (EnttEntity render3dE : enttRender3DDynamicModelView)
		{
			auto& [tranformC, geomtryC] = enttRender3DDynamicModelView.get<ModelMatrixComponent,  DynamicMeshComponent>(render3dE);
			if (geomtryC.MeshR == nullptr)
				continue;
			// Renderer3D::DrawLineBoxAABB(geomtryC.MeshR->GetVertexArray()->GetBoxAABB(), tranformC.GlobleMatrix4x4, (int)render3dE);
			Renderer3D::DrawModdel( tranformC.Globle, geomtryC, (int)render3dE);

		}
		Renderer3D::EndScene();

	}
	
	void Scene::RenderFrambuffers( EnttView3D& enttView3D, EnttView2D& enttView2D, EnttCameraView& enttCameraView)
	{
		EnttFrameBufferView frambufferView = m_Registery.view<ViewMatrixComponent, ModelMatrixComponent, FrameBufferComponent, CameraComponent>();
		for (EnttEntity frameE : frambufferView)
		{
			auto& [viewC, modelC, frambufferC, camerC] = frambufferView.get<ViewMatrixComponent, ModelMatrixComponent, FrameBufferComponent, CameraComponent>(frameE);
			if (frambufferC.FrameBuffer)
			{
				frambufferC.FrameBuffer->Bind();
				RenderCommand::SetClearColor(m_BackGround);
				RenderCommand::Clear();
				RenderScene3D(camerC.Camera, viewC.Globle, enttView3D);
				RenderScene2D(camerC.Camera, viewC.Globle, enttView2D);
				frambufferC.FrameBuffer->Unbind();
			}
			
		}
	}

	void Scene::RenderScene3DShadows(glm::mat4& transform, EnttView3D& enttView3D, int ligthIndex)
	{
		RY_PROFILE_SCOPE("Scene Shadow Draw 3D");
		EnttRender3DEditeView& enttRender3DEditeView = enttView3D.EditeCV;

		RenderCommand::SetClearColor({0.f,0.f,0.f,1.f});
		RenderCommand::Clear();
		
		int flagmode = RenderCommand::GetMode();
		RenderCommand::SetMode(Renderer::Mode::CallFace_None | Renderer::Mode::Death_Buffer);
		Renderer3D::BeginSceneShadow(transform);


		EnttRender3DStaticModelView& enttRender3DStaticModelView = enttView3D.StaticCV;

		for (EnttEntity render3dE : enttRender3DStaticModelView)
		{
			auto& [tranformC, geomtryC] = enttRender3DStaticModelView.get<ModelMatrixComponent, StaticMeshComponent>(render3dE);

			if (geomtryC.ModelR == nullptr)
				continue;
			Renderer3D::DrawModdelShadow(tranformC.Globle, geomtryC, (int)render3dE);
		}

		Renderer3D::EndSceneShadow(ligthIndex);

		RenderCommand::SetMode(flagmode);
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
	void Scene::OnComponentAdded<AmbientLigthComponent>(Entity entity, AmbientLigthComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<DrirektionleLigthComponent>(Entity entity, DrirektionleLigthComponent& component)
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

#pragma endregion

}