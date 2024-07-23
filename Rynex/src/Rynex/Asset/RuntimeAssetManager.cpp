#include "rypch.h"
#include "EditorAssetManager.h"
#include "Base/AssetImporter.h"
//#include "Rynex/Serializers/YAML.h"
#include "Rynex/Project/Project.h"

namespace Rynex {



    RuntimeAssetManager::RuntimeAssetManager()
    {
        RY_PROFILE_FUNCTION();
        RY_CORE_ERROR("RuntimeAssetManager Constructor");
    }

    bool RuntimeAssetManager::IsAssetHandleValid(AssetHandle handle) const
    {
        return handle != 0 && m_AssetRegistry.find(handle) != m_AssetRegistry.end();

    }

    Ref<Asset> RuntimeAssetManager::GetAsset(AssetHandle handle)
    {
        RY_PROFILE_FUNCTION();
        // RY_CORE_ASSERT(false, "EditorAssetManager::GetAsset");
        if (!IsAssetHandleValid(handle))
            return nullptr;
        Ref<Asset> asset;
        if (IsAssetLoaded(handle))
        {
            asset = m_LoadedAssets.at(handle);
        }
        else
        {
            const AssetMetadata& metadata = GetMetadata(handle);
            m_AssetRegistry[handle].State = AssetState::Loading;
            asset = AssetImporter::ImportAsset(handle, metadata);
            asset->Handle = handle;
            m_AssetRegistry[handle].State = AssetState::Ready;
            if (!asset) {}
            m_LoadedAssets[handle] = asset;


        }
        return asset;
    }

    bool RuntimeAssetManager::IsAssetLoaded(AssetHandle handle) const
    {
        return m_LoadedAssets.find(handle) != m_LoadedAssets.end();
    }

    void RuntimeAssetManager::ImportAsset(const std::filesystem::path& filepath)
    {
    }

    const AssetMetadata& RuntimeAssetManager::GetMetadata(AssetHandle handle) const
    {
        RY_PROFILE_FUNCTION();
        static AssetMetadata s_NullMetadata;
        auto it = m_AssetRegistry.find(handle);
        if (it == m_AssetRegistry.end())
            return s_NullMetadata;

        return it->second;
    }

}

