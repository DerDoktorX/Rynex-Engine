#pragma once
#include <Rynex/Asset/Base/Asset.h>
#include <Rynex/Asset/Base/AssetMetadata.h>
#include <Rynex/Renderer/Objects/Model.h>

namespace Rynex {
	struct MeshIdentifeier
	{
		uint32_t Index;
		std::string Name;
	};

	class ModelImporter
	{
	public:
		static Ref<Model> ImportModel(AssetHandle handle, const AssetMetadata& metadata, bool async);
		static Ref<Model> LoadModel(const std::filesystem::path& path, bool async);
		static Ref<Mesh> LoadMeshFormModel(const std::filesystem::path& path, const MeshIdentifeier& meshID, bool async);
		static std::vector<Ref<Model>> LoadMeshesFormModel(const std::filesystem::path& path, const std::vector<MeshIdentifeier>& meshID, bool async);
#if RY_EDITOR_ASSETMANGER_THREADE ? 0 : 0
		static Ref<Model> ImportModelAsync(AssetHandle handle, const AssetMetadata& metadata);
		static Ref<Model> LoadModelAsync(const std::filesystem::path& path);
#endif

		static void ReLoadeModel(AssetHandle handle, const std::filesystem::path& path, bool async);
	};

}

