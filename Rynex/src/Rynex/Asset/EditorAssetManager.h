#pragma once


#include "Base/AssetManagerBase.h"
#include "Base/AssetMetadata.h"
#include "Rynex/Asset/Base/Asset.h"

#include <map>

namespace Rynex {

    struct AssetFileDirectory
    {
#if 0
        std::map<std::filesystem::path, std::string> Folders;
        std::map<std::filesystem::path, AssetHandle> Files;
#else
        std::vector<std::filesystem::path> Folders;
        std::vector<AssetHandle> Files;
#endif
        std::string FolderName;
        std::filesystem::path FolderPath;
    };


    using AssetRegistry = std::map<AssetHandle, AssetMetadata>;
    using PathRegistry = std::map<std::filesystem::path, AssetHandle>;
    using AssetDirectory = std::map<std::filesystem::path, AssetFileDirectory>;

    
    class EditorAssetManager : public AssetManagerBase
    {
    public:
        EditorAssetManager();

        virtual bool IsAssetHandleValid(AssetHandle handle) const override;
        virtual Ref<Asset> GetAsset(AssetHandle handle) override;
        virtual Ref<Asset> EditorAssetManager::GetAssetFromPath(const std::filesystem::path& path) override;

        virtual bool IsAssetLoaded(AssetHandle handle) const override;

        void ImportAsset(const std::filesystem::path& filepath);

        const AssetMetadata& GetMetadata(AssetHandle handle) const;
        void SetMetadataState(AssetHandle handle, AssetState state);

        const std::filesystem::path& GetFilePath(AssetHandle handle) const;
        const AssetRegistry& GetAssetRegistry() const { return m_AssetRegistry; }
        const AssetHandle& GetAssetHandle(const std::filesystem::path& path);
        const AssetHandle& AddFileToRegistry(const std::filesystem::path& filepath);

        const void ReLoadeAsset(AssetHandle handle) const;

        const AssetDirectory& GetAssetDirectorys() const { return m_AssetDirectorysRegistry; }
        AssetFileDirectory& GetAssetFileDirectory(const std::filesystem::path& curentPath) { return m_AssetDirectorysRegistry[curentPath]; }

        void CreateDirektoryRegestriy(const std::filesystem::path& curentPath);
        void AddAssetToDirektory(const std::filesystem::path& path, const std::filesystem::path& curentPath);
        void AddDirektoryToDirektory(const std::filesystem::path& path, const std::filesystem::path& curentPath);
        void CreateDirektoryUnknownTypeDirektory(const std::filesystem::path& path, const std::filesystem::path& curentPath);
        
        void SerialzeAssetRegestriy();
        bool DeserialzeAssetRegestriy();

    private: 
        AssetMap m_LoadedAssets;

        AssetRegistry m_AssetRegistry;

        PathRegistry m_PathRegistry;

        AssetDirectory m_AssetDirectorysRegistry;
    };

}

