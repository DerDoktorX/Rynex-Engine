#pragma once
namespace YAML {
	class Node;
}

namespace Rynex {
	class MeshStatic;
	class MeshSource;

	class StaticMeshSerializer
	{
	public:
		StaticMeshSerializer(const Ref<MeshStatic>& meshSource);

		bool Serialize(const FileSystem::Path& filepath);
		bool Deserialize(const FileSystem::Path& filepath);
	private:
		bool DeserializeMeshNodes(const YAML::Node& meshNodes, Ref<MeshSource>& sourceMesh);
	private:
		Ref<MeshStatic> m_MeshStatic;
		
	};

}