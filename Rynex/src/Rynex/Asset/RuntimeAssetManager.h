#pragma once
#include "Base/AssetManagerBase.h"
#include "Base/AssetMetadata.h"
#include "Rynex/Asset/Base/Asset.h"
#include "EditorAssetManager.h"

#include <map>

namespace Rynex {

  

    class RuntimeAssetManager : public AssetManagerBase
    {
    public:
        RuntimeAssetManager();

        virtual bool IsAssetHandleValid(AssetHandle handle) const override;
        virtual Ref<Asset> GetAsset(AssetHandle handle) override;
        virtual bool IsAssetLoaded(AssetHandle handle) const override;

        void ImportAsset(const std::filesystem::path& filepath);

        const AssetMetadata& GetMetadata(AssetHandle handle) const;
    private:
        AssetMap m_LoadedAssets;
        AssetRegistry m_AssetRegistry;
        PathRegistry m_PathRegistry;
        AssetDirectory m_AssetDirectorys;
    };

}

