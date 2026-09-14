#pragma once
#include <Rynex/Renderer/Mesh/MeshSource.h>

namespace Rynex {
	class EditorLayer;

	class MeshPannel
	{
	public:
		MeshPannel();
		~MeshPannel();

		void OnAttache(EditorLayer* editorLayer);
		void OnDetache();

		void SetMeshContent(Ref<MeshStatic> meshStatic) { m_MeshObject = meshStatic; }
		void SetMeshContent(Ref<MeshDynamic> meshDynamic);

	
		void OpenWindow();

		void OnUpdate();
		void OnImGuiRender();

		void SaveMesh();
	private:
		void OnImGuiScenHirachy(Ref<MeshSource> meshSource);
		void OnImGuiNode(MeshSource::EntityNodes& nodes, uint32_t id);
		bool IsNodeSelected(const MeshSource::EntityNodes& node) const;
		static bool CheckMeshes(const Ref<MeshSingle>& meshSingle, const std::vector<uint32_t>& meshLocalIndexVec);

	private:
		EditorLayer* m_EditorLayer = nullptr;
		Ref<MeshStatic> m_MeshObject;
		bool m_WindowOpen;
		MeshSource::EntityNodes m_HirachyNode;

		MeshSource::EntityNodes m_SelectionNode;
		Ref<MeshSingle> m_MeshSingleSelcted;
		Ref<Material> m_MatierleSelcted;
	};

}
