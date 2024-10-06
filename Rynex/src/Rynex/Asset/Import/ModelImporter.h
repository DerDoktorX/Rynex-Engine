#pragma once
#include <Rynex/Asset/Base/Asset.h>
#include <Rynex/Asset/Base/AssetMetadata.h>
#include <Rynex/Renderer/Objects/Model.h>

namespace Rynex {


	class ModelImporter
	{
	public:
		static Ref<Model> ImportModel(AssetHandle handle, const AssetMetadata& metadata);
		static Ref<Model> LoadModel(const std::filesystem::path& path);
		static void ReLoadeModel(AssetHandle handle, const std::filesystem::path& path);
	};

}

