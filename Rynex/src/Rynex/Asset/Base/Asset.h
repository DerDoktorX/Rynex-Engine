#pragma once
#include "Rynex/Core/UUID.h"

namespace Rynex {

	using AssetHandle = UUID;

	enum class AssetType
	{
		None = 0,
		Scene,
		Shader,
		Texture, 
		Texture2D,
		Value, Vector2, Vector3, Vector4, Matrix3x3, Matrix4x4,
		Entity, SceneCamera,
		Framebuffer,
		VertexBuffer, IndexBuffer, StorageBuffer, UniformBuffer,
		VertexArray, StorageArray,
		Mesh, Model
	};

	std::string_view AssetTypeToString(AssetType type);
	AssetType AssetTypeFromString(std::string_view assetType);
	AssetType GetAssetTypeFromFilePath(const std::filesystem::path& filePath);
	std::string GetAssetTypeDragAndDropName(AssetType type);
	std::string GetAssetTypeMoveAssetInfosName(AssetType type);

	class Asset
	{
	private:
		bool m_Valid = false;
	public:
		AssetHandle Handle;
		void Invalidate() { m_Valid = false; };
		void Validate() { m_Valid = true; };
		bool IsValid() const{ return m_Valid; };
		virtual AssetType GetType() const = 0;
		
	};

}


