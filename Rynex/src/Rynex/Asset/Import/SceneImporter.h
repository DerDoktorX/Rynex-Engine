#pragma once
#include "Rynex/Scene/Scene.h"
#include "Rynex/Asset/Base/AssetMetadata.h"


namespace Rynex {

	class SceneImporter
	{
	public:
		static Ref<Scene> ImportScene(AssetHandle handle, const AssetMetadata& metadata, bool async);
		static Ref<Scene> LoadScene(const std::filesystem::path& path, bool async);
		static void ReLoadingScene(AssetHandle handle, const std::filesystem::path& path, bool async);
	};

}

