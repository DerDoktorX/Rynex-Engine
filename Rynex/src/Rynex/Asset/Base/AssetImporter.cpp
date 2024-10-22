#include "rypch.h"
#include "AssetImporter.h"
#include "Asset.h"

#include "Rynex/Asset/Import/TextureImporter.h"
#include "Rynex/Asset/Import/FramebufferImporter.h"
#include "Rynex/Asset/Import/SceneImporter.h"
#include "Rynex/Asset/Import/ShaderImporter.h"
#include "Rynex/Asset/Import/VertexArrayImporter.h"
#include "Rynex/Asset/Import/ModelImporter.h"

namespace Rynex {

	using AssetsImportFunction = std::function <Ref<Asset>(AssetHandle, const AssetMetadata)>;

	static std::map<AssetType, AssetsImportFunction> s_AssetsImportFuncs = {
		{ AssetType::Texture2D, TextureImporter::ImportTexture },
		{ AssetType::Texture, TextureImporter::ImportTexture },
		{ AssetType::Shader, ShaderImporter::ImportShader },
		{ AssetType::Scene, SceneImporter::ImportScene },
		{ AssetType::Framebuffer, FramebufferImporter::ImportFramebuffer },
		{ AssetType::VertexArray, VertexArrayImporter::ImportVertexArray },
		{ AssetType::Model, ModelImporter::ImportModel }

	};

	using AssetsReloadingFunction = std::function <void(AssetHandle, const std::filesystem::path)>;

	static std::map<AssetType, AssetsReloadingFunction> s_AssetsReloadeFuncs = {
		{ AssetType::Texture2D, TextureImporter::ReLoadeTexture },
		{ AssetType::Texture, TextureImporter::ReLoadeTexture },
		{ AssetType::Shader, ShaderImporter::ReLoadeShader },
		{ AssetType::Scene, SceneImporter::ReLoadingScene },
		{ AssetType::Framebuffer, FramebufferImporter::ReLoadingFramebuffer },
		{ AssetType::VertexArray, VertexArrayImporter::ReLoadeVertexArray },
		{ AssetType::Model, ModelImporter::ReLoadeModel }

	};
	
	
	Ref<Asset> AssetImporter::ImportAsset(AssetHandle handle, const AssetMetadata& metadata)
	{
		return s_AssetsImportFuncs.at(metadata.Type)(handle, metadata);
	}

	void AssetImporter::ReLoadeAsset(AssetHandle handle, const AssetMetadata& metadata)
	{
		s_AssetsReloadeFuncs.at(metadata.Type)(handle, metadata.FilePath);
	}

	
}
