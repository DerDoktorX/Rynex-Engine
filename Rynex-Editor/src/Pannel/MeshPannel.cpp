#include <rypch.h>
#include "MeshPannel.h"

#include <imgui/imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>

#include <Rynex/Core/Input.h>
#include <Rynex/Renderer/Mesh/MeshStatic.h>
#include <Rynex/Asset/Base/AssetManager.h>

namespace Rynex {

	MeshPannel::MeshPannel()
		: m_WindowOpen(true)
		, m_SelectionNode()
		, m_EditorLayer(nullptr)
	{
	}
	MeshPannel::~MeshPannel()
	{
		RY_CORE_ASSERT(nullptr == m_EditorLayer)
	}

	void MeshPannel::OnAttache(EditorLayer* editorLayer)
	{
		m_EditorLayer = editorLayer;

#if 0
		m_MeshObject = AssetManager::GetAsset<MeshStatic>("Assets/Models/main_sponza/main_sponza/NewSponza_Main_glTF_003.rystmesh");
		Ref<MeshSource>meshSource = m_MeshObject->GetMeshSource();
		m_HirachyNode = meshSource->GetNodes();
#endif
	}

	void MeshPannel::OnDetache()
	{
		m_EditorLayer = nullptr;
		m_WindowOpen = false;
		RY_DESTROY_REF(m_MeshObject);
	}

	void MeshPannel::OnUpdate()
	{ 
	}

	void MeshPannel::OpenWindow()
	{
		m_WindowOpen = true;
	}

	void MeshPannel::OnImGuiRender()
	{
		if (!m_WindowOpen)
			return;
		ImGui::Begin("Mesh", &m_WindowOpen);
		uint32_t id = 0;
		OnImGuiNode(m_HirachyNode, id);
		ImGui::End();
	}

	void MeshPannel::OnImGuiNode(MeshSource::EntityNodes& nodes, uint32_t id)
	{
		ImGuiTreeNodeFlags flags = (IsNodeSelected(nodes) ? ImGuiTreeNodeFlags_Selected : 0);
		flags |= ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		std::vector<MeshSource::EntityNodes>& childrens = nodes.Childrens;
		if(childrens.empty())
			flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth;

		std::string& name = nodes.NodeName;
		std::string idName = (name + std::to_string(id));
		ImGui::PushID(idName.c_str());

		bool opende = ImGui::TreeNodeEx(&id, flags, name.c_str());
		if (ImGui::IsItemClicked() && Input::IsKeyPressed(Key::M))
		{
			const Ref<MeshSource> source = m_MeshObject->GetMeshSource();
			const std::vector<Ref<MeshSingle>>& meshSingleVec = source->GetMeshSingleVecConst();
			
			for (uint32_t meshIndex : nodes.ObjectMeshIndexVec)
			{
				m_MeshSingleSelcted = meshSingleVec.at(meshIndex);
				m_SelectionNode = MeshSource::EntityNodes{};
			}
		}
		else if (ImGui::IsItemClicked())
		{
			m_SelectionNode = nodes;
			m_MeshSingleSelcted = nullptr;
		}

		if(opende)
		{
			for (MeshSource::EntityNodes& nodes : childrens)
			{
				uint32_t idchild = id + 1;
				OnImGuiNode(nodes, idchild);
			}
			if (!childrens.empty())
				ImGui::TreePop();
		}

		ImGui::PopID();
	}

	bool MeshPannel::IsNodeSelected(const MeshSource::EntityNodes& node) const
	{
		return node == m_SelectionNode || CheckMeshes(m_MeshSingleSelcted, node.ObjectMeshIndexVec);
	}

	bool MeshPannel::CheckMeshes(const Ref<MeshSingle>& meshSingle, const std::vector<uint32_t>& meshLocalIndexVec)
	{
		if (nullptr == meshSingle)
			return false;

		uint32_t localeIndex = meshSingle->GetModelLocalMesheIndex();
		for (const uint32_t& index : meshLocalIndexVec)
		{
			if (localeIndex == index)
				return true;
		}

		return false;
	}
	
}
