#pragma once

#include <Rynex/Asset/Base/AssetManagerBase.h>
#include <Rynex/Asset/Base/AssetMetadata.h>
#include <Rynex/Asset/Base/Asset.h>
#include <Rynex/Asset/EditorAssetManager.h>

namespace Rynex {

   
  
    class RuntimeAssetManager : public AssetManagerBase
    {
    public:
        RuntimeAssetManager();

        virtual bool IsAssetHandleValid(AssetHandle handle) const override;
        virtual bool IsAssetHandleValid(const FileSystem::Path& filepath) const override;
        virtual bool IsAssetInteral(AssetHandle handle) const override;
        virtual bool IsAssetLoaded(AssetHandle handle) const override;
        virtual bool IsAssetLoaded(const FileSystem::Path& filepath) const override;

        virtual Ref<Asset> GetAsset(AssetHandle handle) override;
        virtual Ref<Asset> GetAsset(const FileSystem::Path& filepath) override;
        virtual Ref<Asset> GetAssetAsync(AssetHandle handle) override;
        virtual Ref<Asset> GetAssetAsync(const FileSystem::Path& path) override;

        virtual const AssetHandle GetAssetHandle(const FileSystem::Path& path) const override;

        virtual const AssetMetadata GetMetadata(AssetHandle handle) const override;
        virtual const AssetMetadata GetMetadata(const FileSystem::Path& path) const override;

        virtual void CreatLocaleAsset(Ref<Asset> asset, AssetMetadata& metadata, AssetHandle handle) override;
        virtual AssetHandle CreatLocaleAsset(Ref<Asset> asset, AssetMetadata& metadata) override;

        virtual Ref<Asset> GetLocaleAsset(AssetHandle handle) override;
        virtual void DeleteLocaleAsset(AssetHandle handle) override;

       
        void ImportAsset(const std::filesystem::path& filepath);
        const AssetMetadata& GetMetadata(AssetHandle handle);

    private:
        AssetMap m_LoadedAssets;
        AssetRegistry m_AssetRegistry;
    };


}

