#pragma once

#include "AssetMetadata.h"

namespace Rynex {

	class AssetImporter
	{
	public: 
		static Ref<Asset> ImportAsset(AssetHandle handle, const AssetMetadata& metadata);
		static void ReLoadeAsset(AssetHandle handle, const AssetMetadata& metadata);
	};

}

