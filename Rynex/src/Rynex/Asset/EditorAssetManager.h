#pragma once


#include <future>
#include <Rynex/Asset/Base/AssetManagerBase.h>
#include <Rynex/Asset/Base/AssetMetadata.h>
#include <Rynex/Asset/Base/Asset.h>
#include <Rynex/Asset/Base/AssetMangerMapMutex.h>
#include <Rynex/Asset/Base/AssetMangerLeftRightMap.h>

#include <Rynex/Renderer/API/Texture.h>

#include <map>



namespace Rynex {

	struct AssetBrowserData
	{
		bool IsAssset = false;
		bool IsFolder = false;
		AssetHandle Handle = 0;
		AssetMetadata Metadata = AssetMetadata();
		AssetType Type = AssetType::None;
		std::string TypeString = "";
		Ref<Texture> Texture = nullptr;
		AssetState State = AssetState::None;

		
		std::string Name = "";
		FileSystem::Path Path = "";
		FileSystem::Path RelativProjectPath = "";
		std::string PathString = "";

		AssetBrowserData() = default;
		AssetBrowserData(bool isAssset, bool isFolder, 
			AssetHandle handle, const AssetMetadata& metadata, 
			AssetType type,  const std::string& typeString, 
			AssetState state,const std::string& name, 
			const FileSystem::Path& path,  const std::string& pathString)
			: IsAssset(isAssset), 
			IsFolder(isFolder),
			Handle(handle),
			Metadata(metadata),
			Type(type), 
			TypeString(typeString), 
			State(state),
			Name(name), 
			Path(path),
			RelativProjectPath(""), 
			PathString(pathString), 
			Texture(nullptr)
		{}
		AssetBrowserData(AssetBrowserData&&) = default;
		AssetBrowserData(const AssetBrowserData&) = default;
	};

	struct AssetFileDirectory
	{
		std::vector<FileSystem::Path> m_Folders;
		std::vector<AssetHandle> m_Files;
		std::string m_FolderName;
		FileSystem::Path m_FolderPath;
	};

	using HandleRegistry = std::map<AssetHandle, AssetMetadata>;
	using PathRegistry = std::map<FileSystem::Path, AssetHandle>;
	using DirectoryRegistry = std::map<FileSystem::Path, AssetFileDirectory>;
	using ContentBrowserItemes = std::vector<AssetBrowserData>;

	class AssetRegistry
	{
	public:
		bool IsAssetInRegistry(AssetHandle handle) const;
		bool IsAssetInRegistry(const FileSystem::Path& path) const;
		bool IsDirectoryInRegistry(const FileSystem::Path& parentPath) const;
		bool IsAssetPath(const FileSystem::Path& pathSystem) const;
		bool IsAssetInteral(AssetHandle handle) const;

		void CreateAsset(const FileSystem::Path& path, AssetHandle handle = AssetHandle(), AssetMetadata metadata = AssetMetadata(), bool findDirectOnDisc = true);
		AssetHandle CreatLocaleAsset(Ref<Asset>& asset, AssetMetadata& metadata);

		bool UpdateAssetData(AssetHandle handle, AssetMetadata metadata);
		bool AddDirectoryToParent(const FileSystem::Path& parentPath);

		AssetHandle GetAssetHandle(const FileSystem::Path& path);
		AssetHandle GetAssetHandleConst(const FileSystem::Path& path) const;

		void DeleteFolder(const FileSystem::Path& folderPath);
		void DeleteLocaleAsset(AssetHandle handle);
		AssetMetadata& GetMetadata(AssetHandle handle);
		const AssetMetadata& GetMetadataConst(AssetHandle handle) const;
		AssetMetadata& GetMetadata(const FileSystem::Path& path);

		const AssetFileDirectory& GetAssetFileDirectory(const FileSystem::Path& path);

		const HandleRegistry& GetHandleRegistry() const { return m_HandleRegistry; }
		const PathRegistry& GetPathRegistry() const { return m_PathRegistry; }
		const DirectoryRegistry& GetDirectorysRegistry() const { return m_DirectoryRegistry; }

		bool IsCurentAssetState(const FileSystem::Path& showPath) const;
		ContentBrowserItemes GetCurentAssetInformation(const FileSystem::Path& showPath);

		static std::string GetCurrentTimeStr();
	private:
		HandleRegistry m_HandleRegistry;
		PathRegistry m_PathRegistry;
		DirectoryRegistry m_DirectoryRegistry;

		bool m_Changes = true;
		FileSystem::Path m_CurentPath = "";
	};

	

#pragma region EditorAssetManagerThreade

	struct AssetBrowserDataThreade
	{
		bool IsAssset = false;
		bool IsFolder = false;
		AssetHandle Handle = 0;
		AssetMetadata Metadata = AssetMetadata();
		AssetType Type = AssetType::None;
		std::string TypeString = "";
		Ref<Texture> Texture = nullptr;
		AssetState State = AssetState::None;

		std::string Name = "";
		FileSystem::Path Path = "";
		FileSystem::Path RelativProjectPath = "";
		std::string PathString = "";

		AssetBrowserDataThreade() = default;
		AssetBrowserDataThreade(bool isAssset, bool isFolder, AssetHandle handle, const AssetMetadata& metadata, AssetType type, const std::string& typeString, AssetState state, const std::string& name, const FileSystem::Path& path, const std::string& pathString)
			: IsAssset(isAssset), IsFolder(isFolder), Handle(handle), Metadata(metadata), Type(type), TypeString(typeString), State(state), Name(name), Path(path), RelativProjectPath(""), PathString(pathString), Texture(nullptr)
		{}
		AssetBrowserDataThreade(AssetBrowserDataThreade&&) = default;
		AssetBrowserDataThreade(const AssetBrowserDataThreade&) = default;
	};

	struct AssetFileDirectoryThreade
	{
		std::vector<FileSystem::Path>	    Folders;
		std::vector<AssetHandle>			AssetFiles;
		std::string							FolderName;
		FileSystem::Path				    FolderPath;
	};

	struct AssetRigestriyPannel
	{
		AssetHandle Handle = 0ull;
		AssetMetadata Metadata = AssetMetadata();
		std::string FileAssetPath = "";

		AssetRigestriyPannel() = default;

		AssetRigestriyPannel(AssetHandle handle, const AssetMetadata& metadata)
			: Handle(handle), Metadata(metadata), FileAssetPath(metadata.FilePath.string())
		{ }

		AssetRigestriyPannel(const AssetRigestriyPannel&) = default;
		AssetRigestriyPannel(AssetRigestriyPannel&&) = default;

	};

	using HandleRegistryThreade = std::map<AssetHandle, AssetMetadata>;
	using PathRegistryThreade = std::map<FileSystem::Path, AssetHandle>;
	using DirectoryRegistryThreade = std::map<FileSystem::Path, AssetFileDirectoryThreade>;
	using ContentBrowserItemesThreade = std::vector<AssetBrowserDataThreade>;
	using RegisterItemesThreade = std::vector<AssetRigestriyPannel>;

	

	class EditorAssetManegerThreade : public AssetManagerBase
	{
	public:
		template<typename K, typename T>
		using AssetMangerMap = AssetMangerMapMutex<K, T>;


		virtual ~EditorAssetManegerThreade() = default;

		virtual void OnAttach();
		virtual void OnDetach();

		bool IsDirectoryInRegistry(const FileSystem::Path& parentPath);

		bool IsAssetPathExtensionVaild(const FileSystem::Path& parentPath) const;
		virtual bool IsAssetHandleValid(AssetHandle handle) const override;
		virtual bool IsAssetHandleValid(const FileSystem::Path& path) const override;
		virtual bool IsAssetInteral(AssetHandle handle) const override;
		virtual bool IsAssetLoaded(const FileSystem::Path& path) const override;
		virtual bool IsAssetLoaded(AssetHandle handle) const override;

		void IsAssetHandleValidAsync(AssetHandle handle, bool* result);
		void IsAssetHandleValidAsync(const FileSystem::Path& path, bool* result);
		void IsAssetInteralAsync(AssetHandle handle, bool* result);
		void IsAssetLoadedAsync(AssetHandle handle, bool* result);
		void IsAssetLoadedAsync(const FileSystem::Path& path, bool* result);

		virtual const AssetMetadata GetMetadata(AssetHandle handle) const override;
		virtual const AssetMetadata GetMetadata(const FileSystem::Path& path) const override;
		void GetMetadataAsync(AssetHandle handle, AssetMetadata* meatdat);
		void GetMetadataAsync(const FileSystem::Path& path, AssetMetadata* meatdat);

		virtual const AssetHandle GetAssetHandle(const FileSystem::Path& path) const override;

		void GetAssetHandleAsync(const FileSystem::Path& path, AssetHandle* handle);

		virtual Ref<Asset> GetAsset(AssetHandle handle) override;
		virtual Ref<Asset> GetAssetAsync(AssetHandle handle) override;
		virtual Ref<Asset> GetAsset(const FileSystem::Path& path) override;
		virtual Ref<Asset> GetAssetAsync(const FileSystem::Path& path) override;


		virtual AssetHandle CreatLocaleAsset(Ref<Asset> asset, AssetMetadata& metadata) override;
		virtual void CreatLocaleAsset(Ref<Asset> asset, AssetMetadata& metadata, AssetHandle handle) override;

		virtual Ref<Asset> GetLocaleAsset(AssetHandle handle) override;
		virtual void DeleteLocaleAsset(AssetHandle handle) override;

		AssetHandle CreatLocaleAssetAsync(Ref<Asset>& asset, AssetMetadata& metadata);
		Ref<Asset> GetLocaleAssetAsync(AssetHandle handle);

		void DeleteLocaleAssetAsync(AssetHandle handle);

		void ScaneDirectory(const FileSystem::Path& path);
		void ReLoadeAsset(AssetHandle handle);
		void ReLoadeAsset(const FileSystem::Path& path);

		bool IsCurentAssetState(const FileSystem::Path& showPath) const;
		ContentBrowserItemesThreade GetCurentAssetInformation(const FileSystem::Path& showPath);

		bool IsCurentRegistryAssetChandge() const;
		RegisterItemesThreade GetCurentAssetRegistry();

		void EventAsyncModified(const FileSystem::Path& path);
		void EventAsyncAdded(const FileSystem::Path& path);
		void EventAsyncRemoved(const FileSystem::Path& path);
		void EventAsyncRenamedNew(const FileSystem::Path& path);
		void EventAsyncRenamedOld(const FileSystem::Path& path);

		void SerialzeAssetRegistry();
		bool DeserialzeAssetRegistry();

		void DeleateDirectory(const FileSystem::Path& path);
		void DeleateFileAsset(AssetHandle handle);
		void UnLoadeFileAsset(const AssetHandle handle);
		
		void ClearLodeadAssetList();
	private:

		bool ExexuteEvent(AssetHandle handle);
		inline bool IsFileAssetExist(const FileSystem::Path& path);
		bool CheckAssetFileExist(AssetHandle handle);

		void CreateAssetDirectory(const FileSystem::Path& parentPath);
		void AddAssetFileToAssetDirectory(const FileSystem::Path& assetPath, const std::string& assetName, const std::string& assetPathString, AssetHandle handle);
		void AddDirectoryToParent(const FileSystem::Path& prentPath);
		void AddDirectory(const FileSystem::Path& path);
		void CreateFileAsset(const FileSystem::Path& path);
		void CreateNewFileAsset(const std::string& name, const FileSystem::Path& path, const std::string& pathMarker);

		void GetMetadata(AssetHandle handle, AssetMetadata* metaData, std::mutex* metaDataMutex);
		void DeleateDirectory(const std::vector<FileSystem::Path>& paths);
		void DeleateFileAsset(const std::vector<AssetHandle>& handles);

		
        Ref<Asset> GetAssetLostConection(AssetHandle handle);
	    void SetAssetMetadataLockState(AssetHandle handle, std::function<void(AssetMetadata&)> lambder);
	    void SetAssetMetadataUpdateContentBrowser(AssetMetadata& metadata);
	    void SetAssetMetadataStateNotLoaded(AssetMetadata& metadata);
	    void SetAssetMetadataStateLoading(AssetMetadata& metadata);
	    void SetAssetMetadataStateError(AssetMetadata& metadata);
	    void SetAssetMetadataStateReady(AssetMetadata& metadata);
	    void SetAssetMetadataStateUpdateing(int* abourtPtr, AssetMetadata& metadata);
	    void SetAssetMetadataStateLostConection(AssetMetadata& metadata);
	private:
		AssetMangerMap<AssetHandle, Ref<Asset>>							    m_LoadedAssets;
		AssetMangerMap<AssetHandle, AssetMetadata>							m_HandleRegistry;
		AssetMangerMap<FileSystem::Path, AssetHandle>					    m_PathRegistry;
		AssetMangerMap<FileSystem::Path, AssetFileDirectoryThreade>	            m_DirectoryRegistry;

		bool							m_FileChanges = true;
		bool							m_RegestryChanges = true;
		mutable std::mutex				m_ChangesMutex;

		uint32_t						m_CurentProzesses;
		mutable std::mutex				m_CurentProzessesMutex;

		FileSystem::Path			    m_CurentPath = "";
		mutable std::mutex				m_CurentPathMutex;

		FileSystem::Path			    m_BaseAssetPath = "";
		mutable std::mutex				m_BaseAssetPathMutex;

		mutable std::mutex				m_WorkingThreadMutex;
		std::vector<std::future<void>>	m_WorkingThread;
		
		
	};

#pragma endregion



}

