#pragma once
#include <Rynex/Core/Base.h>
#include <Rynex/Core/Log.h>
#include <Rynex/Scene/Scene.h>
#include <Rynex/Scene/Entity.h>

#include <imgui/imgui.h>
#include <future>

namespace Rynex {

	struct DealeatEntitiy {
		Entity Entiy;
		bool Children;

		DealeatEntitiy(Entity entity, bool children = false)
			: Entiy(entity), Children(children) { }

		DealeatEntitiy(const DealeatEntitiy&) = default;
		DealeatEntitiy(DealeatEntitiy&&) = default;
	};

	class SceneHierachyPannel
	{
	public:
		SceneHierachyPannel() = default;
		SceneHierachyPannel(const Ref<Scene>& scene);

		void SetContext(const Ref<Scene>& scene);
		void DrawEntityNode(Entity entity, bool normale = true);
		void DrawComponents(Entity entity);

		void OpenSceneHierachy();
		void OpenProperties();
		
		void OnImGuiRender();
		Entity GetSelectedEntity() const { 
			if (m_SelectionContext)
				return m_SelectionContext;
			else
				return Entity{};
		}
		void SetSelectedEntity(Entity entity) { m_SelectionContext = entity; }

		void CreateEntity(const std::string& name, uint32_t entityNumber = 0);
		
		uint32_t GetEntityCount() const { return m_Context->GetEntityCount(); }

		void SetCheckErrors(bool checke = true) { m_CheckErrors = checke; }
	private:
		//Properties
		void DrawProperties();
		void DeleteEntity(Entity entitiy, bool children);
		void EcexuterDeleteing();

		template<typename T>
		bool DisplayAddComponentEntry(const std::string& name);
		
		void CheckEnttiyForError(Entity& entity);

	private:
		Ref<Scene> m_Context;
		std::vector<DealeatEntitiy> m_EntityDelete;
		Entity m_SelectionContext;
		std::vector<std::future<void>>	m_WorkingThread;
		ImGuiID m_ScriptEditID;
		bool m_CheckErrors = false;

		bool m_WindowSceneHierachyOpen = true;
		bool m_WindowPropertiesOpen = true;

	};
	
}


