#include "rypch.h"
#include "AssetImporter.h"
#include "Asset.h"
#include "Rynex/Asset/Import/TextureImporter.h"
#include "Rynex/Asset/Import/SceneImporter.h"
#include "Rynex/Asset/Import/ShaderImporter.h"
#include <Rynex/Project/Project.h>

#include <map>

namespace Rynex {

	using AssetsImportFunction = std::function <Ref<Asset>(AssetHandle, const AssetMetadata)>;

	static std::map<AssetType, AssetsImportFunction> s_AssetsImportFuncs = {
		{ AssetType::Texture2D, TextureImporter::ImportTexture2D },
		{ AssetType::Shader, ShaderImporter::ImportShader },
		{ AssetType::Scene, SceneImporter::ImportScene },
		{ AssetType::Scene, SceneImporter::ImportScene },
	};

	using AssetsReloadingFunction = std::function <void(AssetHandle, const std::filesystem::path)>;

	static std::map<AssetType, AssetsReloadingFunction> s_AssetsReloadingFuncs = {
		{ AssetType::Texture2D, TextureImporter::ReLoadeTexture2D },
		{ AssetType::Shader, ShaderImporter::ReLoadeShader },
		{ AssetType::Scene, SceneImporter::ReLoadingScene },
	};
	
	
	Ref<Asset> AssetImporter::ImportAsset(AssetHandle handle, const AssetMetadata& metadata)
	{
		RY_PROFILE_FUNCTION();
		return s_AssetsImportFuncs.at(metadata.Type)(handle, metadata);
	}

	void AssetImporter::ReLoadeAsset(AssetHandle handle, const AssetMetadata& metadata)
	{
		RY_PROFILE_FUNCTION();
		RY_CORE_WARN("In Dev Funktion: ReLoadeAsset!");
		s_AssetsReloadingFuncs.at(metadata.Type)(handle, metadata.FilePath);
	}
}
