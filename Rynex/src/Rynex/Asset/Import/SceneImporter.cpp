#include "rypch.h"
#include "SceneImporter.h"

#include "Rynex/Serializers/SceneSerializer.h"
#include "Rynex/Project/Project.h"
#include "Rynex/Asset/Base/AssetManager.h"

namespace Rynex {

	Ref<Scene> SceneImporter::ImportScene(AssetHandle handle, const AssetMetadata& metadata, bool async)
	{
		return LoadScene((Project::GetActiveProjectDirectory()/metadata.FilePath).string(), async);
	}

	Ref<Scene> SceneImporter::LoadScene(const std::filesystem::path& path, bool async)
	{
		Ref<Scene> scene = CreateRef<Scene>();
		SceneSerializer serializer(scene);
		serializer.Deserialize(path.string(), async);
		return scene;
	}

	void SceneImporter::ReLoadingScene(AssetHandle handle, const std::filesystem::path& path, bool async)
	{
		SceneSerializer serializer(AssetManager::GetAsset<Scene>(handle));
		serializer.Deserialize((Project::GetActiveProjectDirectory()/path).string(), async);
	}

	


	

}
