#pragma once

#include "Base/AssetManagerBase.h"
#include "Base/AssetMetadata.h"
#include "Rynex/Asset/Base/Asset.h"
#include "Rynex/Asset/EditorAssetManager.h"


#include <map>

namespace Rynex {

   
  
    class RuntimeAssetManager : public AssetManagerBase
    {
    public:
        RuntimeAssetManager();

        virtual bool IsAssetHandleValid(AssetHandle handle) const override;
        virtual bool IsAssetHandleValid(const std::filesystem::path& filepath) const override;
        virtual bool IsAssetInteral(AssetHandle handle) const override;
        virtual bool IsAssetLoaded(AssetHandle handle) const override;
        virtual bool IsAssetLoaded(const std::filesystem::path& filepath) const override;

        virtual Ref<Asset> GetAsset(AssetHandle handle, bool async = false) override;
        virtual Ref<Asset> GetAsset(const std::filesystem::path& filepath) override;
        virtual Ref<Asset> GetAssetAsync(AssetHandle handle) override;
        virtual Ref<Asset> GetAssetAsync(const std::filesystem::path& path) override;

        
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

