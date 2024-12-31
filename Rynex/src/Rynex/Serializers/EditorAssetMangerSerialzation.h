#pragma once


#if RY_EDITOR_ASSETMANGER_THREADE

 #include "Rynex/Asset/EditorAssetManager.h"
#else
#endif

namespace Rynex {
	class AssetRegistry;
	

	class EditorAssetMangerSerialzation
	{
	public:
		static bool Serilze(const std::filesystem::path& filepath, AssetRegistry& assetRegistry);
		static bool Deserilze(const std::filesystem::path& filepath, AssetRegistry& assetRegistry);

#if RY_EDITOR_ASSETMANGER_THREADE
#if 1
		static bool SerilzeThread(const std::filesystem::path& filepath, std::map<AssetHandle, AssetMetadata>* handleReg);
		static bool DeserilzeThread(const std::filesystem::path& filepath, std::map<AssetHandle, AssetMetadata>* handleReg, std::map<std::filesystem::path, AssetHandle>* pathReg);
#endif
#endif
	
	};
}
