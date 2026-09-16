#pragma once

#include <Rynex/Scene/Scene.h>
#include <Rynex/Scene/Components.h>
#include <Rynex/Renderer/Rendering/Render3D/Renderer3D.h>

namespace Rynex {

	class Entity
	{
	public:
		enum State
		{
			None = 0,
			Warning,
			Error
		};
	public:
		Entity();
		Entity(entt::entity handle, Scene* scene);
		Entity(const Entity& other) = default;


		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			RY_CORE_ASSERT(!HasComponent<T>(), "Entity already has that component!");
			Ref<Scene> scene = m_Scene.lock();
		    RY_CORE_ASSERT(nullptr != scene, "Scene is nullptr! (Scene could be deleted or never existed)");
			T& component = scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);

			scene->OnComponentAdded<T>(*this, component);
			return component;
		}
		
		template<typename T, typename... Args>
		T& AddOrReplaceComponent(Args&&... args)
		{
			Ref<Scene> scene = m_Scene.lock();
			T& component = scene->m_Registry.emplace_or_replace<T>( m_EntityHandle, std::forward<Args>(args)...);
			
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
			scene->m_Registry.replace<T>(m_EntityHandle, comp);
		}

		template<typename T>
		T& GetComponent()
		{
			RY_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
			Ref<Scene> scene = m_Scene.lock();
			return scene->m_Registry.get<T>(m_EntityHandle);
		};

		template<typename T>
		const T& GetComponentC() const
		{
			RY_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
			Ref<Scene> scene = m_Scene.lock();
			return scene->m_Registry.get<T>(m_EntityHandle);
		};

		template<typename T, typename N>
		static void OnAssetLoded(Ref<N> asset, Ref<Scene> scene, int entityID);

		template<typename T>
		bool HasComponent() const
		{
			Ref<Scene> scene = m_Scene.lock();
			return scene->m_Registry.has<T>(m_EntityHandle);
		};

		template<typename T>
		void RemoveComponent();

		bool IsVisbble() const
		{
			const VisibleComponent& visbelC = GetComponentC<VisibleComponent>();
			bool visble = visbelC.m_Visible;
			return visble;
		}

		void SetVisable(bool state)
		{
			VisibleComponent& visbelC = GetComponent<VisibleComponent>();
			if (visbelC.m_Visible != state) 
			{
				int entityHandle = GetEntityHandle();
				RY_CORE_INFO("Entity({}) has viblity changed from {} to {}!", entityHandle, visbelC.m_Visible, state);
			}
			visbelC.m_Visible = state;
			
		}


		operator bool() { return IsVaild(); }
		operator entt::entity() const { return m_EntityHandle; }
		
		

		bool IsVaild() const 
		{ 
			Ref<Scene> scene = m_Scene.lock();
			bool result = entt::null != m_EntityHandle && nullptr != scene && scene->m_Registry.valid(m_EntityHandle);
			return result;
		}

		bool IsNotVaild() const
		{
			Ref<Scene> scene = m_Scene.lock();
			bool result = (entt::null == m_EntityHandle || nullptr == scene || !scene->m_Registry.valid(m_EntityHandle));
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

		
		UUID GetParentID() const { return GetComponentC<RelationshipUUIDComponent>().m_Parent; }

		Entity GetParentEntity() const
		{ 
			if (UUID idParent = GetParentID())
				return GetScene()->GetEntitiyByUUID(idParent);
			return Entity();
		}
		
		std::vector<UUID>& GetChildrens() 
		{ 
			return GetComponent<RelationshipUUIDComponent>().m_Childrens; 
		}

		bool RemoveFromChildrens(Entity e);
		bool AddToChildrens(Entity e);

		UUID GetUUID() { return GetComponent<IDComponent>().m_ID; }
		std::string GetTagName() { return GetComponent<TagComponent>().m_Tag; }
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
		entt::entity m_EntityHandle;
		State m_State;
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
		scene->m_Registry.remove<T>(m_EntityHandle);
	}


	template<typename T, typename N>
	void Entity::SetLodedAsset(Ref<N> asset)
	{
		static_assert(false, "not set types");
	}
	
	template<>
	inline void Entity::SetLodedAsset<SpriteRendererComponent, Texture>(Ref<Texture> asset)
	{
		SpriteRendererComponent& componet = GetComponent<SpriteRendererComponent>();
		componet.m_Texture = asset;
		UpdateComponent(componet);
	}

	

	template<>
	inline void Entity::SetLodedAsset<ModelMangerComponent, MeshStatic>(Ref<MeshStatic> asset)
	{
		ModelMangerComponent& componet = GetComponent<ModelMangerComponent>();
		componet.m_MeshStatic = asset;
		componet.m_RenderStoreIndexVec2.clear();
		componet.m_ObjectRenderIndexPiplineVec2.Clear();
		componet.m_SingleMeshes.clear();

		UpdateComponent(componet);		

	}

}


