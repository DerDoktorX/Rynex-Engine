#pragma once

#include "Scene.h"
#include "Components.h"

#include <entt.hpp>
#include <Rynex/Renderer/Rendering/Render3D/Renderer3D.h>

namespace Rynex {

	class Entity
	{
	public:
		enum State
		{
			None = 0,
			Wahrning,
			Error
		};
	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene);
		Entity(const Entity& other) = default;



		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			RY_CORE_ASSERT(!HasComponent<T>(), "Entity arlead has that component!");
			Ref<Scene> scene = m_Scene.lock();
			T& component = scene->m_Registery.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
			scene->OnComponentAdded<T>(*this, component);
			return component;
		}
		
		template<typename T, typename... Args>
		T& AddOrReplaceComponent(Args&&... args)
		{
			Ref<Scene> scene = m_Scene.lock();
			T& component = scene->m_Registery.emplace_or_replace<T>( m_EntityHandle, std::forward<Args>(args)...);
			
			scene->OnComponentAdded<T>(*this, component);
			return component;
			
		}

		template<typename T>
		void LockComponent()
		{
			
		}

		template<typename T>
		void UnlockComponent()
		{

		}

		

		Entity AddChildrenEntity(const std::string& name = std::string(""));
		Entity CopyEntity();

		void DestroyEntity();
		void DestroyEntityChildrens();
		void CopyChildres(Entity e);
		
		template<typename T>
		void UpdateComponent(const T& comp)
		{
			RY_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
			Ref<Scene> scene = m_Scene.lock();
			scene->m_Registery.replace<T>(m_EntityHandle, comp);
		}

		template<typename T>
		T& GetComponent()
		{
			RY_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
			Ref<Scene> scene = m_Scene.lock();
			return scene->m_Registery.get<T>(m_EntityHandle);
		};

		template<typename T>
		const T& GetComponentC() const
		{
			RY_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
			Ref<Scene> scene = m_Scene.lock();
			return scene->m_Registery.get<T>(m_EntityHandle);
		};

		template<typename T, typename N>
		static void OnAssetLoded(Ref<N> asset, Ref<Scene> scene, int entityID);

		template<typename T>
		bool HasComponent() const
		{
			Ref<Scene> scene = m_Scene.lock();
			return scene->m_Registery.has<T>(m_EntityHandle);
		};

		template<typename T>
		void RemoveComponent();

		bool IsVisbble() const
		{
			const VisibleComponent& visbelC = GetComponentC<VisibleComponent>();
			bool visble = visbelC.isVisable;
			return visble;
		}

		void SetVisable(bool state)
		{
			VisibleComponent& visbelC = GetComponent<VisibleComponent>();
			if (visbelC.isVisable != state) 
			{
				int entityHandle = GetEntityHandle();
				RY_CORE_INFO("Entity({}) has viblity changed from {} to {}!", entityHandle, visbelC.isVisable, state);
			}
			visbelC.isVisable = state;
			
		}


		operator bool() { return IsVaild(); }
		operator entt::entity() const { return m_EntityHandle; }
		
		

		bool IsVaild() const 
		{ 
			Ref<Scene> scene = m_Scene.lock();
			bool result = entt::null != m_EntityHandle && nullptr != scene && scene->m_Registery.valid(m_EntityHandle);
			return result;
		}

		bool IsNotVaild() const
		{
			Ref<Scene> scene = m_Scene.lock();
			bool result = (entt::null == m_EntityHandle || nullptr == scene || !scene->m_Registery.valid(m_EntityHandle));
			return result;
		}

		uint32_t GetEntityIndexHandle() const
		{
			uint32_t uEnitityID = entt::to_integral(m_EntityHandle);
			return uEnitityID;
		}
		int GetEntityHandle() const 
		{ 
			uint32_t uEnitityID = GetEntityIndexHandle();
			int enitityID = static_cast<int>(uEnitityID);
			return enitityID;
		}
		entt::entity GetDefaultEntityHandle() const { return m_EntityHandle; }

		
		UUID GetParentID() const { return GetComponentC<RealtionShipUUIDComponent>().parent; }

		Entity GetParentEntity() const
		{ 
			if (UUID idParent = GetParentID())
				return GetScene()->GetEntitiyByUUID(idParent);
			return Entity();
		}
		
		std::vector<UUID>& GetChildrens() 
		{ 
			return GetComponent<RealtionShipUUIDComponent>().childrens; 
		}

		bool RemoveFromChildrens(Entity e);
		bool AddToChildrens(Entity e);

		UUID GetUUID() { return GetComponent<IDComponent>().ID; }
		std::string GetTagName() { return GetComponent<TagComponent>().Tag; }
		void UpdateMatrix();
		void UpdateAutoMatrix();
		
		void UpadteTransformFromMatrix();
		void UpdateMatrixFromTransform();
		void ExecuteOnChildrens(const std::function<void(Entity e)>& func);

		template<typename T, typename N>
		void SetLodedAsset(Ref<N> asset);

		bool operator==(const Entity& other) const
		{
			Ref<Scene> scene = m_Scene.lock();
			return m_EntityHandle == other.m_EntityHandle && scene == other.GetScene();
		}

		bool operator!=(const Entity& other) const
		{
			return !(*this == other);
		}

		State GetState() const { return m_State; }
		void SetState(State state) { m_State = state; }
		
		const Scene *const GetScenePtr() const;
		Ref<Scene> GetScene() const;

		bool IsInSceneCameraEntityViewFustrum()
		{
			Ref<Scene> scene = m_Scene.lock();
			return scene->IsCameraEntityViewFustrum();
		}
	

	private:
		Weak<Scene> m_Scene;
		entt::entity m_EntityHandle{ entt::null };
		State m_State = State::None;
	private:

	};



	template<typename T, typename N>
	void Entity::OnAssetLoded(Ref<N> asset, Ref<Scene> scene, int entityID)
	{
		entt::entity enttEntityID = entt::entity(entityID);
		Entity entity = Entity(enttEntityID, scene.get() );

		if (!entity)
			return;

		if (!entity.HasComponent<T>())
			return;

		entity.SetLodedAsset<T, N>(asset);
	}

	template<typename T>
	void Entity::RemoveComponent()
	{
		RY_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
		Ref<Scene> scene = m_Scene.lock();
		scene->m_Registery.remove<T>(m_EntityHandle);
	}


	template<typename T, typename N>
	void Entity::SetLodedAsset(Ref<N> asset)
	{
		static_cast(false, "not set types");
	}
	
	template<>
	inline void Entity::SetLodedAsset<SpriteRendererComponent, Texture>(Ref<Texture> asset)
	{
		SpriteRendererComponent& componet = GetComponent<SpriteRendererComponent>();
		componet.Texture = asset;
		UpdateComponent(componet);
	}

	

	template<>
	inline void Entity::SetLodedAsset<ModelMangerComponent, MeshStatic>(Ref<MeshStatic> asset)
	{
		ModelMangerComponent& componet = GetComponent<ModelMangerComponent>();
		componet.meshStatic = asset;
		componet.rendereStoreIndexVec2.clear();
		componet.objectRendereIndexPiplineVec2.Clear();
		componet.singleMeshes.clear();

		UpdateComponent(componet);		

	}

}


