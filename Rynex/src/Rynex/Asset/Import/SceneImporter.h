#pragma once
#include <Rynex/Scene/Scene.h>
#include <Rynex/Asset/Base/AssetMetadata.h>


namespace Rynex {

	class SceneImporter
	{
	public:
		static Ref<Scene> ImportScene(AssetHandle handle, const AssetMetadata& metadata);
		static Ref<Scene> LoadScene(const FileSystem::Path& path);
		static bool ReloadScene(AssetHandle handle, const FileSystem::Path& path);
	};

}

