#include "rypch.h"
#include "MeshImporter.h"

#include <Rynex/Serializers/StaticMeshSerializer.h>
#include <Rynex/Project/Project.h>
#include <Rynex/Asset/Base/AssetManager.h>

namespace Rynex {

	Ref<MeshStatic> MeshImporter::ImportMesh(AssetHandle handle, const AssetMetadata& metadata)
	{
		return LoadMesh(metadata.m_Path);
	}

	Ref<MeshStatic> MeshImporter::LoadMesh(const FileSystem::Path& path)
	{
		Ref<MeshStatic> meshStatic = CreateRef<MeshStatic>();
		StaticMeshSerializer serializer(meshStatic);
		serializer.Deserialize(path);
		return meshStatic;
	}

	bool MeshImporter::ReloadMesh(AssetHandle handle, const FileSystem::Path& path)
	{
		Ref<MeshStatic> meshStatic = AssetManager::GetAsset<MeshStatic>(handle);
		StaticMeshSerializer serializer(meshStatic);
		return serializer.Deserialize(path);
	}
}