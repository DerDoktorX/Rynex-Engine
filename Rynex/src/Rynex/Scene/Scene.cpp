#include "rypch.h"
#include "Scene.h"

#include "Entity.h"
#include "Components.h"
#include "ScriptableEntity.h"
#include "Rynex/Renderer/Rendering/Renderer2D.h"
#include "Rynex/Renderer/Rendering/Renderer3D.h"
#include "Rynex/Scripting/ScriptingEngine.h"



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
	
	static void DrawObjectRender3D(const Ref<VertexArray>& vertexArray)
	{
		switch (vertexArray->GetPrimitv())
		{
			case VertexArray::Primitv::Traingle:
			{
				Renderer3D::DrawMesh(vertexArray);
				return;
			}
			case VertexArray::Primitv::TraingleStrips:
			{
				Renderer3D::DrawMeshStrips(vertexArray);

				return;
			}
			case VertexArray::Primitv::TraingleFan:
		{
			RY_CORE_ASSERT(false, "Primitv TraingleFan is not Implemantent jet!");
			//Renderer3D::DrawMeshStrips(geomtry.Geometry);
			return;
		}
			case VertexArray::Primitv::Line:
			{
				Renderer3D::DrawLine(vertexArray);
				return;
			}
			case VertexArray::Primitv::LineLoop:
			{
				//RY_CORE_ASSERT(false, "Primitv LineLoop is not Implemantent jet!");
				Renderer3D::DrawLineLoop(vertexArray);
				return;
			}
			case VertexArray::Primitv::LineStrips:
			{
				RY_CORE_ASSERT(false, "Primitv LineStrips is not Implemantent jet!");
				return;
			}
			case VertexArray::Primitv::Points:
			{
				Renderer3D::DrawPoints(vertexArray);
				return;
			}
			case VertexArray::Primitv::Patches:
			{
				Renderer3D::DrawPatches(vertexArray);
				return;
			}
			default:
				RY_CORE_ASSERT(false, "Primitv type is Unknown!");
				break;
		}
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
		RY_PROFILE_FUNCTION();
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
		RY_PROFILE_FUNCTION();
		return CreateEntityWitheUUID(UUID(), name);
	}

	Entity Scene::CreateEntityWitheUUID(UUID uuid, const std::string& name)
	{
		RY_PROFILE_FUNCTION();
		Entity entity = { m_Registery.create(), this };
		
		entity.AddComponent<IDComponent>(uuid);
		entity.AddComponent<TransformComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;

		m_EntityMapID[uuid] = entity;
		m_EntityMapTag[name] = entity;

		return entity;
	}

	void Scene::OnRuntimStart()
	{
		RY_PROFILE_FUNCTION();
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
		RY_PROFILE_FUNCTION();
		ScriptingEngine::OnRuntimeStop();
	}


	void Scene::DestroyEntity(Entity entity)
	{
		RY_PROFILE_FUNCTION();
		m_EntityMapID.erase(entity.GetUUID());
		m_EntityMapTag.erase(entity.GetComponent<TagComponent>().Tag);
		m_Registery.destroy(entity);
	}


	void Scene::OnUpdateEditor(TimeStep ts, EditorCamera& camera)
	{

		RY_PROFILE_FUNCTION();
#if 0
#if RENDERER_3D
		////////////////////////////////////////////////////////////
		/// Renderer 3D ////////////////////////////////////////////
		////////////////////////////////////////////////////////////
		
		


		{
			Renderer3D::BeginScene(camera);
			auto group3D = m_Registery.group<TransformComponent>(entt::get<MaterialComponent, GeomtryComponent>);
			for (auto entity3D : group3D)
			{
				auto& [tranform, material, geomtry] = group3D.get<TransformComponent, MaterialComponent, GeomtryComponent>(entity3D);
				if (material.Shader == nullptr || geomtry.Geometry == nullptr)
					continue;
				Renderer3D::BeforDrawEntity(material, tranform.GetTransform());
				Renderer3D::DrawMeshStrips(geomtry.Geometry);
				//Renderer3D::AfterDrawEntity(material);
			}
			Renderer3D::EndScene();
		}
#endif	
#if RENDERER_2D
		////////////////////////////////////////////////////////////
		/// Renderer 2D ////////////////////////////////////////////
		////////////////////////////////////////////////////////////

		{
			Renderer2D::BeginScene(camera);

			 auto group2D = m_Registery.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			// auto view2D = m_Registery.view<TransformComponent, SpriteRendererComponent>();
			for (auto entity2D : group2D) //for (auto entity2D : view2D)
			{
				auto& [tranform, sprite] = group2D.get<TransformComponent, SpriteRendererComponent>(entity2D);
				//auto& [tranform, sprite] = view2D.get<TransformComponent, SpriteRendererComponent>(entity2D);
				Renderer2D::DrawSprite(tranform.GetTransform(), sprite, (int)entity2D);
			}

			Renderer2D::EndScene();
		}
#endif	
#else
		if(ScriptingEngine::ReloadeScriptAvaible())
		{
			ScriptingEngine::OnRuntimeStart(this);
			auto view = m_Registery.view<ScriptComponent>();

			for (auto e : view)
			{
				Entity entity = { e, this };
				ScriptingEngine::OnDrawEntity(entity);
			}
			ScriptingEngine::OnRuntimeStop();
		}
		RenderScene3D(camera);
		RenderScene2D(camera);
#endif


	}

	void Scene::OnUpdateRuntime(TimeStep ts)
	{
		RY_PROFILE_FUNCTION();
#if ENTITY_SCRIPT
		{
#if NATIVE_SCRIPT
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
#endif
#if SCRIPT_CS
			auto view = m_Registery.view<ScriptComponent>();
			for (auto e : view)
			{
				Entity entity = { e, this };
				ScriptingEngine::OnUpdateEntity(entity, ts);
			}
		}
#endif
#endif


		Camera *mainCamera = nullptr;
		const glm::mat4 *mainTransform;

		auto view = m_Registery.view<TransformComponent, CameraComponent>();
		for (auto entity : view)
		{
			auto& [tranform, camera] = view.get<TransformComponent, CameraComponent>(entity);


			if (camera.Primary)
			{
				mainCamera = &camera.Camera;
				mainTransform = &tranform.GetTransform();
				break;
			}
		}

		if (mainCamera) 
		{

#if RENDERER_3D
			{
				////////////////////////////////////////////////////////////
				/// Renderer 3D ////////////////////////////////////////////
				////////////////////////////////////////////////////////////

				Renderer3D::BeginScene(*mainCamera, *mainTransform);
				auto group3D = m_Registery.group<TransformComponent>(entt::get<MaterialComponent, GeomtryComponent>);

				for (auto entity3D : group3D)
				{
					auto& [tranform, material, geomtry] = group3D.get<TransformComponent, MaterialComponent, GeomtryComponent>(entity3D);
					if (material.Shader == nullptr || geomtry.Geometry == nullptr)
						continue;
					Renderer3D::BeforDrawEntity(material, tranform.GetTransform());
					Renderer3D::DrawMeshStrips(geomtry.Geometry);
					//Renderer3D::AfterDrawEntity(material);
				}
				Renderer3D::EndScene();
			}
#endif	

#if RENDERER_2D
			{
				////////////////////////////////////////////////////////////
				/// Renderer 2D ////////////////////////////////////////////
				////////////////////////////////////////////////////////////

				Renderer2D::BeginScene(*mainCamera, *mainTransform);
				auto view2D = m_Registery.view<TransformComponent, SpriteRendererComponent>();
				//auto group2D = m_Registery.group<TransformComponent>(entt::get<SpriteRendererComponent>);
				//for (auto entity2D : group2D)	
				for (auto entity2D : view2D)
				{
					//auto& [tranform, sprite] = group2D.get<TransformComponent, SpriteRendererComponent>(entity2D);
					auto& [tranform, sprite] = view2D.get<TransformComponent, SpriteRendererComponent>(entity2D);

					//Renderer2D::DrawQuad(tranform.GetTransform(), sprite.Color);
					Renderer2D::DrawSprite(tranform.GetTransform(), sprite, (int)entity2D);
				}

				Renderer2D::EndScene();
		}

#endif
		}

		
	}

	void Scene::OnUpdateSimulation(TimeStep ts, EditorCamera& camera)
	{
		RY_PROFILE_FUNCTION();
		if (!m_IsPaused || m_StepFrames-- > 0)
		{
			// Physics
			{
				const int32_t velocityIterations = 6;
				const int32_t positionIterations = 2;
				//m_PhysicsWorld->Step(ts, velocityIterations, positionIterations);
#if 0
				// Retrieve transform from Box2D
				auto view = m_Registery.view<Rigidbody2DComponent>();
				for (auto e : view)
				{
					Entity entity = { e, this };
					auto& transform = entity.GetComponent<TransformComponent>();
					auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

					b2Body* body = (b2Body*)rb2d.RuntimeBody;
					const auto& position = body->GetPosition();
					transform.Translation.x = position.x;
					transform.Translation.y = position.y;
					transform.Rotation.z = body->GetAngle();
				}
#endif
			}
		}

		// Render
		RenderScene2D(camera);
		RenderScene3D(camera);
	}

	void Scene::RenderScene2D(EditorCamera& camera)
	{
		RY_PROFILE_FUNCTION();
#if RENDERER_2D
		Renderer2D::BeginScene(camera);
#if QUADS_DRAW
		{
			auto view2D = m_Registery.view<SpriteRendererComponent, TransformComponent>();
			for (auto entity : view2D)
			{
				auto& [sprite, transform] = view2D.get<SpriteRendererComponent, TransformComponent>(entity);

				Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int)entity);
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
		Renderer2D::EndScene();
#endif
	}

	void Scene::RenderScene3D(EditorCamera& camera)
	{
		RY_PROFILE_FUNCTION();
#if RENDERER_3D
		{
			////////////////////////////////////////////////////////////
			/// Renderer 3D ////////////////////////////////////////////
			////////////////////////////////////////////////////////////

			Renderer3D::BeginScene(camera);
			auto view3D = m_Registery.view<TransformComponent, MaterialComponent, GeomtryComponent>();

			for (auto entity3D : view3D)
			{
				auto& [tranform, material, geomtry] = view3D.get<TransformComponent, MaterialComponent, GeomtryComponent>(entity3D);
				Ref<VertexArray> vertexArray = geomtry.Geometry;
				if (material.Shader == nullptr || vertexArray == nullptr)
					continue;
				Renderer3D::BeforDrawEntity(material, tranform.GetTransform());
				DrawObjectRender3D(vertexArray);
				Renderer3D::AfterDrawEntity(material);

				

				{
					//Renderer3D::DrawLineLoop(vertexArray);
				}
			}
			Renderer3D::EndScene();
		}
#endif	
	}


	void Scene::OnViewportResize(uint32_t withe, uint32_t heigth)
	{
		RY_PROFILE_FUNCTION();
		m_ViewPortWithe = withe;
		m_ViewPortHeigth = heigth;

		//resize
		auto view = m_Registery.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);

			if (!cameraComponent.FixedAspectRotaion)
				cameraComponent.Camera.SetViewPortSize(withe, heigth);
			
		}
	}


	Entity Scene::GetEntitiyByUUID(UUID uuid)
	{
		RY_PROFILE_FUNCTION();
		if(m_EntityMapID.find(uuid) != m_EntityMapID.end()) 
			return { m_EntityMapID.at(uuid), this};

		return {};
	}

	bool Scene::IsTagInScene(const std::string& tag)
	{
		return (m_EntityMapTag.find(tag) != m_EntityMapTag.end());
	}

	void Scene::GetMainCameraMainTransform( Camera *mainCamera, glm::mat4* mainTransform)
	{
		RY_PROFILE_FUNCTION();
		auto view = m_Registery.view<TransformComponent, CameraComponent>();
		for (auto entity : view)
		{
			auto& [tranform, camera] = view.get<TransformComponent, CameraComponent>(entity);


			if (camera.Primary)
			{
				mainCamera = &camera.Camera;
				mainTransform = &tranform.GetTransform();
				break;
			}
		}
	}

	Entity Scene::GetPrimaryCameraEntity()
	{
		RY_PROFILE_FUNCTION();
		auto view = m_Registery.view<CameraComponent>();
		for (auto entity : view)
		{
			const auto& camera = view.get<CameraComponent>(entity);
			if (camera.Primary)
				return Entity{ entity, this };
		}
		return {};
	}

	//Default
	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component)
	{
#if 1
		static_assert(false);
#endif
	}


	template<>
	void Scene::OnComponentAdded<IDComponent>(Entity entity, IDComponent& component)
	{

	}

#if 1
	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
	{
		
	}
#endif

	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		component.Camera.SetViewPortSize(m_ViewPortWithe, m_ViewPortHeigth);
	}

#if 1
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
#endif

}