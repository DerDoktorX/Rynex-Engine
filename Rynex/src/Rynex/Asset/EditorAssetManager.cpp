#include "rypch.h"
#include "EditorAssetManager.h"

#include "Base/AssetManager.h"
#include "Base/AssetImporter.h"
#include "Rynex/Project/Project.h"

#include "Rynex/Serializers/EditorAssetMangerSerialzation.h"
#include "Rynex/Renderer/API/Texture.h"

#include <chrono>

namespace Rynex {


#pragma region AssetRegistry

	bool AssetRegistry::IsAssetInRegistry(AssetHandle handle) const
	{
		return m_HandleRegistry.find(handle) != m_HandleRegistry.end();
	}

	bool AssetRegistry::IsAssetInRegistry(const std::filesystem::path& path) const
	{
		return m_PathRegistry.find(path.generic_string()) != m_PathRegistry.end();
	}

	bool AssetRegistry::IsDirectoryInRegistry(const std::filesystem::path& parentPath) const
	{
		return m_DirectoryRegistry.find(parentPath.generic_string()) != m_DirectoryRegistry.end();
	}

	bool AssetRegistry::IsAssetPath(const std::filesystem::path& path) const
	{
		return path.has_extension();
	}

	bool AssetRegistry::IsAssetInteral(AssetHandle handle) const
	{
		return IsAssetInRegistry(handle) && GetMetadataConst(handle).GetIntern();
	}

	void AssetRegistry::CreateAsset(const std::filesystem::path& path, AssetHandle handle, AssetMetadata metadata, bool findDirectOnDisc)
	{
		if (!IsAssetInRegistry(path) && IsAssetPath(path))
		{
			
			if (!metadata)
			{
				metadata.SetActive(true);
				metadata.FilePath = path.generic_string();
				metadata.Name = path.filename().generic_string();
				metadata.Type = Asset::GetAssetTypeFromFilePath(path);
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
			std::string extension = path.extension().string();
			if(extension.find("-",0) < extension.size())
			{
				size_t pos = extension.find("-", 0)- extension.size();

				std::string& pathGenaric = path.generic_string();
				
				std::string& parentGenaric = pathGenaric.substr(0, pathGenaric.size() - 2);
				if (IsDirectoryInRegistry(parentGenaric))
				{
					AssetFileDirectory& assetFileDirectory = m_DirectoryRegistry[parentGenaric];
					assetFileDirectory.Files.emplace_back(handle);
				}
				else
				{
					AssetFileDirectory& assetFileDirectory = m_DirectoryRegistry[parentGenaric];
					assetFileDirectory.Files.emplace_back(handle);
					assetFileDirectory.FolderName = path.parent_path().filename().generic_string();
					assetFileDirectory.FolderPath = parentGenaric;
					RY_ASSET_WARN("This Shoud Not Happend!");
				}
			}
			else
			{
				std::string& parentGenaric = path.parent_path().generic_string();
				if (IsDirectoryInRegistry(parentGenaric))
				{
					AssetFileDirectory& assetFileDirectory = m_DirectoryRegistry[parentGenaric];
					assetFileDirectory.Files.emplace_back(handle);
				}
				else
				{
					AssetFileDirectory& assetFileDirectory = m_DirectoryRegistry[parentGenaric];
					assetFileDirectory.Files.emplace_back(handle);
					assetFileDirectory.FolderName = path.parent_path().filename().generic_string();
					assetFileDirectory.FolderPath = parentGenaric;
					RY_ASSET_WARN("This Shoud Not Happend!");
				}
			}
			

			m_HandleRegistry[handle] = metadata;
			m_PathRegistry[path.generic_string()] = handle;
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
			asset->Handle = AssetHandle();

		metadata.SetIntern(true);

		metadata.State = AssetState::Ready;
		m_HandleRegistry[asset->Handle] = metadata;

		return asset->Handle;
	}


	bool AssetRegistry::AddDirectoryToParent(const std::filesystem::path& path)
	{
		std::string& parentGenaric = path.parent_path().generic_string();
		if (IsDirectoryInRegistry(parentGenaric))
		{
			
			AssetFileDirectory& assetFileDirectory = m_DirectoryRegistry[parentGenaric];
			bool findeFile = false;
			std::filesystem::path pathGenaric = path.generic_string();
			for (auto& folder:  assetFileDirectory.Folders)
			{
				if (folder == pathGenaric)
				{
					
					return true;
				}
			}
			assetFileDirectory.Folders.emplace_back(pathGenaric);
			m_Changes = true;
			return true;
		}
		else
		{
			AssetFileDirectory& assetFileDirectory = m_DirectoryRegistry[parentGenaric];
			assetFileDirectory.FolderName = path.parent_path().filename().generic_string();
			assetFileDirectory.FolderPath = parentGenaric;
			assetFileDirectory.Folders.emplace_back(path.generic_string());
			RY_ASSET_WARN("This Shoud Not Happend!");
			m_Changes = true;
			return true;
		}
		return false;
	}

	AssetHandle AssetRegistry::GetAssetHandle(const std::filesystem::path& path)
	{
		std::string& pathGenaric = path.generic_string();
		if (IsAssetInRegistry(pathGenaric))
		{
			return m_PathRegistry[pathGenaric];
		}
		else if(IsAssetPath(path) && 0 != (int)Asset::GetAssetTypeFromFilePath(path))
		{
			AssetHandle handle;
			do {
				handle = AssetHandle();
			} 
			while (handle == 0 || IsAssetInRegistry(handle));
			
			CreateAsset(path, handle);
			return handle;
		}
		else
			return AssetHandle(0);
	}

	AssetHandle AssetRegistry::GetAssetHandleConst(const std::filesystem::path& path) const
	{
		std::string& pathGenaric = path.generic_string();
		if (!IsAssetInRegistry(pathGenaric))
			return AssetHandle(0);

		AssetHandle handle = m_PathRegistry.at(pathGenaric);
		return handle;
	}

	AssetMetadata& AssetRegistry::GetMetadata(AssetHandle handle)
	{
		if (IsAssetInRegistry(handle))
		{
			m_Changes = true;
			return m_HandleRegistry[handle];
		}
		
		return AssetMetadata();
	}

	const AssetMetadata& AssetRegistry::GetMetadataConst(AssetHandle handle) const
	{
		if (IsAssetInRegistry(handle))
			return m_HandleRegistry.at(handle);
		return AssetMetadata();
	}

	AssetMetadata& AssetRegistry::GetMetadata(const std::filesystem::path& path)
	{
		return GetMetadata(GetAssetHandle(path));
	}

	bool AssetRegistry::IsCurentAssetState(const std::filesystem::path& showPath) const
	{
		return m_Changes || showPath != m_CurentPath;
	}

	ContentBrowserItemes AssetRegistry::GetCurentAssetInformation(const std::filesystem::path& showPath)
	{
		const AssetFileDirectory& assetFileDirectory = GetAssetFileDirectory(showPath);
		const std::vector<std::filesystem::path>& foldersPaths = assetFileDirectory.Folders;
		const std::vector<AssetHandle>& handles = assetFileDirectory.Files;
		ContentBrowserItemes itemes;
		itemes.reserve(foldersPaths.size()+ foldersPaths.size());

		for (const std::filesystem::path& folderPath : foldersPaths)
		{
			itemes.emplace_back(
				AssetBrowserData(false, true,
				AssetHandle(0), AssetMetadata(),
				AssetType::None, "",
				AssetState::None,  folderPath.filename().string(), 
				folderPath, folderPath.string()));
		}
		for (const AssetHandle handle : handles)
		{
			const AssetMetadata& metadata = GetMetadataConst(handle);
			
			AssetType type = Asset::GetAssetTypeFromFilePath(metadata.FilePath.filename());
			RY_CORE_ASSERT(!metadata.GetIntern());
			itemes.emplace_back(AssetBrowserData(true, false,
				handle, metadata,
				type, Asset::GetAssetTypeDragAndDropName(type),
				metadata.State, metadata.FilePath.filename().string(),
				metadata.FilePath, metadata.FilePath.string()));
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

	void AssetRegistry::DeleteFolder(const std::filesystem::path& folderPath)
	{
		std::filesystem::path folderparent = folderPath.parent_path().generic_string();
		if (IsDirectoryInRegistry(folderparent))
		{
			std::vector<std::filesystem::path>& fileDirectory = m_DirectoryRegistry[folderparent].Folders;
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
		RY_ASSET_INFO("Ende EditorAssetManegerThreade OnDetach");
	}


#pragma region IsAsset

	bool EditorAssetManegerThreade::IsDirectoryInRegistry(const std::filesystem::path& parentPath)
	{
		return !parentPath.has_extension() && m_DirectoryRegistry.IsFound(parentPath.generic_string());
	}


	bool EditorAssetManegerThreade::IsAssetPathExtensionVaild(const std::filesystem::path& parentPath) const
	{
		return parentPath.has_extension() && Asset::GetAssetTypeFromFilePath(parentPath) != AssetType::None;
	}

	bool EditorAssetManegerThreade::IsAssetHandleValid(AssetHandle handle) const
	{
		return m_HandleRegistry.IsFound(handle);

	}

	bool EditorAssetManegerThreade::IsAssetHandleValid(const std::filesystem::path& path) const
	{
		const AssetHandle handle = GetAssetHandle(path);
		return IsAssetHandleValid(handle);
	}

	bool EditorAssetManegerThreade::IsAssetInteral(AssetHandle handle) const
	{
		return  IsAssetHandleValid(handle) && GetMetadata(handle).GetIntern();
	}

	bool EditorAssetManegerThreade::IsAssetLoaded(const std::filesystem::path& path) const
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
		m_WorkingThreadMutex.lock();
		m_WorkingThread.emplace_back(std::async(std::launch::async, [&](AssetHandle handleLem, bool* resultLem)
			{
				*resultLem = IsAssetHandleValid(handleLem);
			}, handle, result));
		m_WorkingThreadMutex.unlock();
	}

	void EditorAssetManegerThreade::IsAssetHandleValidAsync(const std::filesystem::path& path, bool* result)
	{
		m_WorkingThreadMutex.lock();
		m_WorkingThread.emplace_back(std::async(std::launch::async, [&](const std::filesystem::path pathLem, bool* resultLem)
			{
				*resultLem = IsAssetHandleValid(pathLem);
			}, path, result));
		m_WorkingThreadMutex.unlock();
	}

	void EditorAssetManegerThreade::IsAssetInteralAsync(AssetHandle handle, bool* result)
	{
		m_WorkingThreadMutex.lock();
		m_WorkingThread.emplace_back(std::async(std::launch::async, [&](AssetHandle handleLem, bool* resultLem)
			{
				*resultLem = IsAssetInteral(handleLem);
			}, handle, result));
		m_WorkingThreadMutex.unlock();
	}

	void EditorAssetManegerThreade::IsAssetLoadedAsync(AssetHandle handle, bool* result)
	{
		m_WorkingThreadMutex.lock();
		m_WorkingThread.emplace_back(std::async(std::launch::async, [&](AssetHandle handleLem, bool* resultLem)
			{
				*resultLem = IsAssetLoaded(handleLem);
			}, handle, result));
		m_WorkingThreadMutex.unlock();
	}

	void EditorAssetManegerThreade::IsAssetLoadedAsync(const std::filesystem::path& path, bool* result)
	{
		m_WorkingThreadMutex.lock();
		m_WorkingThread.emplace_back(std::async(std::launch::async, [&](const std::filesystem::path pathLem, bool* resultLem)
			{
				*resultLem = IsAssetLoaded(pathLem);
			}, path, result));
		m_WorkingThreadMutex.unlock();
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

	const AssetMetadata EditorAssetManegerThreade::GetMetadata(const std::filesystem::path& path) const
	{
		const AssetHandle handle = GetAssetHandle(path);
		return GetMetadata(handle);
	}

	void EditorAssetManegerThreade::GetMetadataAsync(AssetHandle handle, AssetMetadata* meatdata)
	{
		m_WorkingThreadMutex.lock();
		m_WorkingThread.emplace_back(std::async(std::launch::async, [this](AssetHandle handleLem, AssetMetadata* meatdataLem)
			{
				*meatdataLem = GetMetadata(handleLem);
			}, handle, meatdata));
		m_WorkingThreadMutex.unlock();

	}

	void EditorAssetManegerThreade::GetMetadataAsync(const std::filesystem::path& path, AssetMetadata* meatdata)
	{
		m_WorkingThreadMutex.lock();
		m_WorkingThread.emplace_back(std::async(std::launch::async, [this](const std::filesystem::path handleLem, AssetMetadata* meatdataLem)
			{
				*meatdataLem = GetMetadata(handleLem);
			}, path, meatdata));
		m_WorkingThreadMutex.unlock();
	}

	const AssetHandle EditorAssetManegerThreade::GetAssetHandle(const std::filesystem::path& path) const
	{
		static AssetHandle s_NullHnadle = AssetHandle(0);
		if (m_PathRegistry.IsFound(path))
			return m_PathRegistry.GetCopy(path);
			
		return s_NullHnadle;
	}

	

	void EditorAssetManegerThreade::GetAssetHandleAsync(const std::filesystem::path& path, AssetHandle* handle)
	{
		m_WorkingThreadMutex.lock();
		m_WorkingThread.emplace_back(std::async(std::launch::async, [this](const std::filesystem::path pathLem, AssetHandle* handleLem)
			{
				*handleLem = GetAssetHandle(pathLem);
			}, path, handle));
		m_WorkingThreadMutex.unlock();
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
						{
							std::lock_guard<std::mutex> lockPath(m_CurentPathMutex);
							std::lock_guard<std::mutex> lockChange(m_ChangesMutex);

							m_HandleRegistry.GetRefLemda([this](AssetMetadata& metadata)
								{	
									if(metadata.FilePath.parent_path() == m_CurentPath)
										m_FileChanges = true;
									m_RegestryChanges = true;
									metadata.State = AssetState::Loading;
								}, handle);
							
						
						}

						asset = AssetImporter::ImportAsset(handle, GetMetadata(handle));

						{
							
						}
							
						RY_CORE_ASSERT(asset, "Error on: 'EditorAssetManager::GetAsset' No Asset Lodead!");
						if (asset)
						{

							{
								std::lock_guard<std::mutex> lockChange(m_ChangesMutex);
								std::lock_guard<std::mutex> lockPath(m_CurentPathMutex);

								m_HandleRegistry.GetRefLemda([this](AssetMetadata& metadata)
									{
										if (metadata.FilePath.parent_path() == m_CurentPath)
											m_FileChanges = true;
										metadata.State = AssetState::Ready;
										metadata.LoadingInTime = std::chrono::steady_clock::now();
									}, handle);

								asset->Handle = handle;
							}
							
							m_LoadedAssets.Add(handle, asset);

							
							{
								std::lock_guard<std::mutex> lockChange(m_ChangesMutex);
								m_RegestryChanges = true;
							}
						}
						else
						{
							std::lock_guard<std::mutex> lockChange(m_ChangesMutex);
							std::lock_guard<std::mutex> lockPath(m_CurentPathMutex);

							m_HandleRegistry.GetRefLemda([this](AssetMetadata& metadata)
								{
									if (metadata.FilePath.parent_path() == m_CurentPath)
										m_FileChanges = true;
									metadata.State = AssetState::Error;
									metadata.LoadingInTime = std::chrono::steady_clock::now();
								}, handle);


							m_RegestryChanges = true;
						}
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

	Ref<Asset> EditorAssetManegerThreade::GetAsset(const std::filesystem::path& path)
	{
		if (!IsAssetHandleValid(path))
			CreateFileAsset(path);

		const AssetHandle handle = GetAssetHandle(path);

		if (IsFileAssetExist(path))
		{
			{
				std::lock_guard<std::mutex> lockChange(m_ChangesMutex);
				std::lock_guard<std::mutex> lockPath(m_CurentPathMutex);

				m_HandleRegistry.GetRefLemda([this](AssetMetadata& metadata) {
					if (metadata.FilePath.parent_path() == m_CurentPath)
						m_FileChanges = true;
					metadata.State = AssetState::NotLoaded;
					
				}, handle);
				
				m_RegestryChanges = true;
			}
		}
		return GetAsset(handle);
	}

	Ref<Asset> EditorAssetManegerThreade::GetAssetAsync(const std::filesystem::path& path)
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

		{
			std::lock_guard<std::mutex> lockChanges(m_ChangesMutex);
			m_RegestryChanges = true;

			m_HandleRegistry.Add(handle, metadata);
			m_LoadedAssets.Add(handle,asset);
		}
	}

	AssetHandle EditorAssetManegerThreade::CreatLocaleAsset(Ref<Asset> asset, AssetMetadata& metadata)
	{
		while (IsAssetHandleValid(asset->Handle))
			asset->Handle = AssetHandle();
		AssetHandle handle = asset->Handle;

		CreatLocaleAsset(asset, metadata, handle);
		if (IsAssetInteral(handle))
			return handle;

		RY_CORE_ASSERT(false, "funktion retunrt 0 as Assethandle");
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

	void EditorAssetManegerThreade::ScaneDirectory(const std::filesystem::path& curentPath)
	{

		if (curentPath.empty())
			return;
	
		for (const std::filesystem::directory_entry& p : std::filesystem::directory_iterator(curentPath))
		{	
			const std::filesystem::path& path = p.path();
			if (p.is_directory())
			{
				AddDirectoryToParent(path);
				ScaneDirectory(path);
			}
			else
			{
				if (Asset::GetAssetTypeFromFilePath(path) != AssetType::None)
				{
					CreateFileAsset(path);
				}
			}
		}

	}

	void EditorAssetManegerThreade::ReLoadeAsset(AssetHandle handle)
	{
		RY_CORE_ASSERT(handle != 0, "This Handle is not Accipteble");
		if (IsAssetLoaded(handle))
		{
			std::chrono::microseconds waitTime = std::chrono::milliseconds(30);
			std::this_thread::sleep_for(waitTime);
			{
				std::lock_guard<std::mutex> lockChange(m_ChangesMutex);						
				std::lock_guard<std::mutex> lockPath(m_CurentPathMutex);
				int* aboutePtr = nullptr;
				int aboute = 0;
				aboutePtr = &aboute;
				m_HandleRegistry.GetRefLemda([aboutePtr, this](AssetMetadata& metadata)
					{
						if (metadata.State == AssetState::Updateing)
							*aboutePtr = 1;
						else
							metadata.State = AssetState::Updateing;
						if (metadata.FilePath.parent_path() == m_CurentPath)
							m_FileChanges = true;
					}, handle);

				if (aboute == 1)
				{
					RY_ASSET_ERROR("About Reloading! Already Reloding somewhere else!");
					return;
				}

				
				m_RegestryChanges = true;
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
			{
				std::lock_guard<std::mutex> lockChange(m_ChangesMutex);
				std::lock_guard<std::mutex> lockPath(m_CurentPathMutex);

				m_HandleRegistry.GetRefLemda([this](AssetMetadata& metadata)
					{
						metadata.State = AssetState::Ready;
						metadata.LoadingInTime = std::chrono::steady_clock::now();
						if (metadata.FilePath.parent_path() == m_CurentPath)
							m_FileChanges = true;
					}, handle);
				
				m_RegestryChanges = true;
			}
			else
			{						
				std::lock_guard<std::mutex> lockPath(m_CurentPathMutex);
				m_HandleRegistry.GetRefLemda([this](AssetMetadata& metadata)
					{
						metadata.State = AssetState::Ready;
						if (metadata.FilePath.parent_path() == m_CurentPath)
							m_FileChanges = true;
					}, handle);
			}

			RY_ASSET_INFO("Asset: {0} Is Now Reloded", GetMetadata(handle).FilePath.string().c_str());
		}
		else
		{
			RY_ASSET_INFO("don't need Reloding Asset: {0}", GetMetadata(handle).FilePath.string().c_str());
		}
	}

	void EditorAssetManegerThreade::ReLoadeAsset(const std::filesystem::path& path)
	{
		const AssetHandle handle = GetAssetHandle(path);
		ReLoadeAsset(handle);
	}

	void EditorAssetManegerThreade::GetMetadata(AssetHandle handle, AssetMetadata* metaData, std::mutex* metaDataMutex)
	{
		RY_CORE_ASSERT(false);
	}

	void EditorAssetManegerThreade::DeleateDirectory(const std::vector<std::filesystem::path>& foldersPaths)
	{
		for (const std::filesystem::path& dirPath : foldersPaths)
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



	void EditorAssetManegerThreade::ClearLodeadAssetList()
	{
		std::map<AssetHandle, Ref<Asset>> mapCopy;
		{
			m_LoadedAssets.Read(
				[&mapCopy](const std::map<AssetHandle, Ref<Asset>>& map)
				{
					mapCopy = map;
					return true;
				}
			);

		}
		

		for (auto&[assetHandle, assetRef] : mapCopy)
		{
			this->UnLoadeFileAsset(assetHandle);
		}
	}

	bool EditorAssetManegerThreade::IsProjectDriectoryRelativ(const std::filesystem::path& assetPath)
	{
		size_t index = GetIndexProjectDriectory(assetPath);
		std::string assetPathStr = assetPath.string();
		size_t size = assetPathStr.size();

		bool isRelativ = index >= size;
		return isRelativ;
	}

	bool EditorAssetManegerThreade::IsProjectDriectoryAbsolut(const std::filesystem::path& assetPath)
	{
		size_t index = GetIndexProjectDriectory(assetPath);
		std::string assetPathStr = assetPath.string();
		size_t size = assetPathStr.size();
		
		bool isAbsult = assetPath.is_absolute();
		bool isProjectAbsult = index < size;
		return isAbsult || isProjectAbsult;
	}

	size_t EditorAssetManegerThreade::GetIndexProjectDriectory(const std::filesystem::path& assetPath)
	{
		Ref<Project> project = Project::GetActive();
		const std::filesystem::path& projectDirectory = project->GetActiveProjectDirectory();

		std::filesystem::path filePathDirParent = projectDirectory.parent_path();
		std::filesystem::path filePathDirParentName = filePathDirParent.filename();
		std::string projectDirectoryParentNameStr = filePathDirParentName.string();
		std::string assetPathStr = assetPath.string();

		size_t index = assetPathStr.find(projectDirectoryParentNameStr);
		return index;
	}

	std::filesystem::path EditorAssetManegerThreade::GetFilePathProjectDirectoryAbosult(const std::filesystem::path& assetPath)
	{
		if(EditorAssetManegerThreade::IsProjectDriectoryAbsolut(assetPath))
			return assetPath;

		Ref<Project> project = Project::GetActive();
		const std::filesystem::path& projectDirectory = project->GetActiveProjectDirectory();
		std::filesystem::path filePath = projectDirectory / assetPath;
	
		return filePath;
	}

	std::filesystem::path EditorAssetManegerThreade::GetFilePathProjectDirectoryRelative(const std::filesystem::path& assetPath)
	{
		if (EditorAssetManegerThreade::IsProjectDriectoryRelativ(assetPath))
			return assetPath;

		Ref<Project> project = Project::GetActive();
		const std::filesystem::path& projectDirectory = project->GetActiveProjectDirectory();

		std::filesystem::path assetRaltiv = std::filesystem::relative(assetPath, projectDirectory);
		return std::filesystem::path();
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
						[this, &mapHandleRegister](std::map<std::filesystem::path, AssetHandle>& mapPathRegister)
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
						return diferenc > 1.5;
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

	bool EditorAssetManegerThreade::IsFileAssetExist(const std::filesystem::path& path)
	{
		std::filesystem::path pathExist = path;


		if (path.is_relative())
		{
			std::string marker = Project::GetExpextedMarker(path);
			std::filesystem::path base = Project::GetAbsulteFilePathFormMarker(marker);
			pathExist = base / path;
			pathExist = pathExist.generic_string();
			RY_CORE_WARN("Maby the File-Path is not absolut \n\t{}\n\t{}", pathExist, path);
		}
		return std::filesystem::exists(pathExist);
	}

	bool EditorAssetManegerThreade::IsCurentAssetState(const std::filesystem::path& showPath) const
	{
		return m_FileChanges || showPath != m_CurentPath;
	}

	ContentBrowserItemesThreade EditorAssetManegerThreade::GetCurentAssetInformation(const std::filesystem::path& showPath)
	{
		std::string showPathGenaric = showPath.generic_string();

		if (IsDirectoryInRegistry(showPathGenaric))
		{
			ContentBrowserItemesThreade itemes;
			{

				
				m_DirectoryRegistry.ReadValue(showPathGenaric,
					[this, &itemes](const AssetFileDirectoryThreade& assetFileDirectory) -> bool
					{
						const std::vector<std::filesystem::path>& foldersPaths = assetFileDirectory.Folders;
						const std::vector<AssetHandle>& handles = assetFileDirectory.AssetFiles;

						itemes.reserve(foldersPaths.size() + foldersPaths.size());

						for (const std::filesystem::path& folderPath : foldersPaths)
						{
							itemes.emplace_back(
								AssetBrowserDataThreade(false, true,
									AssetHandle(0), AssetMetadata(),
									AssetType::None, "",
									AssetState::None, folderPath.filename().string(),
									folderPath, folderPath.string()
								)
							);
						}

						for (const AssetHandle handle : handles)
						{
							if (!IsAssetHandleValid(handle))
								continue;
							CheckAssetFileExist(handle);
							const AssetMetadata metadata = GetMetadata(handle);

							AssetType type = Asset::GetAssetTypeFromFilePath(metadata.FilePath);

							RY_CORE_ASSERT(!metadata.GetIntern());
							itemes.emplace_back(true, false,
								handle, metadata, type, Asset::GetAssetTypeDragAndDropName(type), metadata.State,
								metadata.FilePath.filename().string(),
								metadata.FilePath, metadata.FilePath.string());
						}

						return true;
					}
				);

			}
			{
				std::lock_guard<std::mutex> lockChange(m_ChangesMutex);
				std::lock_guard<std::mutex> lockPath(m_CurentPathMutex);
				m_FileChanges = false;
				m_CurentPath = showPathGenaric;
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
			m_HandleRegistry.Read(
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

	void EditorAssetManegerThreade::EventAsyncModified(const std::filesystem::path& path)
	{
		const AssetHandle handle = GetAssetHandle(path);
		
		if (ExexuteEvent(handle))
		{
			m_WorkingThreadMutex.lock();
			m_WorkingThread.emplace_back(std::async(std::launch::async, [this](AssetHandle handleLem) {
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

	void EditorAssetManegerThreade::EventAsyncAdded(const std::filesystem::path& path)
	{
		m_WorkingThreadMutex.lock();
		m_WorkingThread.emplace_back(std::async(std::launch::async, [this](const std::filesystem::path pathLem)
			{
				std::filesystem::path parent = pathLem.parent_path();
				RY_ASSET_WARN("Exexute Added Event -> Path: '{}' | Scane Dir: '{}'", pathLem.filename().string().c_str(), parent.string().c_str());
				ScaneDirectory(parent);
			}, path));
		m_WorkingThreadMutex.unlock();
	}

	void EditorAssetManegerThreade::EventAsyncRemoved(const std::filesystem::path& path)
	{
		RY_ASSET_WARN("Event Removed Thread is doing nothing! TODO!");
	}

	void EditorAssetManegerThreade::EventAsyncRenamedNew(const std::filesystem::path& path)
	{
		RY_ASSET_WARN("Event RenamedNew Thread is doing nothing! TODO!");
	}

	void EditorAssetManegerThreade::EventAsyncRenamedOld(const std::filesystem::path& path)
	{
		RY_ASSET_WARN("Event RenamedOld Thread is doing nothing! TODO!");
	}

	bool EditorAssetManegerThreade::CheckAssetFileExist(AssetHandle handle)
	{
		if (!IsAssetHandleValid(handle))
			return false;

		std::pair<bool, AssetState> result = m_HandleRegistry.ReadValue(handle,
			[this](const AssetMetadata& metaData) 
			{
				return std::pair<bool, AssetState>{ !IsAssetPathExtensionVaild(metaData.FilePath), metaData.State };
			}
		);
		AssetState state = result.second;
		if (result.first)
		{
			m_HandleRegistry.WriteValue(handle,
				[](AssetMetadata& metaData)
				{
					metaData.State = AssetState::LostConection;
				}
			);
		}
		else if (state == AssetState::LostConection || state == AssetState::Error)
		{
			m_HandleRegistry.WriteValue(handle,
				[](AssetMetadata& metaData)
				{
					metaData.State = AssetState::NotLoaded;
				}
			);
		}
		return true;
	}

	void EditorAssetManegerThreade::CreateAssetDirectory(const std::filesystem::path& path)
	{
		RY_CORE_ASSERT(IsDirectoryInRegistry(path), "Found folder in register alraedy or folder path is a file!");

		std::filesystem::path folderRelativPath = Project::GenarteProjectRaltivPath(path);
		std::string pathGenaric = folderRelativPath.generic_string();
		std::string name = folderRelativPath.filename().string();

	
		std::lock_guard<std::mutex> lockChang(m_ChangesMutex);
		std::lock_guard<std::mutex> lockPath(m_CurentPathMutex);
		if (m_CurentPath == folderRelativPath)
			m_FileChanges = true;

		m_RegestryChanges = true;
		
		
		AssetFileDirectoryThreade assetFileDirectory;
		assetFileDirectory.FolderName = name;
		assetFileDirectory.FolderPath = pathGenaric;

		m_DirectoryRegistry.Add(pathGenaric, assetFileDirectory);

		RY_ASSET_INFO("Add AssetFolder Names {} on location {}", name.c_str(), pathGenaric.c_str());
	}

	void EditorAssetManegerThreade::AddAssetFileToAssetDirectory(const std::filesystem::path& assetPath, const std::string& assetName, const std::string& assetPathString, AssetHandle handle)
	{
		std::string parentGenaric;
		std::string extension = assetPath.extension().string();
		if (extension.find("-", 0) < extension.size())
		{
			RY_ASSET_WARN("Deteced speciel file Extension on Asset {} on location {}", assetName.c_str(), assetPathString.c_str());
			parentGenaric = assetPathString.substr(0, assetPathString.size() - 2);
		}
		else
		{
			parentGenaric = assetPath.parent_path().generic_string();
		}

		if (!IsAssetHandleValid(handle))
			CreateFileAsset(parentGenaric);


		std::lock_guard<std::mutex> lockChang(m_ChangesMutex);
		m_FileChanges = true;
		m_RegestryChanges = true;
		if(!m_DirectoryRegistry.IsFound(parentGenaric))
			m_DirectoryRegistry.Add(parentGenaric, AssetFileDirectoryThreade());
		m_DirectoryRegistry.GetRefLemda([&](AssetFileDirectoryThreade& assetFileDirectory)
			{
				for (auto& files : assetFileDirectory.AssetFiles)
				{
					if (files == handle)
						return;
				}
				assetFileDirectory.AssetFiles.emplace_back(handle);
			}, parentGenaric);

		
	}

	void EditorAssetManegerThreade::AddDirectoryToParent(const std::filesystem::path& path)
	{
		std::filesystem::path folderRelativPath = Project::GenarteProjectRaltivPath(path);
		

		std::string origenelGenaric = folderRelativPath.generic_string();
		std::string origenelName = folderRelativPath.filename().string();
		std::string parentGenaric = folderRelativPath.parent_path().generic_string();

		if (!IsDirectoryInRegistry(origenelGenaric))
		{
			CreateAssetDirectory(origenelGenaric);
		}

		if (IsDirectoryInRegistry(parentGenaric))
		{
			std::lock_guard<std::mutex> lockChang(m_ChangesMutex);
			std::lock_guard<std::mutex> lockPath(m_CurentPathMutex);
			if (m_CurentPath == parentGenaric)
				m_FileChanges = true;
			m_RegestryChanges = true; 
			
			bool result = m_DirectoryRegistry.ReadValue(parentGenaric, [&origenelGenaric](const AssetFileDirectoryThreade& assetFileDirectory)
				{
					for (auto& folder : assetFileDirectory.Folders)
					{
						if (folder == origenelGenaric)
							return false;
					}
					return true;

					
				});
			if (result)
			{
				m_DirectoryRegistry.WriteValue(parentGenaric, 
					[&origenelGenaric](AssetFileDirectoryThreade& assetFileDirectory)
					{
						assetFileDirectory.Folders.emplace_back(origenelGenaric);
					}
				);
				
			}
			RY_ASSET_WARN("Add Childe Folder {} to Folder {}", std::filesystem::path(parentGenaric).filename().string(), origenelGenaric);
		}
		else
		{
			CreateAssetDirectory(parentGenaric);
		}
	}


	void EditorAssetManegerThreade::CreateFileAsset(const std::filesystem::path& path)
	{
		std::filesystem::path markedAssetPath = Project::GeanrateRealtivePathWitheMarker(path);
		
		std::string assetPathString = path.generic_string();
		std::filesystem::path assetPath = assetPathString;
		std::string assetName = assetPath.filename().string();

		if (!IsAssetHandleValid(assetPath) && IsAssetPathExtensionVaild(assetPath))
		{
			CreateNewFileAsset(assetName, assetPath, markedAssetPath);
		}
		else if (IsAssetHandleValid(assetPath))
		{
			
			if (!IsFileAssetExist(assetPath))
			{
				const AssetHandle handle = GetAssetHandle(assetPath);
				if (IsAssetLoaded(handle))
					UnLoadeFileAsset(handle);


				std::lock_guard<std::mutex> lockChang(m_ChangesMutex);
				std::lock_guard<std::mutex> lockPath(m_CurentPathMutex);

				m_HandleRegistry.GetRefLemda([this](AssetMetadata& metaData)
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
				AddAssetFileToAssetDirectory(assetPathString, assetName, assetPathString, handle);
			}


		}
		else if (!IsAssetPathExtensionVaild(assetPath))
		{
			RY_ASSET_WARN("Asset {} we don't add to regestry because extione {} is not AssetType", assetPathString, assetPath.extension().string());
		}
		else
		{
			RY_CORE_ASSERT(false, "Don't know how that can happen");
		}
	}

	void EditorAssetManegerThreade::CreateNewFileAsset(const std::string& name, const std::filesystem::path& path, const std::filesystem::path& pathMarker)
	{
		AssetMetadata metaData;
		metaData.SetActive(true);
		metaData.SetFilePath(path);
		metaData.Name = name;
		metaData.Type = Asset::GetAssetTypeFromFilePath(path);
		metaData.ChangeTime = AssetRegistry::GetCurrentTimeStr();
		metaData.State = IsFileAssetExist(path) ? AssetState::NotLoaded : AssetState::LostConection;

		AssetHandle handle;
		while (handle == 0 || IsAssetHandleValid(handle))
			handle = AssetHandle();

		std::string pathString = path.string();
		AddAssetFileToAssetDirectory(path, name, pathString, handle);

		{
			std::lock_guard<std::mutex> lock(m_ChangesMutex);
			std::lock_guard<std::mutex> lockPath(m_CurentPathMutex);
			if (m_CurentPath == path.parent_path())
				m_FileChanges = true;

			m_RegestryChanges = true;
		}
		m_HandleRegistry.Add(handle, metaData);
		m_PathRegistry.Add(metaData.AbsolutePath, handle);
		m_PathRegistry.Add(metaData.RealtivePath, handle);

		if(m_PathRegistry.IsFound(metaData.FilePath))
			m_PathRegistry.Add(metaData.FilePath, handle);
	}

	void EditorAssetManegerThreade::DeleateDirectory(const std::filesystem::path& path)
	{
		std::string pathGenaric = path.generic_string();
		if (IsDirectoryInRegistry(pathGenaric))
		{
			AssetFileDirectoryThreade dirRegT;
			dirRegT = m_DirectoryRegistry.GetCopy(pathGenaric);

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
				m_PathRegistry.Remove(metadata.RealtivePath);
				m_PathRegistry.Remove(metadata.AbsolutePath);

				if (m_PathRegistry.IsFound(metadata.FilePath))
					m_PathRegistry.Remove(metadata.FilePath);
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

	template<>
	inline AssetMangerMapMutex<AssetHandle, Ref<Asset>>::~AssetMangerMapMutex()
	{
		RY_CORE_ASSERT(!m_OutSideScope, "Mutex is alrady Set Globle");
		std::unique_lock writerLock(m_Mutex);
		m_Stop = true;
		if (!m_AssetMap.empty())
		{
			WriteAction(
				[this](std::map<AssetHandle, Ref<Asset>>& map)
				{
					for (auto& [key, asset] : map)
					{
						asset.reset();
					}
					map.clear();
				}
			);
		}
	}


	template<>
	inline AssetMangerLeftRightMap<AssetHandle, Ref<Asset>>::~AssetMangerLeftRightMap()
	{
		std::unique_lock writerLock(m_WriterMutex);
		m_Stop = true;
		
		WriteAction(
			[this](std::map<AssetHandle, Ref<Asset>>& map)
			{
				for (auto& [key, asset] : map)
				{
					asset.reset();
				}
				map.clear();
			}
		);
	}
}