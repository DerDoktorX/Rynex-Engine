#include "rypch.h"
#include "EditorAssetManager.h"

#include <Rynex/Asset/Base/Asset.h>
#include <Rynex/Asset/Base/AssetManager.h>
#include <Rynex/Asset/Base/AssetImporter.h>
#include <Rynex/Project/Project.h>

#include <Rynex/Serializers/EditorAssetMangerSerialzation.h>


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
				metadata.FilePath = path.GetPath();
				metadata.Name = path.GetNamePathString();
				metadata.Type = path.GetAssetFileType();
				metadata.State = AssetState::NotLoaded;
				metadata.ChangeTime = GetCurrentTimeStr();
			}

			if(findDirectOnDisc)
			{
				metadata.State = AssetState::NotLoaded;
			}
			else
			{
				metadata.State = AssetState::LostConection;
				metadata.ChangeTime = GetCurrentTimeStr();
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
			metadata.State = AssetState::NotLoaded;
		}
		m_Changes = true;
	}

	AssetHandle AssetRegistry::CreatLocaleAsset(Ref<Asset>& asset, AssetMetadata& metadata)
	{
		while (asset->Handle == 0 && IsAssetInRegistry(asset->Handle))
		{
			asset->Handle = AssetHandle();
		}

		metadata.SetIntern(true);

		metadata.State = AssetState::Ready;
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
			while (handle == 0 || IsAssetInRegistry(handle));
			
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
			
			AssetType type = Asset::GetAssetTypeFromFilePath(metadata.FilePath.filename());
			RY_CORE_ASSERT(!metadata.GetIntern());

		    AssetBrowserData assetBrowserData(
		        true,
		        false,
				handle,
				metadata,
				type,
				Asset::GetAssetTypeDragAndDropName(type),
				metadata.State,
				metadata.FilePath.filename().string(),
				metadata.FilePath,
				metadata.FilePath.string()
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


	void EditorAssetManegerThreade::OnAttach()
	{
		RY_ASSET_WARN("Begin EditorAssetManegerThreade OnAttach");
		DeserialzeAssetRegistry();
		const std::filesystem::path& baseDir = Project::GetActiveAssetDirectory();
		ScaneDirectory(baseDir);
		RY_ASSET_INFO("Ende EditorAssetManegerThreade OnAttach");
	}

	void EditorAssetManegerThreade::OnDetach()
	{
		RY_ASSET_WARN("Begin EditorAssetManegerThreade OnDetach");
		SerialzeAssetRegistry();
		m_PathRegistry.Shutdown();
		m_LoadedAssets.Shutdown();
		m_DirectoryRegistry.Shutdown();
		m_HandleRegistry.Shutdown();
		
		RY_ASSET_INFO("Ende EditorAssetManegerThreade OnDetach");
	}


#pragma region IsAsset

	bool EditorAssetManegerThreade::IsDirectoryInRegistry(const FileSystem::Path& parentPath)
	{
		return parentPath.IsDirectory() && m_DirectoryRegistry.IsFound(parentPath);
	}


	bool EditorAssetManegerThreade::IsAssetPathExtensionVaild(const FileSystem::Path& parentPath) const
	{
		return parentPath.IsFile() && AssetType::None != parentPath.GetAssetFileType();
	}

	bool EditorAssetManegerThreade::IsAssetHandleValid(AssetHandle handle) const
	{
		return m_HandleRegistry.IsFound(handle);

	}

	bool EditorAssetManegerThreade::IsAssetHandleValid(const FileSystem::Path& path) const
	{
		const AssetHandle handle = GetAssetHandle(path);
		return IsAssetHandleValid(handle);
	}

	bool EditorAssetManegerThreade::IsAssetInteral(AssetHandle handle) const
	{
		return  IsAssetHandleValid(handle) && GetMetadata(handle).GetIntern();
	}

	bool EditorAssetManegerThreade::IsAssetLoaded(const FileSystem::Path& path) const
	{
		AssetHandle handle = GetAssetHandle(path);
		return IsAssetLoaded(handle);
	}

	bool EditorAssetManegerThreade::IsAssetLoaded(AssetHandle handle) const
	{
		return m_LoadedAssets.IsFound(handle);
	}


	void EditorAssetManegerThreade::IsAssetHandleValidAsync(AssetHandle handle, bool* result)
	{
	    std::lock_guard<std::mutex> scope(m_WorkingThreadMutex);
	    std::function<void(AssetHandle, bool*)> lambder = [this](AssetHandle handleLem, bool* resultLem) -> void
	    {
	        *resultLem = IsAssetHandleValid(handleLem);
	    };
		m_WorkingThread.emplace_back(std::async(std::launch::async, lambder, handle, result));

	}

	void EditorAssetManegerThreade::IsAssetHandleValidAsync(const FileSystem::Path& path, bool* result)
	{
		std::lock_guard<std::mutex> scope(m_WorkingThreadMutex);

	    std::function<void(FileSystem::Path, bool*)> lambder = [this](FileSystem::Path pathLem, bool* resultLem) -> void
		{
			*resultLem = IsAssetHandleValid(pathLem);
		};

		m_WorkingThread.emplace_back(std::async(std::launch::async, lambder, path, result));

	}

	void EditorAssetManegerThreade::IsAssetInteralAsync(AssetHandle handle, bool* result)
	{
	    std::lock_guard<std::mutex> scope(m_WorkingThreadMutex);

		m_WorkingThread.emplace_back(std::async(std::launch::async, [&](AssetHandle handleLem, bool* resultLem)
			{
				*resultLem = IsAssetInteral(handleLem);
			}, handle, result));

	}

	void EditorAssetManegerThreade::IsAssetLoadedAsync(AssetHandle handle, bool* result)
	{
	    std::lock_guard<std::mutex> scope(m_WorkingThreadMutex);
	    std::function<void(AssetHandle, bool*)> lambder = [this](AssetHandle handleLambder, bool* resultLambder) -> void
	    {
	        *resultLambder = IsAssetLoaded(handleLambder);
	    };
		m_WorkingThread.emplace_back(std::async(std::launch::async, lambder, handle, result));

	}

	void EditorAssetManegerThreade::IsAssetLoadedAsync(const FileSystem::Path& path, bool* result)
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

	const AssetMetadata EditorAssetManegerThreade::GetMetadata(AssetHandle handle) const
	{
		if (IsAssetHandleValid(handle))
		{
			return m_HandleRegistry.GetCopy(handle);
		}
		return AssetMetadata();
	}

	const AssetMetadata EditorAssetManegerThreade::GetMetadata(const FileSystem::Path& path) const
	{
		const AssetHandle handle = GetAssetHandle(path);
		return GetMetadata(handle);
	}

	void EditorAssetManegerThreade::GetMetadataAsync(AssetHandle handle, AssetMetadata* meatdata)
	{
	    std::lock_guard<std::mutex> scope(m_WorkingThreadMutex);
	    std::function<void(AssetHandle, AssetMetadata*)> lambder = [this](AssetHandle handleLambder, AssetMetadata* metadataLambder) -> void
	    {
	        *metadataLambder = GetMetadata(handleLambder);
	    };

		m_WorkingThread.emplace_back(std::async(std::launch::async, lambder, handle, meatdata));


	}

	void EditorAssetManegerThreade::GetMetadataAsync(const FileSystem::Path& path, AssetMetadata* meatdata)
	{
	    std::lock_guard<std::mutex> scope(m_WorkingThreadMutex);

	    std::function<void(FileSystem::Path handleLambder, AssetMetadata* metadataLambder)> lambder = [this](FileSystem::Path handleLambder, AssetMetadata* metadataLambder) -> void
		{
			*metadataLambder = GetMetadata(handleLambder);
		};

		m_WorkingThread.emplace_back( std::async(std::launch::async, lambder, path, meatdata) );

	}

	const AssetHandle EditorAssetManegerThreade::GetAssetHandle(const FileSystem::Path& path) const
	{
		static AssetHandle s_NullHnadle = AssetHandle(0);
		if (m_PathRegistry.IsFound(path))
			return m_PathRegistry.GetCopy(path);
			
		return s_NullHnadle;
	}

	

	void EditorAssetManegerThreade::GetAssetHandleAsync(const FileSystem::Path& path, AssetHandle* handle)
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

	Ref<Asset> EditorAssetManegerThreade::GetAsset(AssetHandle handle)
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

				switch (metadataC.State)
				{	
					case AssetState::NotLoaded:
					case AssetState::LostConection:
					{
					    asset = GetAssetLostConection(handle);
					    break;
					}
					case AssetState::Loading:
					{
						bool lastTimeTaskExexute = true;
						do {
							if (Asset::CurrentOnMainThread() || !lastTimeTaskExexute)
							{
								using namespace std::chrono_literals;
								std::this_thread::sleep_for(100ms);
								RY_ASSET_WARN("Wait For Asset To Loade");
							}
							else
							{
								lastTimeTaskExexute = Application::Get().ExecuteTaskFromThread();
							}
						} while (GetMetadata(handle).State == AssetState::Loading);

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
						RY_CORE_ASSERT(false, "State Not Deffined!");
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

	Ref<Asset> EditorAssetManegerThreade::GetAssetAsync(AssetHandle handle)
	{
		return GetAsset(handle);
	}

	Ref<Asset> EditorAssetManegerThreade::GetAsset(const FileSystem::Path& path)
	{
		if (!IsAssetHandleValid(path))
			CreateFileAsset(path);

		const AssetHandle handle = GetAssetHandle(path);

		if (IsFileAssetExist(path))
		{
			SetAssetMetadataLockState(handle, std::bind(
			    &EditorAssetManegerThreade::SetAssetMetadataStateNotLoaded,
			    this,
			    std::placeholders::_1)
			);

		}
		return GetAsset(handle);
	}

	Ref<Asset> EditorAssetManegerThreade::GetAssetAsync(const FileSystem::Path& path)
	{
		if (!IsAssetHandleValid(path))
			CreateFileAsset(path);
		const AssetHandle handle = GetAssetHandle(path);
		return GetAsset(handle);
	}


#pragma region LocaleAssetsDataMage

	void EditorAssetManegerThreade::CreatLocaleAsset(Ref<Asset> asset, AssetMetadata& metadata, AssetHandle handle)
	{		
		metadata.SetIntern(true);
		metadata.State = AssetState::Ready;

		std::lock_guard<std::mutex> lockChanges(m_ChangesMutex);
		m_RegestryChanges = true;

		m_HandleRegistry.Add(handle, metadata);
		m_LoadedAssets.Add(handle,asset);

	}

	AssetHandle EditorAssetManegerThreade::CreatLocaleAsset(Ref<Asset> asset, AssetMetadata& metadata)
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

	Ref<Asset> EditorAssetManegerThreade::GetLocaleAsset(AssetHandle handle)
	{
		RY_CORE_ASSERT(false, "This funktion is not any longer Sepoted! Use the funktion GetAsset");
		if (IsAssetLoaded(handle) && !IsAssetHandleValid(handle))
		{
			return m_LoadedAssets.GetCopy(handle);
		}
		return nullptr;
	}

	void EditorAssetManegerThreade::DeleteLocaleAsset(AssetHandle handle)
	{
		if (IsAssetLoaded(handle) && IsAssetInteral(handle))
		{
			std::lock_guard<std::mutex> lockChanges(m_ChangesMutex);

			m_LoadedAssets.Remove(handle);
			m_HandleRegistry.Remove(handle);
			m_RegestryChanges = true;
		}
	}


#pragma endregion


#pragma endregion


#pragma region DiscFile

	void EditorAssetManegerThreade::ScaneDirectory(const FileSystem::Path& curent)
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
				ScaneDirectory(path);
			}
			else
			{
				if (path.GetAssetFileType() != AssetType::None)
				{
					CreateFileAsset(path);
				}
			}
		}

	}

	void EditorAssetManegerThreade::ReLoadeAsset(AssetHandle handle)
	{
		RY_CORE_ASSERT(handle != 0, "This Handle is Unacceptable");
		if (IsAssetLoaded(handle))
		{
			std::chrono::microseconds waitTime = std::chrono::milliseconds(30);
			std::this_thread::sleep_for(waitTime);
			{
				int about = 0;
			    SetAssetMetadataLockState(handle, std::bind(&EditorAssetManegerThreade::SetAssetMetadataStateUpdateing, this, &about, std::placeholders::_1));
				if (about == 1)
				{
					RY_ASSET_ERROR("About Reloading! Already Reloading somewhere else!");
					return;
				}
				RY_ASSET_WARN("Beginn reloding Asset");
			} 
			

			const AssetMetadata metadataC = GetMetadata(handle);
			bool result = false;
			for (uint32_t i = 0; i < 3 && !result; i++)
			{
				
				waitTime = std::chrono::milliseconds(i * 45);
				if(waitTime != std::chrono::milliseconds(0))
					std::this_thread::sleep_for(waitTime);
				result = AssetImporter::ReLoadeAsset(handle, metadataC);
			}
			
			if(result)
			    SetAssetMetadataLockState(handle, std::bind(&EditorAssetManegerThreade::SetAssetMetadataStateReady, this, std::placeholders::_1));
			else
			    SetAssetMetadataLockState(handle, std::bind(&EditorAssetManegerThreade::SetAssetMetadataStateError, this, std::placeholders::_1));


			RY_ASSET_INFO("Asset: {0} Is Now Reloded", GetMetadata(handle).FilePath.string().c_str());
		}
		else
		{
			RY_ASSET_INFO("don't need Reloding Asset: {0}", GetMetadata(handle).FilePath.string().c_str());
		}
	}

	void EditorAssetManegerThreade::ReLoadeAsset(const FileSystem::Path& path)
	{
		const AssetHandle handle = GetAssetHandle(path);
		ReLoadeAsset(handle);
	}

	void EditorAssetManegerThreade::GetMetadata(AssetHandle handle, AssetMetadata* metaData, std::mutex* metaDataMutex)
	{
		RY_CORE_ASSERT(false);
	}

	void EditorAssetManegerThreade::DeleateDirectory(const std::vector<FileSystem::Path>& foldersPaths)
	{
		for (const FileSystem::Path& dirPath : foldersPaths)
		{
			DeleateDirectory(dirPath);
		}
	}

	void EditorAssetManegerThreade::DeleateFileAsset(const std::vector<AssetHandle>& handles)
	{
		for (const AssetHandle handles : handles)
		{
			DeleateFileAsset(handles);
		}
	}

    Ref<Asset> EditorAssetManegerThreade::GetAssetLostConection(AssetHandle handle)
    {
        SetAssetMetadataLockState(handle, std::bind(&EditorAssetManegerThreade::SetAssetMetadataStateLoading, this, std::placeholders::_1));

	    Ref<Asset> asset = AssetImporter::ImportAsset(handle, GetMetadata(handle));

	    if (asset)
	    {
	        SetAssetMetadataLockState(handle, std::bind(&EditorAssetManegerThreade::SetAssetMetadataStateReady, this, std::placeholders::_1));
	        asset->Handle = handle;

	        m_LoadedAssets.Add(handle, asset);
	        std::lock_guard<std::mutex> lockChange(m_ChangesMutex);
	        m_RegestryChanges = true;
	    }
	    else
	    {
	        RY_CORE_ERROR("'EditorAssetManager::GetAsset' No Asset Loaded! Empty Resource");
	        SetAssetMetadataLockState(handle, std::bind(&EditorAssetManegerThreade::SetAssetMetadataStateError, this, std::placeholders::_1));
	    }
	    return asset;
    }

    void EditorAssetManegerThreade::SetAssetMetadataLockState(AssetHandle handle, std::function<void(AssetMetadata&)> lambder)
    {
	    std::lock_guard<std::mutex> lockPath(m_CurentPathMutex);
	    std::lock_guard<std::mutex> lockChange(m_ChangesMutex);
	    m_HandleRegistry.GetRefLemda(lambder, handle);
    }

    void EditorAssetManegerThreade::SetAssetMetadataUpdateContentBrowser(AssetMetadata& metadata)
    {
	    const FileSystem::Path& path = metadata.Path;
	    FileSystem::Path parent = path.GetParent();
	    if(m_CurentPath == parent)
	        m_FileChanges = true;
	    m_RegestryChanges = true;
    }

    void EditorAssetManegerThreade::SetAssetMetadataStateNotLoaded(AssetMetadata& metadata)
    {
	    SetAssetMetadataUpdateContentBrowser(metadata);
	    metadata.State = AssetState::NotLoaded;
    }

    void EditorAssetManegerThreade::SetAssetMetadataStateLoading(AssetMetadata& metadata)
    {
	    SetAssetMetadataUpdateContentBrowser(metadata);
	    metadata.State = AssetState::Loading;
    }

    void EditorAssetManegerThreade::SetAssetMetadataStateError(AssetMetadata& metadata)
    {
	    SetAssetMetadataUpdateContentBrowser(metadata);
	    metadata.State = AssetState::Error;
    }

    void EditorAssetManegerThreade::SetAssetMetadataStateReady(AssetMetadata& metadata)
    {
	    SetAssetMetadataUpdateContentBrowser(metadata);
	    metadata.State = AssetState::Ready;
	    metadata.LoadingInTime = std::chrono::steady_clock::now();
    }

    void EditorAssetManegerThreade::SetAssetMetadataStateUpdateing(int* aboutPtr, AssetMetadata& metadata)
    {
	    if (metadata.State == AssetState::Updateing)
	        *aboutPtr = 1;
	    else
	        metadata.State = AssetState::Updateing;
	    if (metadata.Path.GetParent() == m_CurentPath)
	        m_FileChanges = true;
    }

    void EditorAssetManegerThreade::SetAssetMetadataStateLostConection(AssetMetadata& metadata)
    {
	    SetAssetMetadataUpdateContentBrowser(metadata);
	    metadata.State = AssetState::LostConection;
    }


    void EditorAssetManegerThreade::ClearLodeadAssetList()
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
			this->UnLoadeFileAsset(assetHandle);
		}
	}


	void EditorAssetManegerThreade::SerialzeAssetRegistry()
	{
		RY_ASSET_INFO("SerialzeAsseRegistry Asset Regestriey");

		m_HandleRegistry.Write(
			[](std::map<AssetHandle, AssetMetadata>& map)
			{
				std::filesystem::path path = Project::GetActiveAssetRegistryPath();
				EditorAssetMangerSerialzation::SerilzeThread(path, &map);
			}
		);
	}

	bool EditorAssetManegerThreade::DeserialzeAssetRegistry()
	{
		RY_ASSET_INFO("DeserialzeAssetRegistry Asset Regestriey");

			m_HandleRegistry.Write(
				[this](std::map<AssetHandle, AssetMetadata>& mapHandleRegister)
				{

					m_PathRegistry.Write(
						[this, &mapHandleRegister](std::map<FileSystem::Path, AssetHandle>& mapPathRegister)
						{
							std::filesystem::path path = Project::GetActiveAssetRegistryPath();
							EditorAssetMangerSerialzation::DeserilzeThread(path, &mapHandleRegister, &mapPathRegister);
						}
					);
					
				}
			);
		return true;
	}


	bool EditorAssetManegerThreade::ExexuteEvent(AssetHandle handle)
	{
		constexpr int64_t MAX_WAIT_TIME_LODING_AGAIN_SEC = 1.5;
		if (handle)
		{			
			if(IsAssetLoaded(handle))
			{
				const AssetMetadata metadata = GetMetadata(handle);
				AssetState state = metadata.State;
				std::chrono::steady_clock::time_point lodetime = metadata.LoadingInTime;
				switch (state)
				{
				case Rynex::AssetState::Loading:
				case Rynex::AssetState::Updateing:
				case Rynex::AssetState::Uploading:
					return false;
				case Rynex::AssetState::LostConection:
				case Rynex::AssetState::Error:
					RY_ASSET_INFO("Mayby Conection to {} is Now longer lost!", metadata.FilePath.string().c_str());
				case Rynex::AssetState::Ready:
				default:
					if (lodetime != std::chrono::steady_clock::time_point::min())
					{
						std::chrono::steady_clock::time_point curent = std::chrono::steady_clock::now();
						int64_t diferenc = std::chrono::duration_cast<std::chrono::seconds>(curent - lodetime).count();
						return MAX_WAIT_TIME_LODING_AGAIN_SEC < diferenc;
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

	bool EditorAssetManegerThreade::IsFileAssetExist(const FileSystem::Path& path)
	{
		if (path.IsRelative())
		{
		    const std::filesystem::path& pathSystem = path.GetPath();
			RY_CORE_WARN("Maby the File-Path is not absolut \n\t{}", pathSystem);
		}
		return path.IsExisting();
	}

	bool EditorAssetManegerThreade::IsCurentAssetState(const FileSystem::Path& showPath) const
	{
		return m_FileChanges || showPath != m_CurentPath;
	}

	ContentBrowserItemesThreade EditorAssetManegerThreade::GetCurentAssetInformation(const FileSystem::Path& showPath)
	{
		if (IsDirectoryInRegistry(showPath))
		{
			ContentBrowserItemesThreade itemes;
			{
				m_DirectoryRegistry.Read<bool>(
					[this, &itemes, &showPath](const std::map<FileSystem::Path, AssetFileDirectoryThreade>& assetBrowserDataThreade) -> bool
					{
						const AssetFileDirectoryThreade& assetFileDirectory = assetBrowserDataThreade.at(showPath);
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
							itemes.emplace_back(assetBrowserDataThreade );
						}

						for (const AssetHandle handle : handles)
						{
							if (!IsAssetHandleValid(handle))
								continue;
							CheckAssetFileExist(handle);
							const AssetMetadata metadata = GetMetadata(handle);

							AssetType type = Asset::GetAssetTypeFromFilePath(metadata.FilePath);

							RY_CORE_ASSERT(!metadata.GetIntern());

						    AssetBrowserDataThreade assetBrowserDataThreade(
                                true,
                                false,
                                handle,
                                metadata,
                                type,
                                Asset::GetAssetTypeDragAndDropName(type),
                                metadata.State,
                                metadata.Path.GetNamePathString(),
                                metadata.Path,
                                metadata.Path.GetPathString()
                            );

							itemes.emplace_back(assetBrowserDataThreade);
						}

						return true;
					}
				);

			}
			{
				std::lock_guard<std::mutex> lockChange(m_ChangesMutex);
				std::lock_guard<std::mutex> lockPath(m_CurentPathMutex);
				m_FileChanges = false;
				m_CurentPath = showPath;
			}
			return itemes;

		}
		
		return ContentBrowserItemesThreade();
	}

	bool EditorAssetManegerThreade::IsCurentRegistryAssetChandge() const
	{
		std::lock_guard<std::mutex> lockChange(m_ChangesMutex);
		return m_RegestryChanges;
	}

	RegisterItemesThreade EditorAssetManegerThreade::GetCurentAssetRegistry()
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

	void EditorAssetManegerThreade::EventAsyncModified(const FileSystem::Path& path)
	{
		const AssetHandle handle = GetAssetHandle(path);
		
		if (ExexuteEvent(handle))
		{
			m_WorkingThreadMutex.lock();
			m_WorkingThread.emplace_back(std::async(std::launch::async, [this](AssetHandle handleLem)->void {
				const AssetMetadata metadata = GetMetadata(handleLem);
				RY_ASSET_WARN("Exexute Modified Event -> Asset: '{}'", metadata.FilePath.string().c_str());
				ReLoadeAsset(handleLem);
			}, handle));
			m_WorkingThreadMutex.unlock();
		}
		else
		{
			const AssetMetadata metadata = GetMetadata(handle);
			RY_ASSET_WARN("Blocked Modified Event -> Asset: '{}'", metadata.FilePath.string().c_str());
		}

	}

	void EditorAssetManegerThreade::EventAsyncAdded(const FileSystem::Path& path)
	{
		m_WorkingThreadMutex.lock();
		m_WorkingThread.emplace_back(std::async(std::launch::async, [this](const FileSystem::Path pathLem)
			{
				std::filesystem::path parent = pathLem.GetParentPath();
		        std::string name = pathLem.GetNamePathString();
				RY_ASSET_WARN("Execute Added Event -> Path: '{}' | Scan Dir: '{}'", name, parent);
				ScaneDirectory(parent);
			}, path));
		m_WorkingThreadMutex.unlock();
	}

	void EditorAssetManegerThreade::EventAsyncRemoved(const FileSystem::Path& path)
	{
		RY_ASSET_WARN("Event Removed Thread is doing nothing! TODO!");
	}

	void EditorAssetManegerThreade::EventAsyncRenamedNew(const FileSystem::Path& path)
	{
		RY_ASSET_WARN("Event RenamedNew Thread is doing nothing! TODO!");
	}

	void EditorAssetManegerThreade::EventAsyncRenamedOld(const FileSystem::Path& path)
	{
		RY_ASSET_WARN("Event RenamedOld Thread is doing nothing! TODO!");
	}

	bool EditorAssetManegerThreade::CheckAssetFileExist(AssetHandle handle)
	{
		AssetMetadata metaData = m_HandleRegistry.GetCopy(handle);
		if (!metaData)
			return false;

		const AssetState& state = metaData.State;
		if (!IsAssetPathExtensionVaild(metaData.FilePath))
		{
			 SetAssetMetadataLockState(handle, std::bind(&EditorAssetManegerThreade::SetAssetMetadataStateError, this, std::placeholders::_1));;
		}
		else if (state == AssetState::LostConection || state == AssetState::Error)
		{
		    SetAssetMetadataLockState(handle, std::bind(&EditorAssetManegerThreade::SetAssetMetadataStateLostConection, this, std::placeholders::_1));
		}
		return true;
	}

	void EditorAssetManegerThreade::CreateAssetDirectory(const FileSystem::Path& path)
	{
		RY_CORE_ASSERT(!IsDirectoryInRegistry(path), "Found folder in register alraedy or folder path is a file!");

		std::filesystem::path folderRelativPath = path.GetRelativePath();
		std::string name = folderRelativPath.filename().string();

	
		std::lock_guard<std::mutex> lockChang(m_ChangesMutex);
		std::lock_guard<std::mutex> lockPath(m_CurentPathMutex);
		if (m_CurentPath == folderRelativPath)
			m_FileChanges = true;

		m_RegestryChanges = true;
		
		
		AssetFileDirectoryThreade assetFileDirectory;
		assetFileDirectory.FolderName = name;
		assetFileDirectory.FolderPath = folderRelativPath;

		m_DirectoryRegistry.Add(folderRelativPath, assetFileDirectory);

		RY_ASSET_INFO("Add AssetFolder Names {} on location {}", name, folderRelativPath);
	}

	void EditorAssetManegerThreade::AddAssetFileToAssetDirectory(const FileSystem::Path& assetPath, const std::string& assetName, const std::string& assetPathString, AssetHandle handle)
	{
		FileSystem::Path parent;
		std::string extension = assetPath.GetExtensionPathString();
		if (extension.find("-", 0) < extension.size())
		{
		    size_t count = assetPathString.size() - 2;
			RY_ASSET_WARN("Deteced speciel file Extension on Asset {} on location {}", assetName, assetPathString);
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
		m_RegestryChanges = true;
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

	void EditorAssetManegerThreade::AddDirectoryToParent(const FileSystem::Path& path)
	{
	    FileSystem::Path fileSystemPath(path);


		if (!IsDirectoryInRegistry(path))
		{
			CreateAssetDirectory(path);
		}
	    FileSystem::Path parentFolder = path.GetParent();

		if (IsDirectoryInRegistry(parentFolder))
		{
			std::lock_guard<std::mutex> lockChang(m_ChangesMutex);
			std::lock_guard<std::mutex> lockPath(m_CurentPathMutex);
			if (m_CurentPath == parentFolder)
				m_FileChanges = true;
			m_RegestryChanges = true; 
			
			bool result = m_DirectoryRegistry.ReadValue<bool>(parentFolder,
				[&path](const AssetFileDirectoryThreade& assetFileDirectory) -> bool
				{
					for (auto& folder : assetFileDirectory.Folders)
					{
						if (folder == path)
							return false;
					}
					return true;

					
				});
			if (result)
			{
				m_DirectoryRegistry.WriteValue(parentFolder,
					[&path](AssetFileDirectoryThreade& assetFileDirectory)
					{
						assetFileDirectory.Folders.emplace_back(path);
					}
				);
				
			}
		    std::string parentFolderStr = path.GetNamePathString();
		    const std::filesystem::path& filePath = parentFolder.GetPath();
			RY_ASSET_WARN("Add Child Folder {} to Folder {}", parentFolderStr, filePath);
		}
		else
		{
			CreateAssetDirectory(parentFolder);
		}
	}


	void EditorAssetManegerThreade::CreateFileAsset(const FileSystem::Path& path)
	{

	    std::filesystem::path assetPath = path.GetRelativePath();
	    std::string assetName = assetPath.filename().string();

		if (!IsAssetHandleValid(path) && IsAssetPathExtensionVaild(path))
		{
		    std::string markedAssetPathStr = path.GetMarkedPathString();
			CreateNewFileAsset(assetName, path, markedAssetPathStr);
		}
		else if (IsAssetHandleValid(path))
		{
			
			if (!IsFileAssetExist(path))
			{
				const AssetHandle handle = GetAssetHandle(path);
				if (IsAssetLoaded(handle))
					UnLoadeFileAsset(handle);


				std::lock_guard<std::mutex> lockChang(m_ChangesMutex);
				std::lock_guard<std::mutex> lockPath(m_CurentPathMutex);

				m_HandleRegistry.GetRefLemda([this](AssetMetadata& metaData)-> void
					{
						metaData.State = AssetState::LostConection;
						if (m_CurentPath == metaData.FilePath.parent_path())
							m_FileChanges = true;
						m_RegestryChanges = true;

					}, handle);
				
				
			}
			else
			{
				const AssetHandle handle = GetAssetHandle(assetPath);
				if (!IsAssetLoaded(handle))
				{
					std::lock_guard<std::mutex> lockChang(m_ChangesMutex);
					std::lock_guard<std::mutex> lockPath(m_CurentPathMutex);
					m_HandleRegistry.GetRefLemda([this](AssetMetadata& metaData)
						{
							if (metaData.State != AssetState::NotLoaded)
							{
								metaData.State = AssetState::LostConection;
								if (m_CurentPath == metaData.FilePath.parent_path())
									m_FileChanges = true;
								m_RegestryChanges = true;
							}
						}, handle);
				}
				else
				{
					m_HandleRegistry.GetRefLemda([](AssetMetadata& metaData)
						{
							if (metaData.State != AssetState::Ready && metaData.State != AssetState::Loading)
							{
								RY_CORE_ASSERT(false, "Interesting Stat is not Ready or Current Loding, but is in LoadedAssets Map!");
							}
						}, handle);
				}
			    std::string assetPathStr = assetPath.string();
				AddAssetFileToAssetDirectory(assetPath, assetName, assetPathStr, handle);
			}


		}
		else if (!IsAssetPathExtensionVaild(assetPath))
		{
			RY_ASSET_WARN("Asset {} we don't add to regestry because extione {} is not AssetType", assetPath, assetPath.extension().string());
		}
		else
		{
			RY_CORE_ASSERT(false, "Don't know how that can happen");
		}
	}

	void EditorAssetManegerThreade::CreateNewFileAsset(const std::string& name, const FileSystem::Path& path, const std::string& pathMarker)
	{
		AssetMetadata metaData;
		metaData.SetActive(true);
		metaData.SetFilePath(path);
		metaData.Name = name;
		metaData.Type = path.GetAssetFileType();
		metaData.ChangeTime = AssetRegistry::GetCurrentTimeStr();
		metaData.State = IsFileAssetExist(path) ? AssetState::NotLoaded : AssetState::LostConection;

		AssetHandle handle;
		while (handle == 0 || IsAssetHandleValid(handle))
			handle = AssetHandle();

		std::string pathString = path.GetPathString();
		AddAssetFileToAssetDirectory(path, name, pathString, handle);

		{
			std::lock_guard<std::mutex> lock(m_ChangesMutex);
			std::lock_guard<std::mutex> lockPath(m_CurentPathMutex);
		    FileSystem::Path parent = path.GetParentPath();
			if (m_CurentPath == parent)
				m_FileChanges = true;

			m_RegestryChanges = true;
		}
		m_HandleRegistry.Add(handle, metaData);
		m_PathRegistry.Add(metaData.AbsolutePath, handle);
		m_PathRegistry.Add(metaData.RealtivePath, handle);

		if(m_PathRegistry.IsFound(metaData.FilePath))
			m_PathRegistry.Add(metaData.FilePath, handle);
	}

	void EditorAssetManegerThreade::DeleateDirectory(const FileSystem::Path& path)
	{

		if (IsDirectoryInRegistry(path))
		{

			AssetFileDirectoryThreade dirRegT = m_DirectoryRegistry.GetCopy(path);

			DeleateDirectory(dirRegT.Folders);
			DeleateFileAsset(dirRegT.AssetFiles);
			

			std::lock_guard<std::mutex> lock(m_ChangesMutex);
			m_FileChanges = true;
			m_RegestryChanges = true;
		}
	}

	void EditorAssetManegerThreade::DeleateFileAsset(AssetHandle handle)
	{
		if (IsAssetHandleValid(handle))
		{
			if (IsAssetLoaded(handle))
				UnLoadeFileAsset(handle);
			const AssetMetadata metadata = m_HandleRegistry.GetCopy(handle);
			if (metadata.GetDisc())
			{
			    const FileSystem::Path& pathKey = metadata.Path;
				m_PathRegistry.Remove(pathKey);

				if (m_PathRegistry.IsFound(pathKey))
					m_PathRegistry.Remove(pathKey);
			}
			m_HandleRegistry.Remove(handle);
			std::lock_guard<std::mutex> lockChange(m_ChangesMutex);
			m_FileChanges = true;
			m_RegestryChanges = true;
		}
	}

	void EditorAssetManegerThreade::UnLoadeFileAsset(const AssetHandle handle)
	{		
		if (!IsAssetHandleValid(handle) || !IsAssetLoaded(handle))
			return;
		std::lock_guard<std::mutex> lockChang(m_ChangesMutex);
		std::lock_guard<std::mutex> lockPath(m_CurentPathMutex);
		m_HandleRegistry.GetRefLemda([this](AssetMetadata& metaData)
			{
				metaData.State = AssetState::NotLoaded;
				RY_ASSET_WARN("Try to UnLoadeing Asset {} from disc path {}", metaData.FilePath.string().c_str(), metaData.Name.c_str());
				
				if (m_CurentPath == metaData.FilePath.parent_path())
					m_FileChanges = true;
				m_RegestryChanges = true;
			}, handle);
		Ref<Asset> assetRefCopy = m_LoadedAssets.GetCopy(handle);
		Weak<Asset> assetWeakCopy = assetRefCopy;
		RY_DESTROY_REF(assetRefCopy);

		m_LoadedAssets.Remove(handle);
		
		if (m_LoadedAssets.IsFound(handle))
		{
			RY_CORE_FATAL("UnLoadeing Faild its still in the Loaded AssetList");
		}
		if (Ref<Asset> assetWeakCopyRef = assetWeakCopy.lock())
		{
			RY_CORE_FATAL("UnLoadeing Faild Asset Still exist some whear else!");
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