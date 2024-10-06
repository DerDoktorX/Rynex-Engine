#include "rypch.h"
#include "VertexArrayImporter.h"

#include "Rynex/Asset/Base/AssetManager.h"
#include "Rynex/Serializers/VertexArraySerialzer.h"

namespace Rynex {


	Ref<VertexArray> VertexArrayImporter::ImportVertexArray(AssetHandle handle, const AssetMetadata& metadata)
	{
		return LoadVertexArry(metadata.FilePath.string());
	}

	Ref<VertexArray> VertexArrayImporter::LoadVertexArry(const std::filesystem::path& path)
	{
		Ref<VertexArray> vertexArray = VertexArraySerialzer::Deserlize(path.string());
		Ref<EditorAssetManager> editorAssetManager = Project::GetActive()->GetEditorAssetManger();
		std::string pathForTex = path.string() + "-";
		int index = 0;
		for (auto& vertexbuffer : vertexArray->GetVertexBuffers())
		{
			std::string pathWitheExtension = pathForTex + std::to_string(index);
			if (!editorAssetManager->IsAssetLoaded(pathWitheExtension))
			{
				AssetMetadata metadeta;
				metadeta.Type = vertexbuffer->GetType();
				editorAssetManager->CreateAsset(pathWitheExtension, (Ref<Asset>)vertexbuffer, metadeta);
			}
			vertexbuffer->Handle = editorAssetManager->GetAssetHandle(pathWitheExtension);
			Ref<VertexBuffer> assetVertexBuffer = AssetManager::GetAsset<VertexBuffer>(vertexbuffer->Handle);

			RY_CORE_ASSERT(assetVertexBuffer == vertexbuffer, "ERORO: VertexArrayImporter::LoadVertexArry, we get two diverentTextures they schoud by EQUEL!");
			index++;
		}
		Ref<IndexBuffer> indexBuffer = vertexArray->GetIndexBuffers();
		if(indexBuffer)
		{
			std::string pathWitheExtension = pathForTex + "i";
			if (!editorAssetManager->IsAssetLoaded(pathWitheExtension))
			{
				AssetMetadata metadeta;
				metadeta.Type = indexBuffer->GetType();
				editorAssetManager->CreateAsset(pathWitheExtension, (Ref<Asset>)indexBuffer, metadeta);
			}
			indexBuffer->Handle = editorAssetManager->GetAssetHandle(pathWitheExtension);
			Ref<IndexBuffer> assetIndexBuffer = AssetManager::GetAsset<IndexBuffer>(indexBuffer->Handle);
			RY_CORE_ASSERT(assetIndexBuffer == indexBuffer, "ERORO: VertexArrayImporter::LoadVertexArry, we get two diverentTextures they schoud by EQUEL!");
			index++;
		}
		return vertexArray;
	}

	void VertexArrayImporter::ReLoadeVertexArray(AssetHandle handle, const std::filesystem::path& path)
	{
	}
}