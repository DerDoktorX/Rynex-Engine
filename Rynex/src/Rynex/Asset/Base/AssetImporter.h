#pragma once

#include "AssetMetadata.h"

namespace Rynex {

	class AssetImporter
	{
	public: 
		static Ref<Asset> ImportAsset(AssetHandle handle, const AssetMetadata& metadata, bool async);
#if RY_EDITOR_ASSETMANGER_THREADE ? 0 : 0
		static Ref<Asset> ImportAssetAsync(AssetHandle handle, const AssetMetadata& metadata, bool async);
#endif
		static void ReLoadeAsset(AssetHandle handle, const AssetMetadata& metadata, bool async);

		static Ref<Asset> ImportFromAssetPack();
	};

}

