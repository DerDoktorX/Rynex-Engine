#pragma once

#include "Rynex/Core/TimeStep.h"
#include "Rynex/Core/UUID.h"
#include "Rynex/Asset/Base/Asset.h"
#include "Rynex/Renderer/Camera/EditorCamera.h"
#include "Rynex/Asset/Base/Asset.h"

#include "entt.hpp"

namespace Rynex {
	class Entity;

	class Scene : public Asset
	{
	public:
		Scene();
		~Scene();

		static Ref<Scene> Copy(Ref<Scene> other);

		virtual AssetType GetType() const { return AssetType::Scene; }

		Entity CreateEntity(const std::string& name = std::string(""));
		Entity CreateEntityWitheUUID(UUID uuid, const std::string& name = std::string(""));

		void OnRuntimStart();
		void OnRuntimStop();

		void DestroyEntity(Entity entity);
		//entt::registry& Reg() { return m_Registery; }

		void OnUpdateEditor(TimeStep ts, EditorCamera& camera);
		void OnUpdateRuntime(TimeStep ts);
		void OnUpdateSimulation(TimeStep ts, EditorCamera& camera);

		void OnViewportResize(uint32_t withe, uint32_t heigth);

		void RenderScene2D(EditorCamera& camera);
		void RenderScene3D(EditorCamera& camera);

		Entity GetEntitiyByUUID(UUID uuid);

		bool IsTagInScene(const std::string& tag);

		void GetMainCameraMainTransform(Camera* mainCamera, glm::mat4* mainTransform);
		Entity GetPrimaryCameraEntity();
		uint32_t GetEntityCount() const { return (uint32_t)m_Registery.size(); }



		//void SetPaused(bool paused) { m_IsPaused = paused; }

		//void Step(int frames = 1);

		template<typename... Components>
		auto GetAllEntitiesWith()
		{
			return m_Registry.view<Components...>();
		}

	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
	private:
		entt::registry m_Registery;
		uint32_t m_ViewPortWithe = 1, m_ViewPortHeigth = 1;
		bool m_IsRunning = false;
		bool m_IsPaused = false;
		int m_StepFrames = 0;

		//b2World* m_PhysicsWorld = nullptr;

		std::unordered_map<UUID, entt::entity> m_EntityMapID;
		std::map<std::string, entt::entity> m_EntityMapTag;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierachyPannel;
	};

	

}


