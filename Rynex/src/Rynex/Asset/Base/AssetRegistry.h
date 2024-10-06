#pragma once
#include "AssetManagerBase.h"
#include "AssetMetadata.h"

namespace Rynex {

	class RegistryAsset
	{
	public:
		std::map<AssetHandle, AssetMetadata> m_AssetRegistry;

	};

}


