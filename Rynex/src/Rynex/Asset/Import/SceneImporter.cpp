#include "rypch.h"
#include "SceneImporter.h"

#include "Rynex/Serializers/SceneSerializer.h"
#include "Rynex/Project/Project.h"
#include "Rynex/Asset/Base/AssetManager.h"

namespace Rynex {

	Ref<Scene> SceneImporter::ImportScene(AssetHandle handle, const AssetMetadata& metadata)
	{
		RY_PROFILE_FUNCTION();
		return LoadScene(metadata.FilePath.string());
	}

	Ref<Scene> SceneImporter::LoadScene(const std::filesystem::path& path)
	{
		RY_PROFILE_FUNCTION();
		Ref<Scene> scene = CreateRef<Scene>();
		SceneSerializer serializer(scene);
		serializer.Deserialize(path.string());
		return scene;
	}

	void SceneImporter::ReLoadingScene(AssetHandle handle, const std::filesystem::path& path)
	{
	}


	

}
