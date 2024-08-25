#pragma once
#include <Rynex/Core/UUID.h>

namespace Rynex {

	using AssetHandle = UUID;

	enum class AssetType : uint16_t
	{
		None = 0,
		Scene,
		Shader,
		Texture, Texture2D,
#if 0
		Value, Vector2, Vector3, Vector4, Matrix3x3, Matrix4x4,
		Entity, SceneCamera,
		FrameBuffer,
		VertexBuffer, IndexBuffer,
#endif
	};

	std::string_view AssetTypeToString(AssetType type);
	AssetType AssetTypeFromString(std::string_view assetType);
	AssetType GetAssetTypeFromFilePath(const std::filesystem::path& filePath);
	std::string GetAssetTypeDragAndDropName(AssetType type);

	class Asset
	{
	public:

		AssetHandle Handle;
		virtual AssetType GetType() const = 0;
		
	};

}


