#pragma once
#include <Rynex/Renderer/RenderProxy/RenderProxyPrototype.h>
#include <Rynex/Renderer/Objects/BoundingVolume.h>
#include <Rynex/Renderer/Mesh/MeshSingle.h>
#include <Rynex/Renderer/API/ProtypeAPI.h>
#include <Rynex/Renderer/PiplineObjects/Batching/BatchDataStructs.h>

namespace Rynex {

	

	class RenderWorld
	{
	public:
		
	public:
		RenderWorld();
		~RenderWorld();

		UUID SubmitRenderObject(const glm::mat4& model, const Ref<MeshSingle>& mesh, const Ref<Material>& material, int entity);

		void Remove(const UUID& uuid);
		void ChangeTransform(const UUID& uuid, const glm::mat4& model);
		void ChangeMesh(const UUID& uuid, const Ref<MeshSingle>& mesh);
		void ChangeMesh(const UUID& uuid, const Ref<Material>& material);

		

		void Clear();
	private:
		std::vector<RenderProxy> m_StaticeProxys;
		std::vector<RenderProxy> m_SortedProxys;
		std::unordered_map<int, uint32_t> m_EnitiyToStaticIndex;
	};

	class OuadTree2D 
	{
	private:
		struct Node
		{
			Node* parent = nullptr;
			std::array<Node*, 4> childrens;
			AABB boxVolume;
			std::vector<RenderProxy*> proxyVec;
		};
	public:

		void GenerateNode();
		void CreateNode(Node& node);
	private:
		std::vector<RenderProxy> m_Map;
		std::vector<Node> m_NodesTree;
		Node* m_RootNode;
	};
}
