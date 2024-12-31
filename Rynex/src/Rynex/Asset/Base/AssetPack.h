#pragma once
#include "Rynex/Asset/Base/AssetPackFile.h"

namespace Rynex {
	class AssetPack
	{
	public:
		AssetPack(const std::filesystem::path& assetPackFile);
		void Serilize();
		
		void GetAsset();
	private:
		std::filesystem::path m_AssetPackFile;
		AssetPackFile m_File;
		std::unordered_set<AssetHandle> m_AssetHandleIndex;
	};
}

