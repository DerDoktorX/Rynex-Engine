#include "rypch.h"
#include "FramebufferImporter.h"

#include "Rynex/Asset/Base/AssetManager.h"
#include "Rynex/Serializers/FrambufferSerializer.h"

namespace Rynex {

	Ref<Framebuffer> FramebufferImporter::ImportFramebuffer(AssetHandle handle, const AssetMetadata& metadata, bool async)
	{
		return LoadFramebuffer((Project::GetActiveProjectDirectory() / metadata.FilePath).string(), async);
	}

	Ref<Framebuffer> FramebufferImporter::LoadFramebuffer(const std::filesystem::path& path, bool async)
	{
		Ref<Framebuffer> frambuffer = FrambufferSerializer::Deserilze(path.string());
#if RY_EDITOR_ASSETMANGER_THREADE
		Ref<EditorAssetManegerThreade> editorAssetManager = Project::GetActive()->GetEditorAssetManger();
#else
		Ref<EditorAssetManager> editorAssetManager = Project::GetActive()->GetEditorAssetManger();
#endif
		std::string pathForTex = path.string() + "-";
		int index = 0;
		for (const auto& texture : frambuffer->GetAttachmentsTextures())
		{ 
			std::string pathWitheExtension = pathForTex + std::to_string(index);
			if(!editorAssetManager->IsAssetLoaded(pathWitheExtension))
			{
				AssetMetadata metadeta;
				metadeta.Type = texture->GetType();
#if RY_EDITOR_ASSETMANGER_THREADE
#else
				editorAssetManager->CreateAsset(pathWitheExtension, (Ref<Asset>)texture, metadeta);
#endif
			}
			
			texture->Handle = editorAssetManager->GetAssetHandle(pathWitheExtension);
			Ref<Texture> assetTexture = AssetManager::GetAsset<Texture>(texture->Handle);
			RY_CORE_ASSERT(assetTexture == texture, "ERORO: FramebufferImporter::LoadFramebuffer, we get two diverentTextures they schoud by EQUEL!");
			index++;
		}
		
		return frambuffer;
	}

	void FramebufferImporter::ReLoadingFramebuffer(AssetHandle handle, const std::filesystem::path& path, bool async)
	{
		// Ref<Framebuffer> frambuffer = AssetManager::GetAsset<Framebuffer>(handle);
	}

	
}
