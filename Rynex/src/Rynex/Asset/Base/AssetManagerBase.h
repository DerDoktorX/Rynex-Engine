#pragma once
#include "Asset.h"
#include "AssetMetadata.h"

namespace Rynex {

    using AssetMap = std::map<AssetHandle, Ref<Asset>>;

    class AssetManagerBase
    {
    public: 
        virtual void OnAttach() = 0;
        virtual void OnDetach() = 0;

        virtual bool IsAssetHandleValid(AssetHandle handle) const  = 0;
        virtual bool IsAssetHandleValid(const std::filesystem::path& path) const = 0;
        virtual bool IsAssetInteral(AssetHandle handle) const = 0;

        virtual bool IsAssetLoaded(AssetHandle handle) const = 0;
        virtual bool IsAssetLoaded(const std::filesystem::path& path) const = 0;

        virtual Ref<Asset> GetAsset(AssetHandle handle, bool async) = 0;
        virtual Ref<Asset> GetAssetAsync(AssetHandle handle) = 0;
        virtual Ref<Asset> GetAsset(const std::filesystem::path& path) = 0;
        virtual Ref<Asset> GetAssetAsync(const std::filesystem::path& path) = 0;


        virtual void CreatLocaleAsset(Ref<Asset> asset, AssetMetadata& metadata, AssetHandle handle) = 0;
        virtual AssetHandle CreatLocaleAsset(Ref<Asset> asset, AssetMetadata& metadata) = 0;

        virtual Ref<Asset> GetLocaleAsset(AssetHandle handle) = 0;
        virtual void DeleteLocaleAsset(AssetHandle handle) = 0;        
    };

}

