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

	using AssetsImportFunction = std::function <Ref<Asset>(AssetHandle, const AssetMetadata, bool)>;

	static std::map<AssetType, AssetsImportFunction> s_AssetsImportFuncs = {
		{ AssetType::Texture2D, TextureImporter::ImportTexture },
		{ AssetType::Texture, TextureImporter::ImportTexture },
		{ AssetType::Shader, ShaderImporter::ImportShader },
		{ AssetType::Scene, SceneImporter::ImportScene },
		{ AssetType::Framebuffer, FramebufferImporter::ImportFramebuffer },
		{ AssetType::VertexArray, VertexArrayImporter::ImportVertexArray },
		{ AssetType::Model, ModelImporter::ImportModel }

	};

#if RY_EDITOR_ASSETMANGER_THREADE  ? 0:0
	static std::map<AssetType, AssetsImportFunction> s_AssetsAsyncImportFuncs = {
		{ AssetType::Texture2D, TextureImporter::ImportTexture },
		{ AssetType::Texture, TextureImporter::ImportTexture },
		// { AssetType::Shader, ShaderImporter::ImportShader },
		// { AssetType::Scene, SceneImporter::ImportScene },
		{ AssetType::Framebuffer, FramebufferImporter::ImportFramebuffer },
		// { AssetType::VertexArray, VertexArrayImporter::ImportVertexArray },
		{ AssetType::Model, ModelImporter::ImportModelAsync }

	};
#endif

	using AssetsReloadingFunction = std::function <void(AssetHandle, const std::filesystem::path, bool)>;

	static std::map<AssetType, AssetsReloadingFunction> s_AssetsReloadeFuncs = {
		{ AssetType::Texture2D, TextureImporter::ReLoadeTexture },
		{ AssetType::Texture, TextureImporter::ReLoadeTexture },
		{ AssetType::Shader, ShaderImporter::ReLoadeShader },
		{ AssetType::Scene, SceneImporter::ReLoadingScene },
		{ AssetType::Framebuffer, FramebufferImporter::ReLoadingFramebuffer },
		{ AssetType::VertexArray, VertexArrayImporter::ReLoadeVertexArray },
		{ AssetType::Model, ModelImporter::ReLoadeModel }

	};
	
	
	Ref<Asset> AssetImporter::ImportAsset(AssetHandle handle, const AssetMetadata& metadata, bool async)
	{		
		return s_AssetsImportFuncs.at(metadata.Type)(handle, metadata, async);
	}
#if RY_EDITOR_ASSETMANGER_THREADE ? 0:0
	Ref<Asset> AssetImporter::ImportAssetAsync(AssetHandle handle, const AssetMetadata& metadata)
	{
		return s_AssetsAsyncImportFuncs.at(metadata.Type)(handle, metadata);
	}
#endif
	void AssetImporter::ReLoadeAsset(AssetHandle handle, const AssetMetadata& metadata, bool async)
	{
		s_AssetsReloadeFuncs.at(metadata.Type)(handle, metadata.FilePath.generic_string(), async);
	}

	
}
