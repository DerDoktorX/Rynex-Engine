#pragma once
#include "Asset.h"
#include "AssetMetadata.h"

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

		template<typename T>
		static Ref<T> GetAsset(const std::filesystem::path& path)
		{
			Ref<Asset> asset = Project::GetActive()->GetAssetManger()->GetAsset(path);
			return std::static_pointer_cast<T>(asset);
		}

		template<typename T>
		static AssetHandle CreatLocaleAsset(Ref<T> asset)
		{
			AssetMetadata metadata = AssetMetadata();
			metadata.Type = asset->GetType();
			return Project::GetActive()->GetAssetManger()->CreatLocaleAsset(asset, metadata);
		}

		template<typename T>
		static Ref<T> GetLocaleAsset(AssetHandle handle)
		{
			RY_CORE_ASSERT(false, "This funktion is not any longer Sepoted! Use the funktion GetAsset");
			Ref<Asset> asset = Project::GetActive()->GetAssetManger()->GetLocaleAsset(handle);
			return std::static_pointer_cast<T>(asset);
		}

		static void DeleteLocaleAsset(AssetHandle handle)
		{
			Project::GetActive()->GetAssetManger()->DeleteLocaleAsset(handle);
		}
	};
	

}


