#include "rypch.h"
#include "AssetImporter.h"

#include <Rynex/Asset/Base/Asset.h>
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


	using AssetsReloadingFunction = std::function<bool(AssetHandle, const FileSystem::Path&)>;

	static std::map<AssetType, AssetsReloadingFunction> s_AssetsReloadeFuncs{
		// { AssetType::Texture2D,     TextureImporter::ReloadTexture },
		// { AssetType::Texture,       TextureImporter::ReloadTexture },
		// { AssetType::Shader,        ShaderImporter::ReloadShader },
		 { AssetType::Scene,         SceneImporter::ReloadScene },
		// { AssetType::Model,         ModelImporter::ReloadModel },
		// { AssetType::MeshSource,    ModelImporter::ReloadModel },
		// { AssetType::MeshStatic,    MeshImporter::ReloadMesh }
	};
	
	
	Ref<Asset> AssetImporter::ImportAsset(const AssetHandle handle, const AssetMetadata& metadata)
	{		
		RY_LOG_DISABLE_NUMBER;

		return s_AssetsImportFuncs.at(metadata.m_Type)(handle, metadata);

		RY_LOG_ENABLE_NUMBER;

	}

	bool AssetImporter::ReloadAsset(const AssetHandle handle, const AssetMetadata& metadata)
	{
		RY_LOG_DISABLE_NUMBER;

		const bool result = s_AssetsReloadeFuncs.at(metadata.m_Type)(handle, metadata.m_Path);
	
		RY_LOG_ENABLE_NUMBER;
		return result;
	}

	
}
