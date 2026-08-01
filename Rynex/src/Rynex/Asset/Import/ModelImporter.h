#pragma once
#include <Rynex/Asset/Base/Asset.h>
#include <Rynex/Asset/Base/AssetMetadata.h>
#include <Rynex/Renderer/Mesh/MeshSource.h>

namespace Rynex {


	class ModelImporter
	{
	public:
		
#if RY_MESH_LOADING

		static Ref<MeshSource> ImportModel(AssetHandle handle, const AssetMetadata& metadata);
		static Ref<MeshSource> LoadModel(const std::filesystem::path& path);

#else
		static Ref<Model> ImportModel(AssetHandle handle, const AssetMetadata& metadata);
		static Ref<Model> LoadModel(const std::filesystem::path& path);
#endif
#if RY_EDITOR_ASSETMANGER_THREADE ? 0 : 0
		static Ref<Model> ImportModelAsync(AssetHandle handle, const AssetMetadata& metadata);
		static Ref<Model> LoadModelAsync(const std::filesystem::path& path);
#endif

		static bool ReLoadeModel(AssetHandle handle, const std::filesystem::path& path);
	};

}

