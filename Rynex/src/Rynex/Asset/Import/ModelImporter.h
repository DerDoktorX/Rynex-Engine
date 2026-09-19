#pragma once
#include <Rynex/Asset/Base/Asset.h>
#include <Rynex/Asset/Base/AssetMetadata.h>
#include <Rynex/Renderer/Mesh/MeshSource.h>

namespace Rynex {


	class ModelImporter
	{
	public:
		static Ref<MeshSource> ImportModel(AssetHandle handle, const AssetMetadata& metadata);
		static Ref<MeshSource> LoadModel(const FileSystem::Path& path);

		static bool ReloadModel(AssetHandle handle, const FileSystem::Path& path);
	};

}

