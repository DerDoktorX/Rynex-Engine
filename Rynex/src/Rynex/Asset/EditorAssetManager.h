#pragma once

#include "Base/AssetManagerBase.h"
#include "Base/AssetMetadata.h"
#include "Rynex/Asset/Base/Asset.h"

#include <map>



namespace Rynex {

	struct AssetFileDirectory
	{
		std::vector<std::filesystem::path> Folders;
		std::vector<AssetHandle> Files;
		std::string FolderName;
		std::filesystem::path FolderPath;
	};

	using HandleRegistry = std::map<AssetHandle, AssetMetadata>;
	using PathRegistry = std::map<std::filesystem::path, AssetHandle>;
	using DirectoryRegistry = std::map<std::filesystem::path, AssetFileDirectory>;


	class AssetRegistry
	{
	public:
		bool IsAssetInRegistry(AssetHandle handle) const;
		bool IsAssetInRegistry(const std::filesystem::path& path) const;
		bool IsDirectoryInRegistry(const std::filesystem::path& parentPath) const;

		void CreateAsset(const std::filesystem::path& path, AssetHandle handle = AssetHandle(), AssetMetadata metadata = AssetMetadata(), bool findDirectOnDisc = true);

		bool UpdateAssetData(AssetHandle handle, AssetMetadata metadata);
		bool AddDirectoryToParent(const std::filesystem::path& parentPath);

		AssetHandle GetAssetHandle(const std::filesystem::path& path);

		// const AssetMetadata& GetMetadata(AssetHandle handle);
		// const AssetMetadata& GetMetadata(const std::filesystem::path& path);

		AssetMetadata& GetMetadata(AssetHandle handle);
		AssetMetadata& GetMetadata(const std::filesystem::path& path);

		const AssetFileDirectory& GetAssetFileDirectory(const std::filesystem::path& path) { return m_DirectoryRegistry[path.generic_string()]; }

		const HandleRegistry& GetHandleRegistry() const { return m_HandleRegistry; }
		const PathRegistry& GetPathRegistry() const { return m_PathRegistry; }
		const DirectoryRegistry& GetDirectorysRegistry() const { return m_DirectoryRegistry; }

	private:
		HandleRegistry m_HandleRegistry;
		PathRegistry m_PathRegistry;
		DirectoryRegistry m_DirectoryRegistry;
	};

	class AssetInternalRegistry
	{
	public:
		bool IsAssetInteralInRegistry(AssetHandle handle) const;

		
		void CreateAssetInteral(Ref<Asset>& asset, AssetMetadata metadata, AssetHandle handle = AssetHandle());
		

		bool UpdateAssetData(AssetHandle handle, AssetMetadata metadata);

		// const AssetMetadata& GetMetadata(AssetHandle handle);
		// const AssetMetadata& GetMetadata(const std::filesystem::path& path);

		AssetMetadata& GetInteralMetadata(AssetHandle handle);
		Ref<Asset> GetAssetInteral(AssetHandle handle);

		const HandleRegistry& GetHandleInteralRegistry() const { return m_HandleRegistry; }

	private:
		HandleRegistry m_HandleRegistry;
		AssetMap m_AssetMap;
	};

	class EditorAssetManager : public AssetManagerBase
	{
	public:
		EditorAssetManager();
		
		virtual void OnAttach();
		virtual void OnDetach();

		virtual Ref<Asset> GetAsset(AssetHandle handle) override;
		virtual Ref<Asset> GetAsset(const std::filesystem::path& path) override;

		virtual bool IsAssetHandleValid(AssetHandle handle) const override;
		virtual bool IsAssetHandleValid(const std::filesystem::path& path) const;

		virtual bool IsAssetLoaded(AssetHandle handle) const override;
		virtual bool IsAssetLoaded(const std::filesystem::path& path) const;
		
		void CreateAsset(const std::filesystem::path& path, AssetHandle handle = AssetHandle(), AssetMetadata metadata = AssetMetadata(), bool findDirectOnDisc = true) { m_AssetRegistry.CreateAsset(path, handle, metadata, findDirectOnDisc); }
		bool AddDirectoryToParent(const std::filesystem::path& parentPath) { return m_AssetRegistry.AddDirectoryToParent(parentPath); }

		//
		AssetMetadata& GetMetadata(AssetHandle handle) { return m_AssetRegistry.GetMetadata(handle); }
		AssetMetadata& GetMetadata(const std::filesystem::path& path) { return m_AssetRegistry.GetMetadata(path); }

		AssetHandle GetAssetHandle(const std::filesystem::path& path) { return m_AssetRegistry.GetAssetHandle(path); }

		const AssetFileDirectory& GetAssetFileDirectory(const std::filesystem::path& path) { return m_AssetRegistry.GetAssetFileDirectory(path); }


		const HandleRegistry& GetHandleRegistry() const { return m_AssetRegistry.GetHandleRegistry(); }
		const PathRegistry& GetPathRegistry() const { return  m_AssetRegistry.GetPathRegistry(); }
		const DirectoryRegistry& GetDirectorysRegistry() const { return  m_AssetRegistry.GetDirectorysRegistry(); }

		// InteralAsset
		void CreateAssetInteral(Ref<Asset>& asset, AssetMetadata metadata, AssetHandle handle = AssetHandle()) { return m_AssetInteralRegestriy.CreateAssetInteral(asset, metadata, handle); };

		AssetMetadata& GetInteralMetadata(AssetHandle handle) { return m_AssetInteralRegestriy.GetInteralMetadata(handle); }
		Ref<Asset> GetAssetInteral(AssetHandle handle) { return m_AssetInteralRegestriy.GetAssetInteral(handle); };

		const HandleRegistry& GetHandleInteralRegistry() const { return m_AssetInteralRegestriy.GetHandleInteralRegistry(); }
		//
		void SerialzeAsseRegistry();
		bool DeserialzeAssetRegistry();
	private:
		AssetMap m_LoadedAssets;
		AssetRegistry m_AssetRegistry;
		AssetInternalRegistry m_AssetInteralRegestriy;
	};

}

