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

	};

	std::string_view AssetTypeToString(AssetType type);
	AssetType AssetTypeFromString(std::string_view assetType);
	AssetType GetAssetTypeFromFilePath(const std::filesystem::path& filePath);

	class Asset
	{
	public:
		//virtual void CreateAsset() const = 0;
		//virtual void SetHandle(AssetHandle handle) { m_Handle = handle; }
		//virtual AssetHandle GetHandle() { return m_Handle; }
		AssetHandle Handle;

		virtual AssetType GetType() const = 0;

		//virtual void* GetLoadedData() const = 0;

		//virtual std::string GetFilePath() const { return m_FilePath; }
		//virtual std::string GetName() const { return m_Name; }
		//virtual std::string GetLoadTime() const { return m_LoadingTime; }
	//protected:
		
		
	};

}


