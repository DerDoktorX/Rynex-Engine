#include "rypch.h"
#include "FramebufferImporter.h"

#include "Rynex/Asset/Base/AssetManager.h"
#include "Rynex/Serializers/FrambufferSerializer.h"

namespace Rynex {

	Ref<Framebuffer> FramebufferImporter::ImportFramebuffer(AssetHandle handle, const AssetMetadata& metadata)
	{
		return LoadFramebuffer(metadata.FilePath.string());
	}

	Ref<Framebuffer> FramebufferImporter::LoadFramebuffer(const std::filesystem::path& path)
	{
		Ref<Framebuffer> frambuffer = FrambufferSerializer::Deserilze(path.string());
		Ref<EditorAssetManager> editorAssetManager = Project::GetActive()->GetEditorAssetManger();
		std::string pathForTex = path.string() + "-";
		int index = 0;
		for (auto& texture : frambuffer->GetTextures())
		{ 
			std::string pathWitheExtension = pathForTex + std::to_string(index);
			if(!editorAssetManager->IsAssetLoaded(pathWitheExtension))
			{
				AssetMetadata metadeta;
				metadeta.Type = texture->GetType();
				editorAssetManager->CreateAsset(pathWitheExtension, (Ref<Asset>)texture, metadeta);
			}
			
			texture->Handle = editorAssetManager->GetAssetHandle(pathWitheExtension);
			Ref<Texture> assetTexture = AssetManager::GetAsset<Texture>(texture->Handle);
			RY_CORE_ASSERT(assetTexture == texture, "ERORO: FramebufferImporter::LoadFramebuffer, we get two diverentTextures they schoud by EQUEL!");
			index++;
		}
		
		return frambuffer;
	}

	void FramebufferImporter::ReLoadingFramebuffer(AssetHandle handle, const std::filesystem::path& path)
	{
		// Ref<Framebuffer> frambuffer = AssetManager::GetAsset<Framebuffer>(handle);
	}

	
}
