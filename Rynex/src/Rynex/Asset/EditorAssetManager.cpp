#include "rypch.h"
#include "EditorAssetManager.h"

#include <Rynex/Asset/Base/Asset.h>
#include <Rynex/Asset/Base/AssetManager.h>
#include <Rynex/Asset/Base/AssetImporter.h>
#include <Rynex/Project/Project.h>

#include <Rynex/Serializers/EditorAssetMangerSerializer.h>


namespace Rynex {


#pragma region AssetRegistry

	bool AssetRegistry::IsAssetInRegistry(AssetHandle handle) const
	{
		return m_HandleRegistry.find(handle) != m_HandleRegistry.end();
	}

	bool AssetRegistry::IsAssetInRegistry(const FileSystem::Path& path) const
	{
		return m_PathRegistry.find(path) != m_PathRegistry.end();
	}

	bool AssetRegistry::IsDirectoryInRegistry(const FileSystem::Path& parentPath) const
	{
		return m_DirectoryRegistry.find(parentPath) != m_DirectoryRegistry.end();
	}

	bool AssetRegistry::IsAssetPath(const FileSystem::Path& pathSystem) const
	{
	    const std::filesystem::path& path = pathSystem.GetPath();
		return path.has_extension();
	}

	bool AssetRegistry::IsAssetInteral(AssetHandle handle) const
	{
		return IsAssetInRegistry(handle) && GetMetadataConst(handle).GetIntern();
	}

	void AssetRegistry::CreateAsset(const FileSystem::Path& path, AssetHandle handle, AssetMetadata metadata, bool findDirectOnDisc)
	{
		if (!IsAssetInRegistry(path) && IsAssetPath(path))
		{
			
			if (!metadata)
			{
				metadata.SetActive(true);
				metadata.m_FilePath = path.GetPath();
				metadata.m_Name = path.GetNamePathString();
				metadata.m_Type = path.GetAssetFileType();
				metadata.m_ChangeTime = GetCurrentTimeStr();
			}

			if(findDirectOnDisc)
			{
				metadata.SetState(AssetState::NotLoaded);
			}
			else
			{
				metadata.SetState(AssetState::LostConnection);
				metadata.m_ChangeTime = GetCurrentTimeStr();
			}
			std::string extension = path.GetExtensionPathString();
		    size_t extensionCount = extension.size();
			if(extension.find("-",0) < extensionCount)
			{
				size_t pos = extension.find("-", 0);
                pos -= extensionCount;
				std::string pathStr = path.GetPathString();
			    size_t pathCount = pathStr.size();
				std::string parentStr = pathStr.substr(0, pathCount - 2);
			    FileSystem::Path parent = parentStr;
				if (IsDirectoryInRegistry(parent))
				{
					AssetFileDirectory& assetFileDirectory = m_DirectoryRegistry[parent];
					assetFileDirectory.m_Files.emplace_back(handle);
				}
				else
				{
					AssetFileDirectory& assetFileDirectory = m_DirectoryRegistry[parent];
					assetFileDirectory.m_Files.emplace_back(handle);
					assetFileDirectory.m_FolderName = parent.GetNamePathString();
					assetFileDirectory.m_FolderPath = parent;
					RY_ASSET_WARN("This Shoud Not Happend!");
				}
			}
			else
			{
				FileSystem::Path parent = path.GetParent();
				if (IsDirectoryInRegistry(parent))
				{
					AssetFileDirectory& assetFileDirectory = m_DirectoryRegistry[parent];
					assetFileDirectory.m_Files.emplace_back(handle);
				}
				else
				{
					AssetFileDirectory& assetFileDirectory = m_DirectoryRegistry[parent];
					assetFileDirectory.m_Files.emplace_back(handle);
					assetFileDirectory.m_FolderName = path.GetNamePathString();
					assetFileDirectory.m_FolderPath = parent;
					RY_ASSET_WARN("This Shoud Not Happend!");
				}
			}
			

			m_HandleRegistry[handle] = metadata;
			m_PathRegistry[path] = handle;
		}
		else if (IsAssetInRegistry(path) && findDirectOnDisc)
		{
			AssetMetadata& metadata = m_HandleRegistry[GetAssetHandle(path)];
			metadata.SetState(AssetState::NotLoaded);
		}
		m_Changes = true;
	}

	AssetHandle AssetRegistry::CreatLocaleAsset(Ref<Asset>& asset, AssetMetadata& metadata)
	{
		while (UUID::Zero() == asset->Handle && IsAssetInRegistry(asset->Handle))
		{
			asset->Handle = AssetHandle();
		}

		metadata.SetIntern(true);

		metadata.SetState(AssetState::Ready);
		AssetHandle handle = asset->Handle;
		m_HandleRegistry[handle] = metadata;

		return handle;
	}


	bool AssetRegistry::AddDirectoryToParent(const FileSystem::Path& path)
	{
	    FileSystem::Path parentPath = path.GetParent();
		if (IsDirectoryInRegistry(path))
		{
			AssetFileDirectory& assetFileDirectory = m_DirectoryRegistry[parentPath];
			for (auto& folder:  assetFileDirectory.m_Folders)
			{
				if (folder == parentPath)
					return true;

			}
			assetFileDirectory.m_Folders.emplace_back(path);
			m_Changes = true;
			return true;
		}


		AssetFileDirectory& assetFileDirectory = m_DirectoryRegistry[parentPath];
		assetFileDirectory.m_FolderName = parentPath.GetNamePathString();
		assetFileDirectory.m_FolderPath = parentPath;
		assetFileDirectory.m_Folders.emplace_back(path);
		RY_ASSET_WARN("This Should Not Happend!");
		m_Changes = true;
		return true;
	}



    AssetHandle AssetRegistry::GetAssetHandle(const FileSystem::Path& path)
	{
		if (IsAssetInRegistry(path))
		{
			return m_PathRegistry[path];
		}

		if(IsAssetPath(path) && AssetType::None != path.GetAssetFileType())
		{
			AssetHandle handle;
			do {
				handle = AssetHandle();
			} 
			while (UUID::Zero() == handle || IsAssetInRegistry(handle));
			
			CreateAsset(path, handle);
			return handle;
		}

	    return AssetHandle(0);

	}

	AssetHandle AssetRegistry::GetAssetHandleConst(const FileSystem::Path& path) const
	{

		if (!IsAssetInRegistry(path))
			return AssetHandle(0);

		AssetHandle handle = m_PathRegistry.at(path);
		return handle;
	}

	AssetMetadata& AssetRegistry::GetMetadata(AssetHandle handle)
	{
		if (IsAssetInRegistry(handle))
		{
			m_Changes = true;
			return m_HandleRegistry[handle];
		}
		AssetMetadata metadata;
		return metadata;
	}

	const AssetMetadata& AssetRegistry::GetMetadataConst(AssetHandle handle) const
	{
		if (IsAssetInRegistry(handle))
			return m_HandleRegistry.at(handle);

		AssetMetadata metadata;
		return metadata;
	}

	AssetMetadata& AssetRegistry::GetMetadata(const FileSystem::Path& path)
	{
	    AssetHandle handle = GetAssetHandle(path);
		return GetMetadata(handle);
	}

    const AssetFileDirectory& AssetRegistry::GetAssetFileDirectory(const FileSystem::Path& path)
    {
	    return m_DirectoryRegistry[path];
    }

    bool AssetRegistry::IsCurentAssetState(const FileSystem::Path& showPath) const
	{
		return m_Changes || showPath != m_CurentPath;
	}

	ContentBrowserItemes AssetRegistry::GetCurentAssetInformation(const FileSystem::Path& showPath)
	{
		const AssetFileDirectory& assetFileDirectory = GetAssetFileDirectory(showPath);
		const std::vector<FileSystem::Path>& foldersPaths = assetFileDirectory.m_Folders;
		const std::vector<AssetHandle>& handles = assetFileDirectory.m_Files;
		ContentBrowserItemes itemes;
		itemes.reserve(foldersPaths.size()+ foldersPaths.size());

		for (const FileSystem::Path& folderPath : foldersPaths)
		{
		    AssetBrowserData browserData(
		        false,
		        true,
		        AssetHandle(0),
		        AssetMetadata(),
		        AssetType::None,
		        "",
		        AssetState::None,
		        folderPath.GetNamePathString(),
		        folderPath.GetPath(),
		        folderPath.GetPathString()
            );
			itemes.emplace_back(browserData);
		}
		for (const AssetHandle handle : handles)
		{
			const AssetMetadata& metadata = GetMetadataConst(handle);
			
			AssetType type = Asset::GetAssetTypeFromFilePath(metadata.m_FilePath.filename());
			RY_CORE_ASSERT(!metadata.GetIntern());

		    AssetBrowserData assetBrowserData(
		        true,
		        false,
				handle,
				metadata,
				type,
				Asset::GetAssetTypeDragAndDropName(type),
				metadata.m_State,
				metadata.m_FilePath.filename().string(),
				metadata.m_FilePath,
				metadata.m_FilePath.string()
			);
			itemes.emplace_back(assetBrowserData);
		}
		m_Changes = false;

		m_CurentPath = showPath;
		return itemes;
	}

	std::string AssetRegistry::GetCurrentTimeStr()
	{
		std::chrono::system_clock::time_point curent = std::chrono::system_clock::now();
		std::time_t t = std::chrono::system_clock::to_time_t(curent);

		// in lokale Zeit umwandeln
		std::tm tm{};
#ifdef _WIN32
		localtime_s(&tm, &t);  // Windows
#else
		localtime_r(&t, &tm);  // Linux / macOS
#endif

		char buffer[20];
		std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm);
		return std::string(buffer);
	}

	void AssetRegistry::DeleteFolder(const FileSystem::Path& folderPath)
	{
		FileSystem::Path parent = folderPath.GetParent();
		if (IsDirectoryInRegistry(parent))
		{
			std::vector<FileSystem::Path>& fileDirectory = m_DirectoryRegistry[parent].m_Folders;
			uint32_t delIndex = fileDirectory.size() + 1;
			for (uint32_t i = 0, size = delIndex-1; i < size; i++)
			{
				if (fileDirectory[i] == folderPath)
					delIndex = i;
			}

			if (delIndex != (fileDirectory.size() + 1))
			{
				RY_CORE_ASSERT(false, "Not finely Rady!")
			}
		}
	}

	void AssetRegistry::DeleteLocaleAsset(AssetHandle handle)
	{
		while(IsAssetInteral(handle))
			m_HandleRegistry.erase(handle);
	}

#pragma endregion



#pragma region EditorAssetManagerThreade


    EditorAssetManagerThread::EditorAssetManagerThread()
        : m_LoadedAssets()
        , m_HandleRegistry()
        , m_PathRegistry()
        , m_DirectoryRegistry()

        , m_FileChanges(true)
        , m_RegistryChanges(true)
        , m_ChangesMutex()
	    , m_CurrentProcess(0)
	    , m_CurrentProcessMutex()
	    , m_CurrentPath()
	    , m_BaseAssetPathMutex()
	    , m_WorkingThreadMutex()
	    , m_WorkingThread()
    {
    }

    EditorAssetManagerThread::~EditorAssetManagerThread()
    {
    }

    void EditorAssetManagerThread::OnAttach()
	{
		RY_ASSET_WARN("Begin EditorAssetManagerThread OnAttach");
		DeserializeAssetRegistry();
	    RY_ASSET_WARN("Fished DeserializeAssetRegistry");
		const std::filesystem::path& baseDir = Project::GetActiveAssetDirectory();
		ScanDirectory(baseDir);
		RY_ASSET_INFO("Ende EditorAssetManagerThread OnAttach");
	}

	void EditorAssetManagerThread::OnDetach()
	{
		RY_ASSET_WARN("Begin EditorAssetManagerThread OnDetach");
		SerializeAssetRegistry();
		m_PathRegistry.Shutdown();
		m_LoadedAssets.Shutdown();
		m_DirectoryRegistry.Shutdown();
		m_HandleRegistry.Shutdown();

		RY_ASSET_INFO("Ende EditorAssetManagerThread OnDetach");
	}


#pragma region IsAsset

	bool EditorAssetManagerThread::IsDirectoryInRegistry(const FileSystem::Path& parentPath) const
    {
		return parentPath.IsDirectory() && m_DirectoryRegistry.IsFound(parentPath);
	}


	bool EditorAssetManagerThread::IsAssetPathExtensionValid(const FileSystem::Path& parentPath) const
	{
		return parentPath.IsFile() && AssetType::None != parentPath.GetAssetFileType();
	}

	bool EditorAssetManagerThread::IsAssetHandleValid(AssetHandle handle) const
	{
		return m_HandleRegistry.IsFound(handle);

	}

	bool EditorAssetManagerThread::IsAssetHandleValid(const FileSystem::Path& path) const
	{
		const AssetHandle handle = GetAssetHandle(path);
		return IsAssetHandleValid(handle);
	}

	bool EditorAssetManagerThread::IsAssetInteral(AssetHandle handle) const
	{
		return  IsAssetHandleValid(handle) && GetMetadata(handle).GetIntern();
	}

	bool EditorAssetManagerThread::IsAssetLoaded(const FileSystem::Path& path) const
	{
		AssetHandle handle = GetAssetHandle(path);
		return IsAssetLoaded(handle);
	}

	bool EditorAssetManagerThread::IsAssetLoaded(AssetHandle handle) const
	{
		return m_LoadedAssets.IsFound(handle);
	}


	void EditorAssetManagerThread::IsAssetHandleValidAsync(AssetHandle handle, bool* result)
	{
	    std::lock_guard<std::mutex> scope(m_WorkingThreadMutex);
	    std::function<void(AssetHandle, bool*)> lambder = [this](AssetHandle handleLem, bool* resultLem) -> void
	    {
	        *resultLem = IsAssetHandleValid(handleLem);
	    };
		m_WorkingThread.emplace_back(std::async(std::launch::async, lambder, handle, result));

	}

	void EditorAssetManagerThread::IsAssetHandleValidAsync(const FileSystem::Path& path, bool* result)
	{
		std::lock_guard<std::mutex> scope(m_WorkingThreadMutex);

	    std::function<void(FileSystem::Path, bool*)> lambder = [this](FileSystem::Path pathLem, bool* resultLem) -> void
		{
			*resultLem = IsAssetHandleValid(pathLem);
		};

		m_WorkingThread.emplace_back(std::async(std::launch::async, lambder, path, result));

	}

	void EditorAssetManagerThread::IsAssetInternalAsync(AssetHandle handle, bool* result)
	{
	    std::lock_guard<std::mutex> scope(m_WorkingThreadMutex);

		m_WorkingThread.emplace_back(std::async(std::launch::async, [&](AssetHandle handleLem, bool* resultLem)
			{
				*resultLem = IsAssetInteral(handleLem);
			}, handle, result));

	}

	void EditorAssetManagerThread::IsAssetLoadedAsync(AssetHandle handle, bool* result)
	{
	    std::lock_guard<std::mutex> scope(m_WorkingThreadMutex);
	    std::function<void(AssetHandle, bool*)> lambder = [this](AssetHandle handleLambder, bool* resultLambder) -> void
	    {
	        *resultLambder = IsAssetLoaded(handleLambder);
	    };
		m_WorkingThread.emplace_back(std::async(std::launch::async, lambder, handle, result));

	}

	void EditorAssetManagerThread::IsAssetLoadedAsync(const FileSystem::Path& path, bool* result)
	{
	    std::lock_guard<std::mutex> scope(m_WorkingThreadMutex);
	    std::function<void(FileSystem::Path, bool*)> lambder = [this](FileSystem::Path pathLambder, bool* resultLambder) -> void
	    {
	        *resultLambder = IsAssetLoaded(pathLambder);
	    };

		m_WorkingThread.emplace_back(std::async(std::launch::async, lambder, path, result));

	}

#pragma endregion


#pragma region GetData

	const AssetMetadata EditorAssetManagerThread::GetMetadata(AssetHandle handle) const
	{
		if (IsAssetHandleValid(handle))
		{
			return m_HandleRegistry.GetCopy(handle);
		}
		return AssetMetadata();
	}

	const AssetMetadata EditorAssetManagerThread::GetMetadata(const FileSystem::Path& path) const
	{
		const AssetHandle handle = GetAssetHandle(path);
		return GetMetadata(handle);
	}

	void EditorAssetManagerThread::GetMetadataAsync(AssetHandle handle, AssetMetadata* meatdata)
	{
	    std::lock_guard<std::mutex> scope(m_WorkingThreadMutex);
	    std::function<void(AssetHandle, AssetMetadata*)> lambder = [this](AssetHandle handleLambder, AssetMetadata* metadataLambder) -> void
	    {
	        *metadataLambder = GetMetadata(handleLambder);
	    };

		m_WorkingThread.emplace_back(std::async(std::launch::async, lambder, handle, meatdata));


	}

	void EditorAssetManagerThread::GetMetadataAsync(const FileSystem::Path& path, AssetMetadata* meatdata)
	{
	    std::lock_guard<std::mutex> scope(m_WorkingThreadMutex);

	    std::function<void(FileSystem::Path handleLambder, AssetMetadata* metadataLambder)> lambder = [this](FileSystem::Path handleLambder, AssetMetadata* metadataLambder) -> void
		{
			*metadataLambder = GetMetadata(handleLambder);
		};

		m_WorkingThread.emplace_back( std::async(std::launch::async, lambder, path, meatdata) );

	}

	const AssetHandle EditorAssetManagerThread::GetAssetHandle(const FileSystem::Path& path) const
	{
		static AssetHandle s_NullHandle = AssetHandle(0);
		if (m_PathRegistry.IsFound(path))
			return m_PathRegistry.GetCopy(path);

		return s_NullHandle;
	}



	void EditorAssetManagerThread::GetAssetHandleAsync(const FileSystem::Path& path, AssetHandle* handle)
	{
	    std::scoped_lock<std::mutex> scope(m_WorkingThreadMutex);
	    std::function<void(FileSystem::Path handleLambder, AssetHandle* metadataLambder)> lambder = [this](FileSystem::Path pathLambder, AssetHandle* handleLambder) -> void
	    {
	        *handleLambder = GetAssetHandle(pathLambder);
	    };
		m_WorkingThread.emplace_back( std::async(std::launch::async, lambder, path, handle) );

	}

#pragma endregion


#pragma region AssetDataMage

	Ref<Asset> EditorAssetManagerThread::GetAsset(AssetHandle handle)
	{
		if (IsAssetHandleValid(handle))
		{
			Ref<Asset> asset = nullptr;
			if (IsAssetLoaded(handle))
			{
				asset = m_LoadedAssets.GetCopy(handle);
			}
			else if (!IsAssetInteral(handle))
			{

				const AssetMetadata metadataC = GetMetadata(handle);

				if (metadataC.GetIntern())
				{
					RY_CORE_ASSERT(false, "Error on: 'EditorAssetManager::GetAsset' Asset is known In Regestry as Interale-Asset But got not Loadede! This cinde off error is a def Error");
					return nullptr;
				}

				switch (metadataC.m_State)
				{
					case AssetState::NotLoaded:
					case AssetState::LostConnection:
					{
					    asset = GetAssetLostConnection(handle);
					    break;
					}
					case AssetState::Loading:
					{
						bool lastTimeTaskExecute = true;
						do {
							if (Asset::CurrentOnMainThread() || !lastTimeTaskExecute)
							{
								using namespace std::chrono_literals;
								std::this_thread::sleep_for(100ms);
								RY_ASSET_WARN("Wait For Asset To Loade");
							}
							else
							{
								lastTimeTaskExecute = Application::Get().ExecuteTaskFromThread();
							}
						} while (AssetState::Loading == GetMetadata(handle).m_State);

						asset = GetAsset(handle);
						break;

					}


					case AssetState::Ready:
					{
						asset = GetAsset(handle);
						break;
					}
					case AssetState::Error:
					{
						return nullptr;
					}
					default:
					{
						RY_CORE_ASSERT(false, "State Not Defined!");
						break;
					}

				}


			}
			else if (IsAssetInteral(handle))
			{
				RY_CORE_ASSERT(false, "Error on: 'EditorAssetManager::GetAsset' Asset is known In Regestry as Interale-Asset But got not Loadede! This cinde off error is a def Error");
				return nullptr;
			}
			else
			{
				RY_CORE_ASSERT(false, "Error on: 'EditorAssetManager::GetAsset' I Dont't know some isuse kan go to hear!");
				return nullptr;
			}
			return asset;
		}


		return nullptr;
	}

	Ref<Asset> EditorAssetManagerThread::GetAssetAsync(AssetHandle handle)
	{
		return GetAsset(handle);
	}

	Ref<Asset> EditorAssetManagerThread::GetAsset(const FileSystem::Path& path)
	{
		if (!IsAssetHandleValid(path))
			CreateFileAsset(path);

		const AssetHandle handle = GetAssetHandle(path);

		if (IsFileAssetExist(path))
		{
			SetAssetMetadataLockState(handle, std::bind(
			    &EditorAssetManagerThread::SetAssetMetadataStateNotLoaded,
			    this,
			    std::placeholders::_1)
			);

		}
		return GetAsset(handle);
	}

	Ref<Asset> EditorAssetManagerThread::GetAssetAsync(const FileSystem::Path& path)
	{
		if (!IsAssetHandleValid(path))
			CreateFileAsset(path);
		const AssetHandle handle = GetAssetHandle(path);
		return GetAsset(handle);
	}


#pragma region LocaleAssetsDataMage

	void EditorAssetManagerThread::CreatLocaleAsset(Ref<Asset> asset, AssetMetadata& metadata, AssetHandle handle)
	{
		metadata.SetIntern(true);
		metadata.SetState(AssetState::Ready);

		std::lock_guard<std::mutex> lockChanges(m_ChangesMutex);
		m_RegistryChanges = true;

		m_HandleRegistry.Add(handle, metadata);
		m_LoadedAssets.Add(handle,asset);

	}

	AssetHandle EditorAssetManagerThread::CreatLocaleAsset(Ref<Asset> asset, AssetMetadata& metadata)
	{
		while (IsAssetHandleValid(asset->Handle))
			asset->Handle = AssetHandle();
		AssetHandle handle = asset->Handle;

		CreatLocaleAsset(asset, metadata, handle);
		if (IsAssetInteral(handle))
			return handle;

		RY_CORE_ASSERT(false, "funktion return 0 as AssetHandle");
		return AssetHandle(0);
	}

	Ref<Asset> EditorAssetManagerThread::GetLocaleAsset(AssetHandle handle)
	{
		RY_CORE_ASSERT(false, "This funktion is not any longer Sepoted! Use the funktion GetAsset");
		if (IsAssetLoaded(handle) && !IsAssetHandleValid(handle))
		{
			return m_LoadedAssets.GetCopy(handle);
		}
		return nullptr;
	}

	void EditorAssetManagerThread::DeleteLocaleAsset(AssetHandle handle)
	{
		if (IsAssetLoaded(handle) && IsAssetInteral(handle))
		{
			std::lock_guard<std::mutex> lockChanges(m_ChangesMutex);

			m_LoadedAssets.Remove(handle);
			m_HandleRegistry.Remove(handle);
			m_RegistryChanges = true;
		}
	}


#pragma endregion


#pragma endregion


#pragma region DiscFile

	void EditorAssetManagerThread::ScanDirectory(const FileSystem::Path& curent)
	{

		if (curent.IsEmpty())
			return;

	    const std::filesystem::path& curentPath = curent.GetPath();
		for (const std::filesystem::directory_entry& p : std::filesystem::directory_iterator(curentPath))
		{
			FileSystem::Path path(p.path());
			if (p.is_directory())
			{
				AddDirectoryToParent(path);
				ScanDirectory(path);
			}
			else
			{
				if (AssetType::None != path.GetAssetFileType())
				{
					CreateFileAsset(path);
				}
			}
		}

	    SetPathUpdateContentBrowser(curent);
	}

	void EditorAssetManagerThread::ReLodeAsset(AssetHandle handle)
	{
		RY_CORE_ASSERT(AssetHandle::Zero() != handle, "This Handle is Unacceptable");
		if (IsAssetLoaded(handle))
		{
			std::chrono::microseconds waitTime = std::chrono::milliseconds(30);
			std::this_thread::sleep_for(waitTime);
			{
				int about = 0;
			    SetAssetMetadataLockState(handle, std::bind(&EditorAssetManagerThread::SetAssetMetadataStateUpdating, this, &about, std::placeholders::_1));
				if (about == 1)
				{
					RY_ASSET_ERROR("About Reloading! Already Reloading somewhere else!");
					return;
				}
				RY_ASSET_WARN("Begin reloading Asset");
			}


			const AssetMetadata metadataC = GetMetadata(handle);
			bool result = false;
			for (uint32_t i = 0; i < 3 && !result; i++)
			{

				waitTime = std::chrono::milliseconds(i * 45);
				if(waitTime != std::chrono::milliseconds(0))
					std::this_thread::sleep_for(waitTime);
				result = AssetImporter::ReLoadAsset(handle, metadataC);
			}

			if(result)
			    SetAssetMetadataLockState(handle, std::bind(&EditorAssetManagerThread::SetAssetMetadataStateReady, this, std::placeholders::_1));
			else
			    SetAssetMetadataLockState(handle, std::bind(&EditorAssetManagerThread::SetAssetMetadataStateError, this, std::placeholders::_1));


			RY_ASSET_INFO("Asset: {0} Is Now Reloaded", GetMetadata(handle).m_FilePath.string().c_str());
		}
		else
		{
			RY_ASSET_INFO("don't need Reloading Asset: {0}", GetMetadata(handle).m_FilePath.string().c_str());
		}
	}

	void EditorAssetManagerThread::ReLoadAsset(const FileSystem::Path& path)
	{
		const AssetHandle handle = GetAssetHandle(path);
		ReLodeAsset(handle);
	}

	void EditorAssetManagerThread::GetMetadata(AssetHandle handle, AssetMetadata* metaData, std::mutex* metaDataMutex)
	{
		RY_CORE_ASSERT(false);
	}

	void EditorAssetManagerThread::DeleteDirectory(const std::vector<FileSystem::Path>& foldersPaths)
	{
		for (const FileSystem::Path& dirPath : foldersPaths)
		{
			DeleteDirectory(dirPath);
		}
	}

	void EditorAssetManagerThread::DeleteFileAsset(const std::vector<AssetHandle>& handles)
	{
		for (const AssetHandle handle : handles)
		{
			DeleteFileAsset(handle);
		}
	}

    Ref<Asset> EditorAssetManagerThread::GetAssetLostConnection(AssetHandle handle)
    {
        SetAssetMetadataLockState(handle, std::bind(&EditorAssetManagerThread::SetAssetMetadataStateLoading, this, std::placeholders::_1));

	    Ref<Asset> asset = AssetImporter::ImportAsset(handle, GetMetadata(handle));

	    if (asset)
	    {
	        SetAssetMetadataLockState(handle, std::bind(&EditorAssetManagerThread::SetAssetMetadataStateReady, this, std::placeholders::_1));
	        asset->Handle = handle;

	        m_LoadedAssets.Add(handle, asset);
	        std::lock_guard<std::mutex> lockChange(m_ChangesMutex);
	        m_RegistryChanges = true;
	    }
	    else
	    {
	        RY_CORE_ERROR("'EditorAssetManager::GetAsset' No Asset Loaded! Empty Resource");
	        SetAssetMetadataLockState(handle, std::bind(&EditorAssetManagerThread::SetAssetMetadataStateError, this, std::placeholders::_1));
	    }
	    return asset;
    }

    void EditorAssetManagerThread::SetAssetMetadataLockState(AssetHandle handle, std::function<void(AssetMetadata&)> lambder)
    {
	    std::lock_guard<std::mutex> lockPath(m_CurrentPathMutex);
	    std::lock_guard<std::mutex> lockChange(m_ChangesMutex);
	    m_HandleRegistry.GetRefLemda(lambder, handle);
    }

    void EditorAssetManagerThread::SetAssetMetadataUpdateContentBrowser(AssetMetadata& metadata)
    {
	    const FileSystem::Path& path = metadata.m_Path;
	    SetPathUpdateContentBrowser(path);
    }

    void EditorAssetManagerThread::SetPathUpdateContentBrowser(const FileSystem::Path& path)
	{
	    FileSystem::Path parent = path.GetParent();
	    if(m_CurrentPath == parent)
	        m_FileChanges = true;
	    m_RegistryChanges = true;
	}

    void EditorAssetManagerThread::SetAssetMetadataStateNotLoaded(AssetMetadata& metadata)
    {
	    SetAssetMetadataUpdateContentBrowser(metadata);
	    metadata.SetState(AssetState::NotLoaded);
    }

    void EditorAssetManagerThread::SetAssetMetadataStateLoading(AssetMetadata& metadata)
    {
	    SetAssetMetadataUpdateContentBrowser(metadata);
	    metadata.SetState(AssetState::Loading);

    }

    void EditorAssetManagerThread::SetAssetMetadataStateError(AssetMetadata& metadata)
    {
	    SetAssetMetadataUpdateContentBrowser(metadata);
	    metadata.SetState(AssetState::Error);

    }

    void EditorAssetManagerThread::SetAssetMetadataStateReady(AssetMetadata& metadata)
    {
	    SetAssetMetadataUpdateContentBrowser(metadata);
	    metadata.SetState(AssetState::Ready);
	    metadata.m_LoadingInTime = std::chrono::steady_clock::now();
	    RY_CORE_TRACE("AssetState: (Ready) {}", metadata.m_Name);
    }

    void EditorAssetManagerThread::SetAssetMetadataStateUpdating(int* aboutPtr, AssetMetadata& metadata)
    {
	    if (AssetState::Updating == metadata.m_State)
	        *aboutPtr = 1;
	    else
	        metadata.SetState(AssetState::Updating);
    }

    void EditorAssetManagerThread::SetAssetMetadataStateLostConnection(AssetMetadata& metadata)
    {
	    SetAssetMetadataUpdateContentBrowser(metadata);
	    metadata.SetState(AssetState::LostConnection);

	    RY_CORE_TRACE("AssetState: (LostConnection) {}", metadata.m_Name);
    }


    void EditorAssetManagerThread::ClearLoadedAssetList()
	{
	    using Map = std::map<AssetHandle, Ref<Asset>>;

		Map mapCopy = m_LoadedAssets.Read<Map>(
		[](const Map& map)-> Map
			{
				return map;
			}
		);

		for (auto&[assetHandle, assetRef] : mapCopy)
		{
			this->UnLodeFileAsset(assetHandle);
		}
	}


	void EditorAssetManagerThread::SerializeAssetRegistry()
	{
		RY_ASSET_INFO("SerializeAsseRegistry Asset Registry");

		m_HandleRegistry.Write(
			[](std::map<AssetHandle, AssetMetadata>& map)
			{
				std::filesystem::path path = Project::GetActiveAssetRegistryPath();
				EditorAssetMangerSerializer::SerilzeThread(path, &map);
			}
		);
	}

	bool EditorAssetManagerThread::DeserializeAssetRegistry()
	{
		RY_ASSET_INFO("Deserialize AssetRegistry Asset Registry");

			m_HandleRegistry.Write(
				[this](std::map<AssetHandle, AssetMetadata>& mapHandleRegister)
				{

					m_PathRegistry.Write(
						[this, &mapHandleRegister](std::map<FileSystem::Path, AssetHandle>& mapPathRegister)
						{
							const std::filesystem::path path = Project::GetActiveAssetRegistryPath();
							EditorAssetMangerSerializer::DeserilzeThread(path, &mapHandleRegister, &mapPathRegister);
						}
					);

				}
			);
		return true;
	}


	bool EditorAssetManagerThread::ExecuteEvent(AssetHandle handle)
	{
		constexpr int64_t MAX_WAIT_TIME_LODING_AGAIN_SEC = 1500;
		if (handle)
		{
			if(IsAssetLoaded(handle))
			{
				const AssetMetadata metadata = GetMetadata(handle);
				AssetState state = metadata.m_State;
				std::chrono::steady_clock::time_point loadtime = metadata.m_LoadingInTime;
				switch (state)
				{
				case Rynex::AssetState::Loading:
				case Rynex::AssetState::Updating:
				case Rynex::AssetState::Uploading:
					return false;
				case Rynex::AssetState::LostConnection:
				case Rynex::AssetState::Error:
					RY_ASSET_INFO("Maybe Connection to {} is Now longer lost!", metadata.m_FilePath.string().c_str());
				case Rynex::AssetState::Ready:
				default:
					if (loadtime != std::chrono::steady_clock::time_point::min())
					{
						std::chrono::steady_clock::time_point curent = std::chrono::steady_clock::now();
						int64_t differenc = std::chrono::duration_cast<std::chrono::milliseconds>(curent - loadtime).count();
						return MAX_WAIT_TIME_LODING_AGAIN_SEC < differenc;
					}
					break;
				}
				return false;
			}
			else
			{
				return false;
			}
		}
		return false;
	}

	bool EditorAssetManagerThread::IsFileAssetExist(const FileSystem::Path& path)
	{
		if (path.IsRelative())
		{
		    const std::filesystem::path& pathSystem = path.GetPath();
			RY_CORE_WARN("Maby the File-Path is not absolut \n\t{}", pathSystem);
		}
		return path.IsExisting();
	}

	bool EditorAssetManagerThread::IsCurrentAssetState(const FileSystem::Path& showPath) const
	{
		return m_FileChanges || showPath != m_CurrentPath;
	}

	ContentBrowserItemesThreade EditorAssetManagerThread::GetCurrentAssetInformation(const FileSystem::Path& showPath)
	{
		if (IsDirectoryInRegistry(showPath))
		{
			ContentBrowserItemesThreade itemes;
			{
				m_DirectoryRegistry.Read<bool>(
					[this, &itemes, &showPath](const std::map<FileSystem::Path, AssetFileDirectoryThreade>& assetBrowserDataThread) -> bool
					{
						const AssetFileDirectoryThreade& assetFileDirectory = assetBrowserDataThread.at(showPath);
						const std::vector<FileSystem::Path>& foldersPaths = assetFileDirectory.Folders;
						const std::vector<AssetHandle>& handles = assetFileDirectory.AssetFiles;

					    size_t count = foldersPaths.size() + foldersPaths.size();
						itemes.reserve(count);

						for (const FileSystem::Path& folderPath : foldersPaths)
						{
						    AssetBrowserDataThreade assetBrowserDataThreade(
						        false,
						        true,
						        AssetHandle(0),
						        AssetMetadata(),
						        AssetType::None,
						        "",
								AssetState::None,
								folderPath.GetNamePathString(),
								folderPath,
								folderPath.GetPathString()
							);
							itemes.emplace_back(assetBrowserDataThreade);
						}

						for (const AssetHandle handle : handles)
						{
							if (!IsAssetHandleValid(handle))
								continue;
							CheckAssetFileExist(handle);
							const AssetMetadata metadata = GetMetadata(handle);

							AssetType type = Asset::GetAssetTypeFromFilePath(metadata.m_FilePath);

							RY_CORE_ASSERT(!metadata.GetIntern());

						    AssetBrowserDataThreade assetBrowserDataThreade(
                                true,
                                false,
                                handle,
                                metadata,
                                type,
                                Asset::GetAssetTypeDragAndDropName(type),
                                metadata.m_State,
                                metadata.m_Path.GetNamePathString(),
                                metadata.m_Path,
                                metadata.m_Path.GetPathString()
                            );

							itemes.emplace_back(assetBrowserDataThreade);
						}

						return true;
					}
				);

			}
			{
				std::lock_guard<std::mutex> lockChange(m_ChangesMutex);
				std::lock_guard<std::mutex> lockPath(m_CurrentPathMutex);
				m_FileChanges = false;
			    m_RegistryChanges = false;
				m_CurrentPath = showPath;
			}

			return itemes;

		}

		return ContentBrowserItemesThreade();
	}

	bool EditorAssetManagerThread::IsCurrentRegistryAssetChange() const
	{
		std::lock_guard<std::mutex> lockChange(m_ChangesMutex);
		return m_RegistryChanges;
	}

	RegisterItemesThreade EditorAssetManagerThread::GetCurrentAssetRegistry()
	{
		RegisterItemesThreade itemes;
		{
			m_HandleRegistry.Read<bool>(
				[&itemes](const std::map<AssetHandle, AssetMetadata>& map) -> bool
				{
					itemes.reserve(map.size());

					for (const auto& [handle, metadata] : map)
					{
						RY_CORE_ASSERT(handle, "Handle withe zero!");
						itemes.emplace_back(handle, metadata);
					}
					return true;
				}
			);

		}

		return itemes;
	}

	void EditorAssetManagerThread::EventAsyncModified(const FileSystem::Path& path)
	{
		const AssetHandle handle = GetAssetHandle(path);

		if (ExecuteEvent(handle))
		{
			m_WorkingThreadMutex.lock();
			m_WorkingThread.emplace_back(std::async(std::launch::async, [this](AssetHandle handleLem)->void {
				const AssetMetadata metadata = GetMetadata(handleLem);
				RY_ASSET_WARN("Execute Modified Event -> Asset: '{}'", metadata.m_FilePath.string().c_str());
				ReLodeAsset(handleLem);
			}, handle));
			m_WorkingThreadMutex.unlock();
		}
		else
		{
			const AssetMetadata metadata = GetMetadata(handle);
			RY_ASSET_WARN("Blocked Modified Event -> Asset: '{}'", metadata.m_FilePath.string().c_str());
		}

	}

	void EditorAssetManagerThread::EventAsyncAdded(const FileSystem::Path& path)
	{
		m_WorkingThreadMutex.lock();
		m_WorkingThread.emplace_back(std::async(std::launch::async, [this](const FileSystem::Path pathLem)
			{
				std::filesystem::path parent = pathLem.GetParentPath();
		        std::string name = pathLem.GetNamePathString();
				RY_ASSET_WARN("Execute Added Event -> Path: '{}' | Scan Dir: '{}'", name, parent);
				ScanDirectory(parent);
			}, path));
		m_WorkingThreadMutex.unlock();
	}

	void EditorAssetManagerThread::EventAsyncRemoved(const FileSystem::Path& path)
	{
		RY_ASSET_WARN("Event Removed Thread is doing nothing! TODO!");
	}

	void EditorAssetManagerThread::EventAsyncRenamedNew(const FileSystem::Path& path)
	{
		RY_ASSET_WARN("Event RenamedNew Thread is doing nothing! TODO!");
	}

	void EditorAssetManagerThread::EventAsyncRenamedOld(const FileSystem::Path& path)
	{
		RY_ASSET_WARN("Event RenamedOld Thread is doing nothing! TODO!");
	}

	bool EditorAssetManagerThread::CheckAssetFileExist(AssetHandle handle)
	{
		AssetMetadata metaData = m_HandleRegistry.GetCopy(handle);
		if (!metaData)
			return false;

		const AssetState& state = metaData.m_State;
		if (!IsAssetPathExtensionValid(metaData.m_FilePath))
		{
			 SetAssetMetadataLockState(handle, std::bind(&EditorAssetManagerThread::SetAssetMetadataStateError, this, std::placeholders::_1));;
		}
		else if ((state == AssetState::LostConnection || state == AssetState::Error) && !metaData.m_Path.IsExisting())
		{
		    SetAssetMetadataLockState(handle, std::bind(&EditorAssetManagerThread::SetAssetMetadataStateLostConnection, this, std::placeholders::_1));
		}
		return true;
	}

	void EditorAssetManagerThread::CreateAssetDirectory(const FileSystem::Path& path)
	{
		RY_CORE_ASSERT(!IsDirectoryInRegistry(path), "Found folder in register already or folder path is a file!");

		std::filesystem::path folderRelativePath = path.GetRelativePath();
		std::string name = folderRelativePath.filename().string();


		std::lock_guard<std::mutex> lockChang(m_ChangesMutex);
		std::lock_guard<std::mutex> lockPath(m_CurrentPathMutex);
		if (m_CurrentPath == folderRelativePath)
			m_FileChanges = true;

		m_RegistryChanges = true;


		AssetFileDirectoryThreade assetFileDirectory;
		assetFileDirectory.FolderName = name;
		assetFileDirectory.FolderPath = folderRelativePath;

		m_DirectoryRegistry.Add(folderRelativePath, assetFileDirectory);

		RY_ASSET_INFO("Add AssetFolder Names {} on location {}", name, folderRelativePath);
	}

	void EditorAssetManagerThread::AddAssetFileToAssetDirectory(const FileSystem::Path& assetPath, AssetHandle handle)
	{
		FileSystem::Path parent = assetPath.GetParent();
		std::string extension = assetPath.GetExtensionPathString();
		if (extension.find("-", 0) < extension.size())
		{
		    std::string assetName = assetPath.GetNamePathString();
		    std::string assetPathString = assetPath.GetExtensionPathString();
		    size_t count = assetPathString.size() - 2;
			RY_ASSET_WARN("Detected special file Extension on Asset {} on location {}", assetName, assetPathString);
			parent = assetPathString.substr(0,count );
		}
		else
		{
			parent = assetPath.GetParent();
		}

		if (!IsAssetHandleValid(handle))
			CreateFileAsset(parent);


		std::lock_guard<std::mutex> lockChang(m_ChangesMutex);

		m_FileChanges = true;
		m_RegistryChanges = true;
		if(!m_DirectoryRegistry.IsFound(parent))
			m_DirectoryRegistry.Add(parent, AssetFileDirectoryThreade());

		m_DirectoryRegistry.GetRefLemda([handle](AssetFileDirectoryThreade& assetFileDirectory)
			{
				for (auto& files : assetFileDirectory.AssetFiles)
				{
					if (files == handle)
						return;
				}
				assetFileDirectory.AssetFiles.emplace_back(handle);
			}, parent);


	}

	void EditorAssetManagerThread::AddDirectoryToParent(const FileSystem::Path& path)
	{
	    FileSystem::Path fileSystemPath(path);


		if (!IsDirectoryInRegistry(path))
		{
			CreateAssetDirectory(path);
		}
	    FileSystem::Path parentFolder = path.GetParent();

		if (IsDirectoryInRegistry(parentFolder))
		{
		    std::lock_guard<std::mutex> lockPath(m_CurrentPathMutex);
		    std::lock_guard<std::mutex> lockChange(m_ChangesMutex);

			m_DirectoryRegistry.Write(
			    std::bind(&EditorAssetManagerThread::AddDirectory, this, path, parentFolder, std::placeholders::_1)
			);
		}
		else
		{
			CreateAssetDirectory(parentFolder);
		}
	}

    void EditorAssetManagerThread::AddDirectory(const FileSystem::Path& folder, const FileSystem::Path& parentFolder, std::map<FileSystem::Path, AssetFileDirectoryThreade>& folderMap)
    {
	    auto it = folderMap.find(parentFolder);
	    if (it == folderMap.end())
	        return;
	    AssetFileDirectoryThreade& assetFileDirectory = it->second;
	    for (auto& folderChildren : assetFileDirectory.Folders)
	    {
	        if (folderChildren == parentFolder)
	            return;
	    }

	    assetFileDirectory.Folders.emplace_back(folder);
	    SetPathUpdateContentBrowser(parentFolder);

	    std::string parentFolderStr = folder.GetNamePathString();
	    const std::filesystem::path& filePath = parentFolder.GetPath();
	    RY_CORE_TRACE("Add Child Folder {} to Folder {}", parentFolderStr, filePath);
    }


    void EditorAssetManagerThread::CreateFileAsset(const FileSystem::Path& path)
	{
		if (!IsAssetHandleValid(path) && IsAssetPathExtensionValid(path))
		{
			CreateNewFileAsset(path);
		}
		else if (IsAssetHandleValid(path))
		{

			if (!IsFileAssetExist(path))
			{
				const AssetHandle handle = GetAssetHandle(path);
				if (IsAssetLoaded(handle))
					UnLodeFileAsset(handle);

			    SetAssetMetadataLockState(handle, std::bind(&EditorAssetManagerThread::SetAssetMetadataStateError, this, std::placeholders::_1));
			}
			else
			{

				const AssetHandle handle = GetAssetHandle(path);
				if (IsAssetLoaded(handle))
				{
				    m_HandleRegistry.GetRefLemda([](AssetMetadata& metaData)-> void
					{
					    RY_CORE_FATAL_IF(AssetState::Ready == metaData.m_State || AssetState::Loading == metaData.m_State, "Interesting Stat is not Ready or Current Loading, but is stored in LoadedAssets Map! \nMaby Youst Asyc Action");
					}, handle);

				}
				else
				{
					SetAssetMetadataLockState(handle, std::bind(&EditorAssetManagerThread::SetAssetMetadataStateNotLoaded, this, std::placeholders::_1));
				}

				AddAssetFileToAssetDirectory(path, handle);
			}


		}
		else if (!IsAssetPathExtensionValid(path))
		{
		    std::filesystem::path assetPath = path.GetPath();
		    std::filesystem::path extension = path.GetExtensionPath();
			RY_ASSET_WARN("Asset {} we don't add to registry because extend {} is not AssetType", assetPath, extension);
		}
		else
		{
			RY_CORE_ASSERT(false, "Don't know how that can happen");
		}
	}

	void EditorAssetManagerThread::CreateNewFileAsset(const FileSystem::Path& path)
	{
		AssetMetadata metaData;
		metaData.SetActive(true);
		metaData.SetFilePath(path);
		metaData.m_Name = path.GetNamePathString();
		metaData.m_Type = path.GetAssetFileType();
		metaData.m_ChangeTime = AssetRegistry::GetCurrentTimeStr();
	    AssetState state = IsFileAssetExist(path) ? AssetState::NotLoaded : AssetState::LostConnection;
		metaData.SetState(state);

		AssetHandle handle;
		while (UUID::Zero() == handle || IsAssetHandleValid(handle))
			handle = AssetHandle();

		std::string pathString = path.GetPathString();
	    std::string nameStr = path.GetNamePathString();
		AddAssetFileToAssetDirectory(path, handle);

		{
			std::lock_guard<std::mutex> lock(m_ChangesMutex);
			std::lock_guard<std::mutex> lockPath(m_CurrentPathMutex);
		    FileSystem::Path parent = path.GetParentPath();
			if (m_CurrentPath == parent)
				m_FileChanges = true;

			m_RegistryChanges = true;
		}
		m_HandleRegistry.Add(handle, metaData);
		m_PathRegistry.Add(metaData.m_AbsolutePath, handle);
		m_PathRegistry.Add(metaData.m_RelativePath, handle);

		if(m_PathRegistry.IsFound(metaData.m_FilePath))
			m_PathRegistry.Add(metaData.m_FilePath, handle);
	}

	void EditorAssetManagerThread::DeleteDirectory(const FileSystem::Path& path)
	{

		if (IsDirectoryInRegistry(path))
		{
			AssetFileDirectoryThreade dirRegT = m_DirectoryRegistry.GetCopy(path);
			DeleteDirectory(dirRegT.Folders);
			DeleteFileAsset(dirRegT.AssetFiles);

			SetPathUpdateContentBrowser(path);
		}
	}

	void EditorAssetManagerThread::DeleteFileAsset(AssetHandle handle)
	{
		if (IsAssetHandleValid(handle))
		{
			if (IsAssetLoaded(handle))
				UnLodeFileAsset(handle);
			const AssetMetadata metadata = m_HandleRegistry.GetCopy(handle);
			if (metadata.GetDisc())
			{
			    const FileSystem::Path& pathKey = metadata.m_Path;
				m_PathRegistry.Remove(pathKey);

				if (m_PathRegistry.IsFound(pathKey))
					m_PathRegistry.Remove(pathKey);


			    RY_CORE_TRACE("Remove Asset: {} {}", metadata.m_Name, handle);
			}
			m_HandleRegistry.Remove(handle);

			std::lock_guard<std::mutex> lockChange(m_ChangesMutex);
		    std::lock_guard<std::mutex> lockPath(m_CurrentPathMutex);
			m_FileChanges = true;
			m_RegistryChanges = true;
		}
	}

	void EditorAssetManagerThread::UnLodeFileAsset(const AssetHandle handle)
	{
		if (!IsAssetHandleValid(handle) || !IsAssetLoaded(handle))
			return;
		std::lock_guard<std::mutex> lockChang(m_ChangesMutex);
		std::lock_guard<std::mutex> lockPath(m_CurrentPathMutex);
	    SetAssetMetadataLockState(handle, std::bind(&EditorAssetManagerThread::SetAssetMetadataStateNotLoaded, this, std::placeholders::_1));
		Ref<Asset> assetRefCopy = m_LoadedAssets.GetCopy(handle);
		Weak<Asset> assetWeakCopy = assetRefCopy;
		RY_DESTROY_REF(assetRefCopy);

		m_LoadedAssets.Remove(handle);
		
		if (m_LoadedAssets.IsFound(handle))
		{
			RY_CORE_FATAL("Unloading Failed its still in the Loaded AssetList");
		}
		if (Ref<Asset> assetWeakCopyRef = assetWeakCopy.lock())
		{
			RY_CORE_FATAL("Unloading Failed Asset Still exist some wear else!");
		}
		

	}

#pragma endregion

#pragma endregion



#pragma endregion

	// template<>
	// inline AssetMangerMapMutex<AssetHandle, Ref<Asset>>::~AssetMangerMapMutex()
	// {
	// 	RY_CORE_ASSERT(!m_OutSideScope, "Mutex is alrady Set Globle");
	// 	std::unique_lock writerLock(m_Mutex);
	// 	m_Stop = true;
	//
	// 	for (auto& [key, asset] : m_AssetMap)
	// 	{
	// 		RY_DESTROY_REF(asset);
	// 	}
	// 	m_AssetMap.clear();
	//
	// }


	

	// template<>
	// inline void AssetMangerLeftRightMap<AssetHandle, Ref<Asset>>::Shoutdown()
	// {
	// 	std::unique_lock writerLock(m_WriterMutex);
	// 	m_Stop = true;
	// 	WriteAction(
	// 		[this](Map& map)
	// 		{
	// 			for (auto& [key, asset] : map)
	// 			{
	// 				RY_DESTROY_REF(asset);
	// 			}
	// 			map.clear();
	// 		}
	// 	);
	// 
	// 	
	// }
}