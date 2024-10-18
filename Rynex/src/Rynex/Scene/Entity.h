#pragma once

#include "Scene.h"
#include "Components.h"

#include <entt.hpp>

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
			T& component = m_Scene->m_Registery.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
			m_Scene->OnComponentAdded<T>(*this, component);
			return component;
		}

		Entity AddChildrenEntity(const std::string& name = std::string(""));

		void DestroyEntity();
		void DestroyEntityChildrens();

		template<typename T>
		T& GetComponent()
		{
			RY_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
			return m_Scene->m_Registery.get<T>(m_EntityHandle);
		};

		template<typename T>
		bool HasComponent()
		{
			return m_Scene->m_Registery.has<T>(m_EntityHandle);
		};

		template<typename T>
		void RemoveComponent()
		{
			RY_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
			m_Scene->m_Registery.remove<T>(m_EntityHandle);
		};

		operator bool() { return m_EntityHandle != entt::null && m_Scene->m_Registery.valid(m_EntityHandle); };
		operator entt::entity() const { return m_EntityHandle; }
		operator uint32_t() const { return (uint32_t)m_EntityHandle; }

		UUID GetUUID() { return GetComponent<IDComponent>().ID; }
		int GetEntityHandle() const { return (int)m_EntityHandle; }

		bool operator==(const Entity& other) const
		{
			return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;
		}

		bool operator!=(const Entity& other) const
		{
			return !(*this == other);
		}

		State GetState() const { return m_State; }
		void SetState(State state) { m_State = state; }
		
	
	private:
		entt::entity m_EntityHandle{ entt::null };
		Scene* m_Scene = nullptr;
		State m_State = State::None;
	};

}


