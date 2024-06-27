#pragma once
#include "Rynex/Core/Base.h"
#include "Rynex/Core/Log.h"
#include "Rynex/Scene/Scene.h"
#include "Rynex/Scene/Entity.h"

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

		uint32_t GetEntityCount() const { return m_Context->GetEntityCount(); }
	private:
		//Properties
		void DrawProperties();
	private:
		Ref<Scene> m_Context;
		Entity	m_SelectionContext;
		
	};
}


