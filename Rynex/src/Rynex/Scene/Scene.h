#pragma once

#include "Rynex/Core/TimeStep.h"
#include "Rynex/Core/UUID.h"
#include "Rynex/Renderer/EditorCamera.h"

#include "entt.hpp"

namespace Rynex {
	class Entity;

	class Scene
	{
	public:
		Scene();
		~Scene();

		Entity CreateEntity(const std::string& name = std::string(""));
		Entity CreateEntityWitheUUID(UUID uuid, const std::string& name = std::string(""));

		void OnRuntimStart();
		void OnRuntimStop();

		void DestroyEntity(Entity entity);
		//entt::registry& Reg() { return m_Registery; }

		void OnUpdateEditor(TimeStep ts, EditorCamera& camera);
		void OnUpdateRuntime(TimeStep ts);
		void OnViewportResize(uint32_t withe, uint32_t heigth);

		Entity GetEntitiyByUUID(UUID uuid);
		Entity GetPrimaryCameraEntity();
		uint32_t GetEntityCount() const { return (uint32_t)m_Registery.size(); }

	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
	private:
		entt::registry m_Registery;
		uint32_t m_ViewPortWithe = 0, m_ViewPortHeigth = 0;

		std::unordered_map<UUID, entt::entity> m_EntityMap;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierachyPannel;
	};

	

}


