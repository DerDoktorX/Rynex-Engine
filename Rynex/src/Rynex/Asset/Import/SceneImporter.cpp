#include "rypch.h"
#include "SceneImporter.h"

#include "Rynex/Serializers/SceneSerializer.h"
#include "Rynex/Project/Project.h"
#include "Rynex/Asset/Base/AssetManager.h"

namespace Rynex {

	Ref<Scene> SceneImporter::ImportScene(AssetHandle handle, const AssetMetadata& metadata)
	{
		std::filesystem::path filePath = metadata.AbsolutePath;
		return LoadScene(filePath);
	}

	Ref<Scene> SceneImporter::LoadScene(const std::filesystem::path& path)
	{
		Ref<Scene> scene = CreateRef<Scene>();
		SceneSerializer serializer(scene);
		serializer.Deserialize(path.string());
		return scene;
	}

	bool SceneImporter::ReLoadingScene(AssetHandle handle, const std::filesystem::path& path)
	{
		Ref<Scene> scene = AssetManager::GetAsset<Scene>(handle);
		SceneSerializer serializer(scene);
		
		return serializer.Deserialize(path);
		 
	}


}
