#include "rypch.h"
#include "Scene.h"

#include "Components.h"
#include "ScriptableEntity.h"

#include "Rynex/Renderer/Renderer2D.h"
#include "Entity.h"


namespace Rynex {
	
	static void DoMath(const glm::mat4& transform)
	{

	};

	static void OnTrasformConstruct(entt::registry& registery, entt::entity entity)
	{

	};

	Scene::Scene()
	{
		


	}

	Scene::~Scene()
	{

	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		return CreateEntityWitheUUID(UUID(), name);
	}

	Entity Scene::CreateEntityWitheUUID(UUID uuid, const std::string& name)
	{
		Entity entity = { m_Registery.create(), this };
		
		entity.AddComponent<IDComponent>(uuid);
		entity.AddComponent<TransformComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;
		
		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registery.destroy(entity);
	}

	void Scene::OnUpdateEditor(TimeStep ts, EditorCamera& camera)
	{

		Renderer2D::BeginScene(camera);

		auto group = m_Registery.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (auto entity : group)
		{
			auto& [tranform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

			//Renderer2D::DrawQuad(tranform.GetTransform(), sprite.Color);
			Renderer2D::DrawSprite(tranform.GetTransform(), sprite, (int)entity);
		}
		Renderer2D::EndScene();
		


	}

	void Scene::OnUpdateRuntime(TimeStep ts)
	{
		
		{
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
		}
		


		Camera* mainCamera = nullptr;
		glm::mat4* mainTransform;
		{

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
		if (mainCamera) 
		{
			Renderer2D::BeginScene(mainCamera->GetProjektion(), *mainTransform);

			auto group = m_Registery.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : group)
			{
				auto& [tranform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

				//Renderer2D::DrawQuad(tranform.GetTransform(), sprite.Color);
				Renderer2D::DrawSprite(tranform.GetTransform(), sprite, (int)entity);
			}

			Renderer2D::EndScene();
		}

		
	}

	void Scene::OnViewportResize(uint32_t withe, uint32_t heigth)
	{
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
	void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<NativeSripteComponent>(Entity entity, NativeSripteComponent& component)
	{

	}
#endif

}