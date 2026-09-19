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
		bool m_IsAsset = false;
		bool m_IsFolder = false;
		AssetHandle m_Handle = AssetHandle::Zero();
		AssetMetadata m_Metadata;
		AssetType m_Type = AssetType::None;
		std::string TypeString;
		Ref<Texture> m_Texture;
		AssetState m_State = AssetState::None;

		
		std::string m_Name;
		FileSystem::Path m_Path;
		FileSystem::Path m_RelativeProjectPath;
		std::string m_PathString;

		AssetBrowserData() = default;
		AssetBrowserData(bool isAsset, bool isFolder,
			AssetHandle handle, const AssetMetadata& metadata, 
			AssetType type,  const std::string& typeString, 
			AssetState state,const std::string& name, 
			const FileSystem::Path& path,  const std::string& pathString)
			: m_IsAsset(isAsset),
			m_IsFolder(isFolder),
			m_Handle(handle),
			m_Metadata(metadata),
			m_Type(type),
			TypeString(typeString), 
			m_Texture(nullptr),
			m_State(state),
			m_Name(name),
			m_Path(path),
			m_RelativeProjectPath(""),
			m_PathString(pathString)
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

	using ContentBrowserItems = std::vector<AssetBrowserData>;

	class AssetRegistry
	{
	public:
	    AssetRegistry();
	    ~AssetRegistry();

		bool IsAssetInRegistry(AssetHandle handle) const;
		bool IsAssetInRegistry(const FileSystem::Path& path) const;
		bool IsDirectoryInRegistry(const FileSystem::Path& parentPath) const;
		bool IsAssetPath(const FileSystem::Path& pathSystem) const;
		bool IsAssetInternal(AssetHandle handle) const;

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
		const DirectoryRegistry& GetDirectoryRegistry() const { return m_DirectoryRegistry; }

		bool IsCurrentAssetState(const FileSystem::Path& showPath) const;
		ContentBrowserItems GetCurrentAssetInformation(const FileSystem::Path& showPath);

		static std::string GetCurrentTimeStr();
	private:
		HandleRegistry m_HandleRegistry;
		PathRegistry m_PathRegistry;
		DirectoryRegistry m_DirectoryRegistry;

		bool m_Changes;
		FileSystem::Path m_CurrentPath;
	};

	

#pragma region EditorAssetManagerThreade

	struct AssetBrowserDataThreade
	{
		bool IsAssset = false;
		bool IsFolder = false;
		AssetHandle Handle = 0;
		AssetMetadata Metadata = AssetMetadata();
		AssetType Type = AssetType::None;
		std::string TypeString;
		Ref<Texture> Texture = nullptr;
		AssetState State = AssetState::None;

		std::string Name;
		FileSystem::Path Path;
		FileSystem::Path RelativProjectPath;
		std::string PathString;

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
			: Handle(handle), Metadata(metadata), FileAssetPath(metadata.m_FilePath.string())
		{ }

		AssetRigestriyPannel(const AssetRigestriyPannel&) = default;
		AssetRigestriyPannel(AssetRigestriyPannel&&) = default;

	};

	using HandleRegistryThreade = std::map<AssetHandle, AssetMetadata>;
	using PathRegistryThreade = std::map<FileSystem::Path, AssetHandle>;
	using DirectoryRegistryThreade = std::map<FileSystem::Path, AssetFileDirectoryThreade>;
	using ContentBrowserItemesThreade = std::vector<AssetBrowserDataThreade>;
	using RegisterItemesThreade = std::vector<AssetRigestriyPannel>;

	

	class EditorAssetManagerThread : public AssetManagerBase
	{
	public:
		template<typename K, typename T>
		using AssetMangerMap = AssetMangerMapMutex<K, T>;

	    EditorAssetManagerThread();
		virtual ~EditorAssetManagerThread() override;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		bool IsDirectoryInRegistry(const FileSystem::Path& parentPath) const;

		bool IsAssetPathExtensionValid(const FileSystem::Path& parentPath) const;
		virtual bool IsAssetHandleValid(AssetHandle handle) const override;
		virtual bool IsAssetHandleValid(const FileSystem::Path& path) const override;
		virtual bool IsAssetInteral(AssetHandle handle) const override;
		virtual bool IsAssetLoaded(const FileSystem::Path& path) const override;
		virtual bool IsAssetLoaded(AssetHandle handle) const override;

		void IsAssetHandleValidAsync(AssetHandle handle, bool* result);
		void IsAssetHandleValidAsync(const FileSystem::Path& path, bool* result);
		void IsAssetInternalAsync(AssetHandle handle, bool* result);
		void IsAssetLoadedAsync(AssetHandle handle, bool* result);
		void IsAssetLoadedAsync(const FileSystem::Path& path, bool* result);

		virtual const AssetMetadata GetMetadata(AssetHandle handle) const override;
		virtual const AssetMetadata GetMetadata(const FileSystem::Path& path) const override;
		void GetMetadataAsync(AssetHandle handle, AssetMetadata* metadat);
		void GetMetadataAsync(const FileSystem::Path& path, AssetMetadata* metadata);

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

		void ScanDirectory(const FileSystem::Path& path);
		void ReLodeAsset(AssetHandle handle);
		void ReLoadAsset(const FileSystem::Path& path);

		bool IsCurrentAssetState(const FileSystem::Path& showPath) const;
		ContentBrowserItemesThreade GetCurrentAssetInformation(const FileSystem::Path& showPath);

		bool IsCurrentRegistryAssetChange() const;
		RegisterItemesThreade GetCurrentAssetRegistry();

		void EventAsyncModified(const FileSystem::Path& path);
		void EventAsyncAdded(const FileSystem::Path& path);
		void EventAsyncRemoved(const FileSystem::Path& path);
		void EventAsyncRenamedNew(const FileSystem::Path& path);
		void EventAsyncRenamedOld(const FileSystem::Path& path);

		void SerializeAssetRegistry();
		bool DeserializeAssetRegistry();

		void DeleteDirectory(const FileSystem::Path& path);
		void DeleteFileAsset(AssetHandle handle);
		void UnLodeFileAsset(const AssetHandle handle);
		
		void ClearLoadedAssetList();
	private:

		bool ExecuteEvent(AssetHandle handle);
		inline bool IsFileAssetExist(const FileSystem::Path& path);
		bool CheckAssetFileExist(AssetHandle handle);

		void CreateAssetDirectory(const FileSystem::Path& parentPath);
		void AddAssetFileToAssetDirectory(const FileSystem::Path& assetPath, AssetHandle handle);
		void AddDirectoryToParent(const FileSystem::Path& prentPath);
		void AddDirectory(const FileSystem::Path& folder, const FileSystem::Path& parentFolder, std::map<FileSystem::Path, AssetFileDirectoryThreade>& folderMap);
		void CreateFileAsset(const FileSystem::Path& path);
		void CreateNewFileAsset(const FileSystem::Path& path);

		void GetMetadata(AssetHandle handle, AssetMetadata* metaData, std::mutex* metaDataMutex);
		void DeleteDirectory(const std::vector<FileSystem::Path>& paths);
		void DeleteFileAsset(const std::vector<AssetHandle>& handles);

		
        Ref<Asset> GetAssetLostConnection(AssetHandle handle);
	    void SetAssetMetadataLockState(AssetHandle handle, std::function<void(AssetMetadata&)> lambder);
	    void SetAssetMetadataUpdateContentBrowser(AssetMetadata& metadata);
	    void SetPathUpdateContentBrowser(const FileSystem::Path& metadata);
	    void SetAssetMetadataStateNotLoaded(AssetMetadata& metadata);
	    void SetAssetMetadataStateLoading(AssetMetadata& metadata);
	    void SetAssetMetadataStateError(AssetMetadata& metadata);
	    void SetAssetMetadataStateReady(AssetMetadata& metadata);
	    void SetAssetMetadataStateUpdating(int* aboutPtr, AssetMetadata& metadata);
	    void SetAssetMetadataStateLostConnection(AssetMetadata& metadata);
	private:
		AssetMangerMap<AssetHandle, Ref<Asset>>							    m_LoadedAssets;
		AssetMangerMap<AssetHandle, AssetMetadata>							m_HandleRegistry;
		AssetMangerMap<FileSystem::Path, AssetHandle>					    m_PathRegistry;
		AssetMangerMap<FileSystem::Path, AssetFileDirectoryThreade>         m_DirectoryRegistry;

		bool							m_FileChanges;
		bool							m_RegistryChanges;
		mutable std::mutex				m_ChangesMutex;

		uint32_t						m_CurrentProcess;
		mutable std::mutex				m_CurrentProcessMutex;

		FileSystem::Path			    m_CurrentPath;
		mutable std::mutex				m_CurrentPathMutex;

		FileSystem::Path			    m_BaseAssetPath;
		mutable std::mutex				m_BaseAssetPathMutex;

		mutable std::mutex				m_WorkingThreadMutex;
		std::vector<std::future<void>>	m_WorkingThread;
	};

#pragma endregion



}

