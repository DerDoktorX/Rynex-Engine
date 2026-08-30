#pragma once
namespace YAML {
	class Node;
}

namespace Rynex {
	class MeshStatic;
	class MeshSource;

	class StaticMeshSerialzation
	{
	public:
		StaticMeshSerialzation(const Ref<MeshStatic>& meshSource);

		bool Serialize(const std::filesystem::path& filepath);
		bool Deserialize(const std::filesystem::path& filepath);
	private:
		bool DeserializeMeshNodes(const YAML::Node& meshNodes, Ref<MeshSource>& sourceMesh);
	private:
		Ref<MeshStatic> m_MeshStatic;
		
	};

}