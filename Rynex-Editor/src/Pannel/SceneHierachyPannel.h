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

		void OnDetache();
		void SetContext(const Ref<Scene>& scene);
		uint32_t DrawEntityNode(Entity entity, uint32_t hirachiyIndex, bool normale = true);
		void DrawComponents(Entity entity);

		void OpenSceneHierachy();
		void OpenProperties();
		
		void OnImGuiRender();
		Entity GetSelectedEntity() 
		{ 
			if (m_SelectionContext.GetScenePtr() != m_Context.get())
				return Entity{};
			else
				return m_SelectionContext;
		}
		void SetSelectedEntity(Entity entity) 
		{ 
			m_SelectionContext = entity; 
			if (m_SelectionContext.GetScenePtr() != m_Context.get())
				m_SelectionContext = Entity{};
			m_Context->SetSelectedEntity(m_SelectionContext);
		}

		void CreateEntity(const std::string& name = "Empty Entity", uint32_t entityNumber = 0);
		
		uint32_t GetEntityCount() const { return m_Context->GetEntityCount(); }

		void SetCheckErrors(bool checke = true) { m_CheckErrors = checke; }

		void TestSubmitStaticProxyLocal();
		
	private:
		//Properties
		// using EntityFunc = std::_Binder<std::_Unforced, void (SceneHierachyPannel::*)(Entity e), Entity&>;
		using EntityFunc = std::function<void()>;
		void DrawProperties();
		void DeleteEntity(Entity entitiy, bool children);
		void EcexuterDeleteing();

		template<typename T>
		bool DisplayAddComponentEntry(const std::string& name);
		
		void CheckEnttiyForError(Entity& entity);

		void AddChiledEntity(Entity e);

		void RemoveEntity(Entity e);
		void RemoveChiledEntity(Entity e);

		void CopyEntity(Entity e);
		void SwapEntitys(Entity a, Entity b);
		uint32_t FindEnitityIndex(Entity e);

		static void ComponentTransformGUI(Entity e, TransformComponent& component);
		static void ComponentModelMatrixGUI(Entity e, ModelMatrixComponent& component);
		static void ComponentCameraGUI(Entity e, CameraComponent& component);
		static void ComponentSpriteRendererGUI(Entity e, SpriteRendererComponent& component);
		static void ComponentScriptGUI(Entity e, ScriptComponent& component);
		static void ComponentFrameBufferGUI(Entity e, FrameBufferComponent& component);
		static void ComponentDrirektionleLigthGUI(Entity e, DrirectionleLigthComponent& component);
		static void ComponentPointLigthGUI(Entity e, PointLigthComponent& component);
		static void ComponentSpotLigthGUI(Entity e, SpotLigthComponent& component);
		static void ComponentTextGUI(Entity e, TextComponent& component);
		static void ComponentViewMatrixGUI(Entity e, ViewMatrixComponent& component);
		static void ComponentStaticMeshGUI(Entity e, ModelMangerComponent& component);
		static void ComponentDynamicMeshGUI(Entity e, DynamicMeshComponent& component);
		static void ComponentStaticSingleMeshGUI(Entity e, StaticMeshComponent& component);
		static void ComponentRenderTargetGUI(Entity e, RenderTargetComponent& component);
	private:
		
		std::vector<Entity> m_SceneList;
		std::vector<DealeatEntitiy> m_EntityDelete;
		std::vector<EntityFunc> m_ExecuteFunc;

		Ref<Scene> m_Context;
		Entity m_SelectionContext;
		Entity m_SelectionContextEndeMarker;

		ImGuiID m_ScriptEditID;
		bool m_CheckErrors = false;

		bool m_WindowSceneHierachyOpen = true;
		bool m_WindowPropertiesOpen = true;
		bool m_MeshConfig = false;
	private:
		inline static const char* s_SceneHierachieDragAndDropRelationShipMoveChar = "SCENE_ENTITY_MOVE_RELATIOSHIP";
	};
	
}


