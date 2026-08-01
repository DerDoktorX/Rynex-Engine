#pragma once

#include "AssetMetadata.h"

namespace Rynex {

	class AssetImporter
	{
	public: 
		static Ref<Asset> ImportAsset(AssetHandle handle, const AssetMetadata& metadata);
#if RY_EDITOR_ASSETMANGER_THREADE ? 0 : 0
		static Ref<Asset> ImportAssetAsync(AssetHandle handle, const AssetMetadata& metadata);
#endif
		static bool ReLoadeAsset(AssetHandle handle, const AssetMetadata& metadata);

		static Ref<Asset> ImportFromAssetPack();
	};

}

