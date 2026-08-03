#pragma once


#include <Rynex/Asset/Base/AssetManagerBase.h>
#include <Rynex/Asset/Base/AssetMetadata.h>
#include <Rynex/Asset/Base/Asset.h>
#include <Rynex/Asset/Base/AssetMangerMapMutex.h>
#include <Rynex/Asset/Base/AssetMangerLeftRightMap.h>

#include <Rynex/Renderer/API/Texture.h>

#include <map>
#include <future>


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
		std::filesystem::path Path = "";
		std::filesystem::path RelativProjectPath = "";
		std::string PathString = "";

		AssetBrowserData() = default;
		AssetBrowserData(bool isAssset, bool isFolder, 
			AssetHandle handle, const AssetMetadata& metadata, 
			AssetType type,  const std::string& typeString, 
			AssetState state,const std::string& name, 
			const std::filesystem::path& path,  const std::string& pathString)
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
		std::vector<std::filesystem::path> Folders;
		std::vector<AssetHandle> Files;
		std::string FolderName;
		std::filesystem::path FolderPath;
		std::filesystem::path ReltiveProjectFolderPath;
	};

	using HandleRegistry = std::map<AssetHandle, AssetMetadata>;
	using PathRegistry = std::map<std::filesystem::path, AssetHandle>;
	using DirectoryRegistry = std::map<std::filesystem::path, AssetFileDirectory>;
	using ContentBrowserItemes = std::vector<AssetBrowserData>;

	class AssetRegistry
	{
	public:
		bool IsAssetInRegistry(AssetHandle handle) const;
		bool IsAssetInRegistry(const std::filesystem::path& path) const;
		bool IsDirectoryInRegistry(const std::filesystem::path& parentPath) const;
		bool IsAssetPath(const std::filesystem::path& path) const;
		bool IsAssetInteral(AssetHandle handle) const;

		void CreateAsset(const std::filesystem::path& path, AssetHandle handle = AssetHandle(), AssetMetadata metadata = AssetMetadata(), bool findDirectOnDisc = true);
		AssetHandle CreatLocaleAsset(Ref<Asset>& asset, AssetMetadata& metadata = AssetMetadata());

		bool UpdateAssetData(AssetHandle handle, AssetMetadata metadata);
		bool AddDirectoryToParent(const std::filesystem::path& parentPath);

		AssetHandle GetAssetHandle(const std::filesystem::path& path);
		AssetHandle GetAssetHandleConst(const std::filesystem::path& path) const;

		void DeleteFolder(const std::filesystem::path& folderPath);
		void DeleteLocaleAsset(AssetHandle handle);
		AssetMetadata& GetMetadata(AssetHandle handle);
		const AssetMetadata& GetMetadataConst(AssetHandle handle) const;
		AssetMetadata& GetMetadata(const std::filesystem::path& path);

		const AssetFileDirectory& GetAssetFileDirectory(const std::filesystem::path& path) { return m_DirectoryRegistry[path.generic_string()]; }

		const HandleRegistry& GetHandleRegistry() const { return m_HandleRegistry; }
		const PathRegistry& GetPathRegistry() const { return m_PathRegistry; }
		const DirectoryRegistry& GetDirectorysRegistry() const { return m_DirectoryRegistry; }

		bool IsCurentAssetState(const std::filesystem::path& showPath) const;
		ContentBrowserItemes GetCurentAssetInformation(const std::filesystem::path& showPath);

		static std::string GetCurrentTimeStr();
	private:
		HandleRegistry m_HandleRegistry;
		PathRegistry m_PathRegistry;
		DirectoryRegistry m_DirectoryRegistry;

		bool m_Changes = true;
		std::filesystem::path m_CurentPath = "";
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
		std::filesystem::path Path = "";
		std::filesystem::path RelativProjectPath = "";
		std::string PathString = "";

		AssetBrowserDataThreade() = default;
		AssetBrowserDataThreade(bool isAssset, bool isFolder, AssetHandle handle, const AssetMetadata& metadata, AssetType type, const std::string& typeString, AssetState state, const std::string& name, const std::filesystem::path& path, const std::string& pathString)
			: IsAssset(isAssset), IsFolder(isFolder), Handle(handle), Metadata(metadata), Type(type), TypeString(typeString), State(state), Name(name), Path(path), RelativProjectPath(""), PathString(pathString), Texture(nullptr)
		{}
		AssetBrowserDataThreade(AssetBrowserDataThreade&&) = default;
		AssetBrowserDataThreade(const AssetBrowserDataThreade&) = default;
	};

	struct AssetFileDirectoryThreade
	{
		std::vector<std::filesystem::path>	Folders;
		std::vector<AssetHandle>			AssetFiles;
		std::string							FolderName;
		std::filesystem::path				FolderPath;
	};

	struct AssetRigestriyPannel
	{
		AssetHandle Handle = 0ui64;
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
	using PathRegistryThreade = std::map<std::filesystem::path, AssetHandle>;
	using DirectoryRegistryThreade = std::map<std::filesystem::path, AssetFileDirectoryThreade>;
	using ContentBrowserItemesThreade = std::vector<AssetBrowserDataThreade>;
	using RegisterItemesThreade = std::vector<AssetRigestriyPannel>;

	

	class EditorAssetManegerThreade : public AssetManagerBase
	{
	public:
		template<typename K, typename T>
		using AssetMangerMap = AssetMangerMapMutex<K, T>;

		virtual void OnAttach();
		virtual void OnDetach();

		bool IsDirectoryInRegistry(const std::filesystem::path& parentPath);

		bool IsAssetPathExtensionVaild(const std::filesystem::path& parentPath) const;
		virtual bool IsAssetHandleValid(AssetHandle handle) const override;
		virtual bool IsAssetHandleValid(const std::filesystem::path& path) const override;
		virtual bool IsAssetInteral(AssetHandle handle) const override;
		virtual bool IsAssetLoaded(const std::filesystem::path& path) const override;
		virtual bool IsAssetLoaded(AssetHandle handle) const override;

		void IsAssetHandleValidAsync(AssetHandle handle, bool* result);
		void IsAssetHandleValidAsync(const std::filesystem::path& path, bool* result);
		void IsAssetInteralAsync(AssetHandle handle, bool* result);
		void IsAssetLoadedAsync(AssetHandle handle, bool* result);
		void IsAssetLoadedAsync(const std::filesystem::path& path, bool* result);

		virtual const AssetMetadata GetMetadata(AssetHandle handle) const override;
		virtual const AssetMetadata GetMetadata(const std::filesystem::path& path) const override;
		void GetMetadataAsync(AssetHandle handle, AssetMetadata* meatdat);
		void GetMetadataAsync(const std::filesystem::path& path, AssetMetadata* meatdat);

		virtual const AssetHandle GetAssetHandle(const std::filesystem::path& path) const override;

		void GetAssetHandleAsync(const std::filesystem::path& path, AssetHandle* handle);

		virtual Ref<Asset> GetAsset(AssetHandle handle) override;
		virtual Ref<Asset> GetAssetAsync(AssetHandle handle) override;
		virtual Ref<Asset> GetAsset(const std::filesystem::path& path) override;
		virtual Ref<Asset> GetAssetAsync(const std::filesystem::path& path) override;


		virtual AssetHandle CreatLocaleAsset(Ref<Asset> asset, AssetMetadata& metadata) override;
		virtual void CreatLocaleAsset(Ref<Asset> asset, AssetMetadata& metadata, AssetHandle handle) override;

		virtual Ref<Asset> GetLocaleAsset(AssetHandle handle) override;
		virtual void DeleteLocaleAsset(AssetHandle handle) override;

		AssetHandle CreatLocaleAssetAsync(Ref<Asset>& asset, AssetMetadata& metadata);
		Ref<Asset> GetLocaleAssetAsync(AssetHandle handle);

		void DeleteLocaleAssetAsync(AssetHandle handle);

		void ScaneDirectory(const std::filesystem::path& path);
		void ReLoadeAsset(AssetHandle handle);
		void ReLoadeAsset(const std::filesystem::path& path);

		bool IsCurentAssetState(const std::filesystem::path& showPath) const;
		ContentBrowserItemesThreade GetCurentAssetInformation(const std::filesystem::path& showPath);

		bool IsCurentRegistryAssetChandge() const;
		RegisterItemesThreade GetCurentAssetRegistry();

		void EventAsyncModified(const std::filesystem::path& path);
		void EventAsyncAdded(const std::filesystem::path& path);
		void EventAsyncRemoved(const std::filesystem::path& path);
		void EventAsyncRenamedNew(const std::filesystem::path& path);
		void EventAsyncRenamedOld(const std::filesystem::path& path);

		void SerialzeAssetRegistry();
		bool DeserialzeAssetRegistry();

		void DeleateDirectory(const std::filesystem::path& path);
		void DeleateFileAsset(AssetHandle handle);
		void UnLoadeFileAsset(const AssetHandle handle);
		
		void ClearLodeadAssetList();
		static bool IsProjectDriectoryRelativ(const std::filesystem::path& assetPath);
		static bool IsProjectDriectoryAbsolut(const std::filesystem::path& assetPath);
		static size_t GetIndexProjectDriectory(const std::filesystem::path& assetPath);
		static std::filesystem::path GetFilePathProjectDirectoryAbosult(const std::filesystem::path& assetPath);
		static std::filesystem::path GetFilePathProjectDirectoryRelative(const std::filesystem::path& assetPath);
	private:

		bool ExexuteEvent(AssetHandle handle);
		inline bool IsFileAssetExist(const std::filesystem::path& path);
		bool CheckAssetFileExist(AssetHandle handle);

		void CreateAssetDirectory(const std::filesystem::path& parentPath);
		void AddAssetFileToAssetDirectory(const std::filesystem::path& assetPath, const std::string& assetName, const std::string& assetPathString, AssetHandle handle);
		void AddDirectoryToParent(const std::filesystem::path& prentPath);
		void AddDirectory(const std::filesystem::path& path);
		void CreateFileAsset(const std::filesystem::path& path);
		void CreateNewFileAsset(const std::string& name, const std::filesystem::path& path, const std::filesystem::path& pathMarker);

		void GetMetadata(AssetHandle handle, AssetMetadata* metaData, std::mutex* metaDataMutex);
		void DeleateDirectory(const std::vector<std::filesystem::path>& paths);
		void DeleateFileAsset(const std::vector<AssetHandle>& handles);

		

	private:
		AssetMangerMap<AssetHandle, Ref<Asset>>							m_LoadedAssets;
		AssetMangerMap<AssetHandle, AssetMetadata>							m_HandleRegistry;
		AssetMangerMap<std::filesystem::path, AssetHandle>					m_PathRegistry;
		AssetMangerMap<std::filesystem::path, AssetFileDirectoryThreade>	m_DirectoryRegistry;

		bool							m_FileChanges = true;
		bool							m_RegestryChanges = true;
		mutable std::mutex				m_ChangesMutex;

		uint32_t						m_CurentProzesses;
		mutable std::mutex				m_CurentProzessesMutex;

		std::filesystem::path			m_CurentPath = "";
		mutable std::mutex				m_CurentPathMutex;

		std::filesystem::path			m_BaseAssetPath = "";
		mutable std::mutex				m_BaseAssetPathMutex;

		mutable std::mutex				m_WorkingThreadMutex;
		std::vector<std::future<void>>	m_WorkingThread;
		
		
	};

#pragma endregion



}

