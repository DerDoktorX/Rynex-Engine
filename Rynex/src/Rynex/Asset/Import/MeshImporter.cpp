#include "rypch.h"
#include "MeshImporter.h"

#include "Rynex/Serializers/StaticMeshSerialzation.h"
#include "Rynex/Project/Project.h"
#include "Rynex/Asset/Base/AssetManager.h"

namespace Rynex {

	Ref<MeshStatic> MeshImporter::ImportMesh(AssetHandle handle, const AssetMetadata& metadata)
	{
		return LoadMesh(metadata.AbsolutePath);
	}

	Ref<MeshStatic> MeshImporter::LoadMesh(const std::filesystem::path& path)
	{
		Ref<MeshStatic> meshStatic = CreateRef<MeshStatic>();
		StaticMeshSerialzation serializer(meshStatic);
		serializer.Deserialize(path);
		return meshStatic;
	}

	bool MeshImporter::ReLoadeMesh(AssetHandle handle, const std::filesystem::path& path)
	{
		Ref<MeshStatic> meshStatic = AssetManager::GetAsset<MeshStatic>(handle);
		StaticMeshSerialzation serializer(meshStatic);
		return serializer.Deserialize(path);
	}
}