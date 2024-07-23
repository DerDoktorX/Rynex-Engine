#include "rypch.h"
#include "AssetImporter.h"
#include "Asset.h"
#include "Rynex/Asset/Import/TextureImporter.h"
#include "Rynex/Asset/Import/SceneImporter.h"
#include "Rynex/Asset/Import/ShaderImporter.h"

#include <map>

namespace Rynex {

	using AssetsImportFunction = std::function <Ref<Asset>(AssetHandle, const AssetMetadata)>;

	static std::map<AssetType, AssetsImportFunction> s_AssetsImportFuncs = {
		{ AssetType::Texture2D, TextureImporter::ImportTexture2D },
		{ AssetType::Shader, ShaderImporter::ImportShader },
		{ AssetType::Scene, SceneImporter::ImportScene },
	};
	
	Ref<Asset> AssetImporter::ImportAsset(AssetHandle handle, const AssetMetadata& metadata)
	{
		RY_PROFILE_FUNCTION();
		return s_AssetsImportFuncs.at(metadata.Type)(handle, metadata);
	}
}
