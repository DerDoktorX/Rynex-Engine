#include "rypch.h"
#include "SceneImporter.h"

#include "Rynex/Serializers/SceneSerializer.h"
#include "Rynex/Project/Project.h"
#include "Rynex/Asset/Base/AssetManager.h"

namespace Rynex {

	Ref<Scene> SceneImporter::ImportScene(AssetHandle handle, const AssetMetadata& metadata)
	{

		return LoadScene(metadata.FilePath.string());
	}

	Ref<Scene> SceneImporter::LoadScene(const std::filesystem::path& path)
	{
		Ref<Scene> scene = CreateRef<Scene>();
		SceneSerializer serializer(scene);
		serializer.Deserialize(path.string());
		return scene;
	}

	void SceneImporter::ReLoadingScene(AssetHandle handle, const std::filesystem::path& path)
	{
		SceneSerializer serializer(AssetManager::GetAsset<Scene>(handle));
		serializer.Deserialize(path.string());
	}

	


	

}
