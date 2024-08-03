#pragma once
#include <Rynex/Core/Base.h>
#include <Rynex/Core/Log.h>
#include <Rynex/Scene/Scene.h>
#include <Rynex/Scene/Entity.h>

#include <imgui/imgui.h>
typedef int ImGuiTreeNodeFlags;


namespace Rynex {

	class SceneHierachyPannel
	{
	public:
		SceneHierachyPannel() = default;
		SceneHierachyPannel(const Ref<Scene>& scene);

		void SetContext(const Ref<Scene>& scene);
		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);

		
		void OnImGuiRender();
		Entity GetSelectedEntity() const { return m_SelectionContext; }
		void SetSelectedEntity(Entity entity) { m_SelectionContext = entity; }
		void CreateEntity(const std::string& name, uint32_t entityNumber = 0);

		uint32_t GetEntityCount() const { return m_Context->GetEntityCount(); }

		void SetCheckErrors(bool checke = true) { m_CheckErrors = checke; }
	private:
		//Properties
		void DrawProperties();
		
		template<typename T>
		void DisplayAddComponentEntry(const std::string& name);
		
		void CheckEnttiyForError(Entity& entity);

	private:
		Ref<Scene> m_Context;
		Entity	m_SelectionContext;

		ImGuiID m_ScriptEditID;
		bool m_CheckErrors = false;
	};
	
}


