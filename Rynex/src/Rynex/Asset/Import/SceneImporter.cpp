#include "rypch.h"
#include "SceneImporter.h"

#include <Rynex/Serializers/SceneSerializer.h>
#include <Rynex/Project/Project.h>
#include <Rynex/Asset/Base/AssetManager.h>

namespace Rynex {

	Ref<Scene> SceneImporter::ImportScene(AssetHandle handle, const AssetMetadata& metadata)
	{
		FileSystem::Path filePath = metadata.m_Path;
		return LoadScene(filePath);
	}

	Ref<Scene> SceneImporter::LoadScene(const FileSystem::Path& path)
	{
		Ref<Scene> scene = CreateRef<Scene>();
		SceneSerializer serializer(scene);
		serializer.Deserialize(path);
		return scene;
	}

	bool SceneImporter::ReloadScene(AssetHandle handle, const FileSystem::Path& path)
	{
		Ref<Scene> scene = AssetManager::GetAsset<Scene>(handle);
		SceneSerializer serializer(scene);
		
		return serializer.Deserialize(path);
		 
	}


}
