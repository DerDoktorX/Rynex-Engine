#pragma once
#include "Rynex/Scene/Scene.h"
#include "Rynex/Asset/Base/AssetMetadata.h"


namespace Rynex {

	class SceneImporter
	{
	public:
		static Ref<Scene> ImportScene(AssetHandle handle, const AssetMetadata& metadata);
		static Ref<Scene> LoadScene(const std::filesystem::path& path);
		static void ReLoadingScene(AssetHandle handle, const std::filesystem::path& path);
	};

}

