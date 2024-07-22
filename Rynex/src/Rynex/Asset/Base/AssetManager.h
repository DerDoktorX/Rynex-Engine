#pragma once

#include "Asset.h"
#include "Rynex/Project/Project.h"

namespace Rynex {

	class AssetManager
	{
	public:
		template<typename T>
		static Ref<T> GetAsset(AssetHandle handle)
		{
			Ref<Asset> asset = Project::GetActive()->GetAssetManger()->GetAsset(handle);
			return std::static_pointer_cast<T>(asset);
		}
	};

}


