#pragma once
#include <Rynex/Asset/Base/Asset.h>
#include <Rynex/Asset/Base/AssetMetadata.h>

namespace Rynex {

    using AssetMap = std::map<AssetHandle, Ref<Asset>>;

    class AssetManagerBase
    {
    public:
        virtual ~AssetManagerBase() = default;
        virtual void OnAttach() = 0;
        virtual void OnDetach() = 0;

        virtual bool IsAssetHandleValid(AssetHandle handle) const  = 0;
        virtual bool IsAssetHandleValid(const FileSystem::Path& path) const = 0;
        virtual bool IsAssetInteral(AssetHandle handle) const = 0;

        virtual bool IsAssetLoaded(AssetHandle handle) const = 0;
        virtual bool IsAssetLoaded(const FileSystem::Path& path) const = 0;

        virtual Ref<Asset> GetAsset(AssetHandle handle) = 0;
        virtual Ref<Asset> GetAssetAsync(AssetHandle handle) = 0;
        virtual Ref<Asset> GetAsset(const FileSystem::Path& path) = 0;
        virtual Ref<Asset> GetAssetAsync(const FileSystem::Path& path) = 0;

        virtual const AssetHandle GetAssetHandle(const FileSystem::Path& path) const = 0;

        virtual const AssetMetadata GetMetadata(AssetHandle handle) const = 0;
        virtual const AssetMetadata GetMetadata(const FileSystem::Path& path) const = 0;

        virtual void CreatLocaleAsset(Ref<Asset> asset, AssetMetadata& metadata, AssetHandle handle) = 0;
        virtual AssetHandle CreatLocaleAsset(Ref<Asset> asset, AssetMetadata& metadata) = 0;

        virtual Ref<Asset> GetLocaleAsset(AssetHandle handle) = 0;
        virtual void DeleteLocaleAsset(AssetHandle handle) = 0;

    };

}

