#pragma once

namespace Rynex {
	class AssetRegistry;

	class EditorAssetMangerSerialzation
	{
	public:
		static bool Serilze(const std::filesystem::path& filepath, AssetRegistry& assetRegistry);
		static bool Deserilze(const std::filesystem::path& filepath, AssetRegistry& assetRegistry);
	};
}
