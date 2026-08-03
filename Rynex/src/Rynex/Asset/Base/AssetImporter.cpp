#include "rypch.h"
#include "AssetImporter.h"
#include "Asset.h"

#include <Rynex/Asset/Import/TextureImporter.h>
#include <Rynex/Asset/Import/SceneImporter.h>
#include <Rynex/Asset/Import/ShaderImporter.h>
#include <Rynex/Asset/Import/ModelImporter.h>
#include <Rynex/Asset/Import/MeshImporter.h>

namespace Rynex {

	using AssetsImportFunction = std::function <Ref<Asset>(AssetHandle, const AssetMetadata)>;

	static std::map<AssetType, AssetsImportFunction> s_AssetsImportFuncs = {
		{ AssetType::Texture2D, TextureImporter::ImportTexture },
		{ AssetType::Texture, TextureImporter::ImportTexture },
		{ AssetType::Shader, ShaderImporter::ImportShader },
		{ AssetType::Scene, SceneImporter::ImportScene },
		{ AssetType::MeshSource, ModelImporter::ImportModel },
		{ AssetType::MeshStatic, MeshImporter::ImportMesh }
	};


	using AssetsReloadingFunction = std::function <bool(AssetHandle, const std::filesystem::path)>;

	static std::map<AssetType, AssetsReloadingFunction> s_AssetsReloadeFuncs = {
		{ AssetType::Texture2D, TextureImporter::ReLoadeTexture },
		{ AssetType::Texture, TextureImporter::ReLoadeTexture },
		{ AssetType::Shader, ShaderImporter::ReLoadeShader },
		{ AssetType::Scene, SceneImporter::ReLoadingScene },
		{ AssetType::Model, ModelImporter::ReLoadeModel },
		{ AssetType::MeshSource , ModelImporter::ReLoadeModel },
		{ AssetType::MeshStatic, MeshImporter::ReLoadeMesh }
	};
	
	
	Ref<Asset> AssetImporter::ImportAsset(AssetHandle handle, const AssetMetadata& metadata)
	{		
		RY_LOG_DISABLE_NUMBER;

		return s_AssetsImportFuncs.at(metadata.Type)(handle, metadata);

		RY_LOG_ENABLE_NUMBER;

	}

	bool AssetImporter::ReLoadeAsset(AssetHandle handle, const AssetMetadata& metadata)
	{
		RY_LOG_DISABLE_NUMBER;

		bool result = s_AssetsReloadeFuncs.at(metadata.Type)(handle, metadata.AbsolutePath);
	
		RY_LOG_ENABLE_NUMBER;
		return result;
	}

	
}
