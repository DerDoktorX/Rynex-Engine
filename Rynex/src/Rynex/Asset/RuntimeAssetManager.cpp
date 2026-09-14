#include "rypch.h"
#include "RuntimeAssetManager.h"
#include <Rynex/Asset/Base/AssetImporter.h>
#include <Rynex/Project/Project.h>


namespace Rynex {



    RuntimeAssetManager::RuntimeAssetManager()
    {
        RY_CORE_ERROR("RuntimeAssetManager Constructor");
    }

    bool RuntimeAssetManager::IsAssetHandleValid(AssetHandle handle) const
    {
        return handle != 0 && m_AssetRegistry.IsAssetInRegistry(handle);
    }

    bool RuntimeAssetManager::IsAssetHandleValid(const FileSystem::Path& filepath) const
    {
        RY_CORE_ASSERT(false, "This Funktion 'RuntimeAssetManager::IsAssetHandleValid' Don't need to Exist in Runtime!");
        return false;
    }

    bool RuntimeAssetManager::IsAssetInteral(AssetHandle handle) const
    {
        return m_AssetRegistry.IsAssetInteral(handle);
    }

    Ref<Asset> RuntimeAssetManager::GetAsset(AssetHandle handle)
    {
        if (m_AssetRegistry.IsAssetInRegistry(handle))
        {
            Ref<Asset> asset = Ref<Asset>();
            if (m_LoadedAssets.find(handle) != m_LoadedAssets.end())
            {
                asset = m_LoadedAssets.at(handle);
            }
            else if (!IsAssetInteral(handle))
            {
                AssetMetadata& metadata = m_AssetRegistry.GetMetadata(handle);
                metadata.State = AssetState::Loading;
                asset = AssetImporter::ImportAsset(handle, metadata);
                metadata.State = AssetState::Ready;
                asset->Handle = handle;
                if (!asset) {}
                m_LoadedAssets[handle] = asset;
            }
            else if (IsAssetInteral(handle))
            {
                RY_CORE_ASSERT(false, "Error on: 'RuntimeAssetManager::GetAsset' Asset is known In Regestry as Interale-Asset But got not Loadede! This cinde off error is a def Error");
                return nullptr;
            }
            else
            {
                RY_CORE_ASSERT(false, "Error on: 'RuntimeAssetManager::GetAsset' I Dont't know some isuse kan go to hear!");
                return nullptr;
            }

        return asset;

        }
        RY_CORE_ASSERT(false, "Error on: 'RuntimeAssetManager::GetAsset' No Asset handle found!");
        return Ref<Asset>();
    }

    Ref<Asset> RuntimeAssetManager::GetAsset(const FileSystem::Path& filepath)
    {
        RY_CORE_ERROR("This Funktion 'RuntimeAssetManager::GetAsset' Don't need to Exist in Runtime!");
        return GetAsset(m_AssetRegistry.GetAssetHandle(filepath));
    }

    Ref<Asset> RuntimeAssetManager::GetAssetAsync(AssetHandle handle)
    {
        return GetAsset(handle);
    }

    Ref<Asset> RuntimeAssetManager::GetAssetAsync(const FileSystem::Path& path)
    {
        AssetHandle handle = m_AssetRegistry.GetAssetHandle(path);
        return GetAsset(handle);
    }

    const AssetHandle RuntimeAssetManager::GetAssetHandle(const FileSystem::Path& path) const
    {
        return m_AssetRegistry.GetAssetHandleConst(path);
    }

    const AssetMetadata RuntimeAssetManager::GetMetadata(AssetHandle handle) const
    {
        if (m_AssetRegistry.IsAssetInRegistry(handle))
            return GetMetadata(handle);
        return AssetMetadata();
    }

    const AssetMetadata RuntimeAssetManager::GetMetadata(const FileSystem::Path& path) const
    {
        RY_CORE_ERROR("This Funktion 'RuntimeAssetManager::IsAssetLoaded' Don't need to Exist in Runtime!");
        return GetMetadata(m_AssetRegistry.IsAssetInRegistry(path));
    }

    bool RuntimeAssetManager::IsAssetLoaded(AssetHandle handle) const
    {
        return m_LoadedAssets.find(handle) != m_LoadedAssets.end();
    }

    bool RuntimeAssetManager::IsAssetLoaded(const FileSystem::Path& filepath) const
    {
        RY_CORE_ERROR("This Funktion 'RuntimeAssetManager::IsAssetLoaded' Don't need to Exist in Runtime!");
        if (!m_AssetRegistry.IsAssetInRegistry(filepath))
            return false;

        AssetHandle handle = m_AssetRegistry.GetAssetHandleConst(filepath);
        return IsAssetLoaded(handle);
    }

    void RuntimeAssetManager::CreatLocaleAsset(Ref<Asset> asset, AssetMetadata& metadata, AssetHandle handle)
    {
        RY_CORE_ASSERT(false);
    }

    AssetHandle RuntimeAssetManager::CreatLocaleAsset(Ref<Asset> asset, AssetMetadata& metadata)
    {
        AssetHandle handle = m_AssetRegistry.CreatLocaleAsset(asset, metadata);
        m_LoadedAssets[handle] = asset;
        if (IsAssetHandleValid(handle) && IsAssetLoaded(handle))
            return handle;
        return AssetHandle(0);
    }

    Ref<Asset> RuntimeAssetManager::GetLocaleAsset(AssetHandle handle)
    {
        RY_CORE_ASSERT(false, "This funktion is not any longer Sepoted! Use the funktion GetAsset");
        if (IsAssetLoaded(handle) && !IsAssetHandleValid(handle))
            return m_LoadedAssets.at(handle);
        return nullptr;
    }

    void RuntimeAssetManager::DeleteLocaleAsset(AssetHandle handle)
    {
        if (IsAssetInteral(handle))
        {
            m_AssetRegistry.DeleteLocaleAsset(handle);
            if(!IsAssetHandleValid(handle))
                m_LoadedAssets.erase(handle);
        }
    }

    void RuntimeAssetManager::ImportAsset(const std::filesystem::path& filepath)
    {
        RY_CORE_ERROR("This Funktion 'RuntimeAssetManager::ImportAsset' Don't need to Exist in Runtime!");
    }

    const AssetMetadata& RuntimeAssetManager::GetMetadata(AssetHandle handle)
    {
        return m_AssetRegistry.GetMetadata(handle);
    }

    

}

