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
		// return IsAssetInRegistry(handle) && GetMetadataConst(handle).Interale;
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
			}

			if(findDirectOnDisc)
			{
				metadata.State = AssetState::NotLoaded;
			}
			else
			{
				metadata.State = AssetState::LostConection;
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
					RY_CORE_WARN("This Shoud Not Happend!");
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
					RY_CORE_WARN("This Shoud Not Happend!");
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

		// metadata.Interale = true;
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
			RY_CORE_WARN("This Shoud Not Happend!");
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
				//fileDirectory.erase(delIndex);
			}
		}
	}

	void AssetRegistry::DeleteLocaleAsset(AssetHandle handle)
	{
		while(IsAssetInteral(handle))
			m_HandleRegistry.erase(handle);
	}

#pragma endregion


#if RY_EDITOR_ASSETMANGER_THREADE

#pragma region EditorAssetManagerThreade


	void EditorAssetManegerThreade::OnAttach()
	{
		RY_CORE_WARN("Begin EditorAssetManegerThreade OnAttach");
		DeserialzeAssetRegistry();
		const std::filesystem::path& baseDir = Project::GetActiveAssetDirectory();
		ScaneDirectory(baseDir);
		RY_CORE_INFO("Ende EditorAssetManegerThreade OnAttach");
	}

	void EditorAssetManegerThreade::OnDetach()
	{
		RY_CORE_WARN("Begin EditorAssetManegerThreade OnDetach");
		SerialzeAssetRegistry();
		RY_CORE_INFO("Ende EditorAssetManegerThreade OnDetach");
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
		// return  IsAssetHandleValid(handle) && GetMetadata(handle).Interale;
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
			return m_HandleRegistry.GetConstCopy(handle);
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
			return m_PathRegistry.GetConstCopy(path);
			
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

	Ref<Asset> EditorAssetManegerThreade::GetAsset(AssetHandle handle, bool async)
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

				// if (GetMetadata(handle).Interale)
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

							m_HandleRegistry.GetRefLemda([](AssetMetadata& metadata)
								{
									metadata.State = AssetState::Loading;
								}, handle);
							
							std::lock_guard<std::mutex> lockChange(m_ChangesMutex);
							m_FileChanges = true;
							m_RegestryChanges = true;
						}

						asset = AssetImporter::ImportAsset(handle, GetMetadata(handle), async);

						{
							
						}
							
						RY_CORE_ASSERT(asset, "Error on: 'EditorAssetManager::GetAsset' No Asset Lodead!");
						if (asset)
						{
							std::lock_guard<std::mutex> lockChange(m_ChangesMutex);
							m_HandleRegistry.GetRefLemda([](AssetMetadata& metadata)
							{
								metadata.State = AssetState::Ready;
								metadata.LoadingInTime = std::chrono::steady_clock::now();
							}, handle);
							
							asset->Handle = handle;
							
							m_LoadedAssets.Add(handle, asset);

							m_FileChanges = true;
							m_RegestryChanges = true;
						}
						else
						{
							std::lock_guard<std::mutex> lockChange(m_ChangesMutex);

							m_HandleRegistry.GetRefLemda([](AssetMetadata& metadata)
								{
									metadata.State = AssetState::Error;
									metadata.LoadingInTime = std::chrono::steady_clock::now();
								}, handle);

							m_FileChanges = true;
							m_RegestryChanges = true;
						}
						break;
					}
					case AssetState::Loading:
					{
						
						do {
							using namespace std::chrono_literals;
							std::this_thread::sleep_for(100ms);
							RY_CORE_WARN("Wait For Asset To Loade");
						} while (GetMetadata(handle).State == AssetState::Loading);

						asset = GetAsset(handle, async);
						break;
						
					}

					
					case AssetState::Ready:
					{
						asset = GetAsset(handle, async);
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
		return GetAsset(handle, true);
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
				
				m_HandleRegistry.GetRefLemda([](AssetMetadata& metadata) {
					metadata.State = AssetState::NotLoaded;
				}, handle);
				
				m_FileChanges = true;
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
		return GetAsset(handle, true);
	}


#pragma region LocaleAssetsDataMage

	void EditorAssetManegerThreade::CreatLocaleAsset(Ref<Asset> asset, AssetMetadata& metadata, AssetHandle handle)
	{		
		// metadata.Interale = true;
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

		for (auto& p : std::filesystem::directory_iterator(curentPath))
		{
			const auto& path = p.path();
			if (p.is_directory())
			{
				AddDirectoryToParent(path);
				ScaneDirectory(path);
			}
			else
			{
				// assetDirectory.Files.emplace_back(m_AssetManger->AddFileToRegistry(path));
				if (Asset::GetAssetTypeFromFilePath(path) != AssetType::None)
				{
					CreateFileAsset(path);
				}
			}
		}

	}

	void EditorAssetManegerThreade::ReLoadeAsset(AssetHandle handle, bool async)
	{
		RY_CORE_ASSERT(handle != 0, "This Handle is not Accipteble");
		if (IsAssetLoaded(handle))
		{
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(20ms);
			{
				std::lock_guard<std::mutex> lockChange(m_ChangesMutex);
				int* aboutePtr = nullptr;
				int aboute = 0;
				aboutePtr = &aboute;
				m_HandleRegistry.GetRefLemda([aboutePtr](AssetMetadata& metadata)
					{
						if (metadata.State == AssetState::Updateing)
							*aboutePtr = 1;
						else
							metadata.State = AssetState::Updateing;
					}, handle);

				if (aboute == 1)
				{
					RY_CORE_ERROR("About Reloading! Already Reloding somewhere else!");
					// RY_CORE_ERROR("About Reloading!");
					return;
				}

				m_FileChanges = true;
				m_RegestryChanges = true;
				RY_CORE_WARN("Beginn reloding Asset");
			} 
			

			const AssetMetadata metadataC = GetMetadata(handle);
			
			AssetImporter::ReLoadeAsset(handle, metadataC, async);

			{
				std::lock_guard<std::mutex> lockChange(m_ChangesMutex);

				m_HandleRegistry.GetRefLemda([](AssetMetadata& metadata)
					{
						metadata.State = AssetState::Ready;
						metadata.LoadingInTime = std::chrono::steady_clock::now();
					}, handle);
				m_FileChanges = true;
				m_RegestryChanges = true;
			}

			RY_CORE_INFO("Asset: {0} Is Now Reloded", GetMetadata(handle).FilePath.string().c_str());
		}
		else
		{
			RY_CORE_INFO("don't need Reloding Asset: {0}", GetMetadata(handle).FilePath.string().c_str());
		}
	}

	void EditorAssetManegerThreade::ReLoadeAsset(const std::filesystem::path& path, bool async)
	{
		const AssetHandle handle = GetAssetHandle(path);
		ReLoadeAsset(handle, async);
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


	void EditorAssetManegerThreade::SerialzeAssetRegistry()
	{
		//std::scoped_lock<std::mutex> lock(m_RegistryThreedQueueMutex);
		RY_CORE_INFO("SerialzeAsseRegistry Asset Regestriey");
		std::filesystem::path path = Project::GetActiveAssetRegistryPath();

		m_HandleRegistry.GlobleMutexBegin();
		std::map<AssetHandle, AssetMetadata>* map = m_HandleRegistry.GetMapPtr();
		EditorAssetMangerSerialzation::SerilzeThread(path, map);
		m_HandleRegistry.GlobleMutexEnde();

	}

	bool EditorAssetManegerThreade::DeserialzeAssetRegistry()
	{
		RY_CORE_INFO("DeserialzeAssetRegistry Asset Regestriey");
		{
			std::filesystem::path path = Project::GetActiveAssetRegistryPath();
			m_HandleRegistry.GlobleMutexBegin();
			m_PathRegistry.GlobleMutexBegin();

			std::map<AssetHandle, AssetMetadata>* mapHR = m_HandleRegistry.GetMapPtr();
			std::map<std::filesystem::path, AssetHandle>* mapPR = m_PathRegistry.GetMapPtr();

			EditorAssetMangerSerialzation::DeserilzeThread(path, mapHR, mapPR);
			
			m_HandleRegistry.GlobleMutexEnde();
			m_PathRegistry.GlobleMutexEnde();
		}
		
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
				// case Rynex::AssetState::NotLoaded:
					return false;
				case Rynex::AssetState::LostConection:
				case Rynex::AssetState::Error:
					RY_CORE_INFO("Mayby Conection to {} is Now longer lost!", metadata.FilePath.string().c_str());
				case Rynex::AssetState::Ready:
				// case Rynex::AssetState::Uploading:
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

	bool EditorAssetManegerThreade::IsCurentAssetState(const std::filesystem::path& showPath) const
	{
		std::lock_guard<std::mutex> lockPath(m_CurentPathMutex);
		std::lock_guard<std::mutex> lockChange(m_ChangesMutex);
		return m_FileChanges || showPath != m_CurentPath;
	}

	ContentBrowserItemesThreade EditorAssetManegerThreade::GetCurentAssetInformation(const std::filesystem::path& showPath)
	{
		std::string showPathGenaric = showPath.generic_string();
		if (IsDirectoryInRegistry(showPathGenaric))
		{
			ContentBrowserItemesThreade itemes;
			{
				m_DirectoryRegistry.GlobleMutexBegin();
				AssetFileDirectoryThreade& assetFileDirectory = m_DirectoryRegistry.GetRef(showPathGenaric);
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
							folderPath, folderPath.string()));
				}

				for (const AssetHandle handle : handles)
				{
					if (!IsAssetHandleValid(handle))
						continue;
					CheckAssetFileExist(handle);
					const AssetMetadata metadata = GetMetadata(handle);

					AssetType type = Asset::GetAssetTypeFromFilePath(metadata.FilePath);

					// RY_CORE_ASSERT(!metadata.Interale);
					RY_CORE_ASSERT(!metadata.GetIntern());
					itemes.emplace_back(true, false,
						handle, metadata, type, Asset::GetAssetTypeDragAndDropName(type), metadata.State,
						metadata.FilePath.filename().string(),
						metadata.FilePath, metadata.FilePath.string());
				}
				m_DirectoryRegistry.GlobleMutexEnde();
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
			m_HandleRegistry.GlobleMutexBegin();
			std::lock_guard<std::mutex> lockChange(m_ChangesMutex);
			std::map<AssetHandle, AssetMetadata>* map = m_HandleRegistry.GetMapPtr();
			
			itemes.reserve(map->size());
			for (const auto&[handle, metadata] : *map)
			{
				RY_CORE_ASSERT(handle, "Handle withe zero!");
				itemes.emplace_back(handle, metadata);
			}
			
			m_RegestryChanges = false;
			m_HandleRegistry.GlobleMutexEnde();
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
				RY_CORE_WARN("Exexute Modified Event -> Asset: '{}'", metadata.FilePath.string().c_str());
				ReLoadeAsset(handleLem, true);
			}, handle));
			m_WorkingThreadMutex.unlock();
		}
		else
		{
			const AssetMetadata metadata = GetMetadata(handle);
			RY_CORE_WARN("Blocked Modified Event -> Asset: '{}'", metadata.FilePath.string().c_str());
		}

	}

	void EditorAssetManegerThreade::EventAsyncAdded(const std::filesystem::path& path)
	{
		m_WorkingThreadMutex.lock();
		m_WorkingThread.emplace_back(std::async(std::launch::async, [this](const std::filesystem::path pathLem)
			{
				std::filesystem::path parent = pathLem.parent_path();
				RY_CORE_WARN("Exexute Added Event -> Path: '{}' | Scane Dir: '{}'", pathLem.filename().string().c_str(), parent.string().c_str());
				ScaneDirectory(parent);
			}, path));
		m_WorkingThreadMutex.unlock();
	}

	void EditorAssetManegerThreade::EventAsyncRemoved(const std::filesystem::path& path)
	{
		RY_CORE_WARN("Event Removed Thread is doing nothing! TODO!");
	}

	void EditorAssetManegerThreade::EventAsyncRenamedNew(const std::filesystem::path& path)
	{
		RY_CORE_WARN("Event RenamedNew Thread is doing nothing! TODO!");
	}

	void EditorAssetManegerThreade::EventAsyncRenamedOld(const std::filesystem::path& path)
	{
		RY_CORE_WARN("Event RenamedOld Thread is doing nothing! TODO!");
	}

	bool EditorAssetManegerThreade::CheckAssetFileExist(AssetHandle handle)
	{
		if (!IsAssetHandleValid(handle))
			return false;
		m_HandleRegistry.GlobleMutexBegin();
		AssetMetadata& metaData = m_HandleRegistry.GetRef(handle);

		if (!IsAssetPathExtensionVaild(metaData.FilePath))
		{
			metaData.State = AssetState::LostConection;
			m_HandleRegistry.GlobleMutexEnde();
			return false;
		}
		if(metaData.State == AssetState::LostConection || metaData.State == AssetState::Error)
			metaData.State = AssetState::NotLoaded;
		m_HandleRegistry.GlobleMutexEnde();
		return true;
	}

	void EditorAssetManegerThreade::CreateAssetDirectory(const std::filesystem::path& path)
	{
		std::string pathGenaric = path.generic_string();
		std::string name = path.filename().string();

		if (!IsDirectoryInRegistry(pathGenaric))
		{
			{
				std::lock_guard<std::mutex> lockChang(m_ChangesMutex);
				m_FileChanges = true;
				m_RegestryChanges = true;
				m_DirectoryRegistry.GetRefLemda([name, pathGenaric](AssetFileDirectoryThreade& assetFileDirectory)
				{
					assetFileDirectory.FolderName = name;
					assetFileDirectory.FolderPath = pathGenaric;
				}, pathGenaric);
				
			}
			
		
			RY_CORE_INFO("Add AssetFolder Names {} on location {}", name.c_str(), pathGenaric.c_str());
			// AddDirectoryToParent(pathGenaric);
		}

	}

	void EditorAssetManegerThreade::AddAssetFileToAssetDirectory(const std::filesystem::path& assetPath, const std::string& assetName, const std::string& assetPathString, AssetHandle handle)
	{
		std::string parentGenaric;
		std::string extension = assetPath.extension().string();
		if (extension.find("-", 0) < extension.size())
		{
			RY_CORE_WARN("Deteced speciel file Extension on Asset {} on location {}", assetName.c_str(), assetPathString.c_str());
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
				
				// RY_CORE_INFO("Add Asset on parent folder {}", assetPathString.c_str());
			}, parentGenaric);

		
	}

	void EditorAssetManegerThreade::AddDirectoryToParent(const std::filesystem::path& path)
	{
		std::string& origenelGenaric = path.generic_string();
		std::string& origenelName = path.filename().string();
		std::string& parentGenaric = path.parent_path().generic_string();

		if (!IsDirectoryInRegistry(origenelGenaric)&& !path.has_extension())
		{
			std::lock_guard<std::mutex> lockChang(m_ChangesMutex);
			m_FileChanges = true;
			m_RegestryChanges = true;
			if(!m_DirectoryRegistry.IsFound(origenelGenaric))
				m_DirectoryRegistry.Add(origenelGenaric, AssetFileDirectoryThreade());
			m_DirectoryRegistry.GetRefLemda([origenelName, origenelGenaric](AssetFileDirectoryThreade dirReg)
				{
					dirReg.FolderName = origenelName;
					dirReg.FolderPath = origenelGenaric;
				}, origenelGenaric);
			
		}

		if (IsDirectoryInRegistry(parentGenaric))
		{
			std::lock_guard<std::mutex> lockChang(m_ChangesMutex);
			m_FileChanges = true;
			m_RegestryChanges = true; 
			
			m_DirectoryRegistry.GetRefLemda([origenelGenaric](AssetFileDirectoryThreade& assetFileDirectory)
				{
					bool findeFile = false;
					for (auto& folder : assetFileDirectory.Folders)
					{
						if (folder == origenelGenaric)
							return;
					}

					assetFileDirectory.Folders.emplace_back(origenelGenaric);
				}, parentGenaric);

			RY_CORE_WARN("Add Childe Folder {} to Folder {}", std::filesystem::path(parentGenaric).filename().string().c_str(), origenelGenaric.c_str());
		}
		else
		{
			CreateAssetDirectory(parentGenaric);

			std::lock_guard<std::mutex> lockChang(m_ChangesMutex);
		}
	}


	void EditorAssetManegerThreade::CreateFileAsset(const std::filesystem::path& path)
	{
		std::string assetPathString = path.generic_string();
		std::filesystem::path assetPath = assetPathString;
		std::string assetName = assetPath.filename().generic_string();

		if (!IsAssetHandleValid(assetPath) && IsAssetPathExtensionVaild(assetPath))
		{
			AssetMetadata metaData;
			// metaData.Aktive = true;
			metaData.SetActive(true);
			metaData.FilePath = assetPath;
			metaData.Name = assetName;
			metaData.Type = Asset::GetAssetTypeFromFilePath(assetPath);

			metaData.State = IsFileAssetExist(assetPath) ? AssetState::NotLoaded : AssetState::LostConection;

			AssetHandle handle;
			while (handle == 0 || IsAssetHandleValid(handle))
				handle = AssetHandle();

			AddAssetFileToAssetDirectory(assetPathString, assetName, assetPathString, handle);

			{
				std::lock_guard<std::mutex> lock(m_ChangesMutex);
				
				m_FileChanges = true;
				m_RegestryChanges = true;
			}
			m_HandleRegistry.Add(handle, metaData);
			m_PathRegistry.Add(assetPath, handle);

		}
		else if (IsAssetHandleValid(assetPath))
		{
			// RY_CORE_INFO("Check Asset Alrady Existing!");
			// check State
			
			if (!IsFileAssetExist(assetPath))
			{
				const AssetHandle handle = GetAssetHandle(assetPath);
				if (IsAssetLoaded(handle))
					UnLoadeFileAsset(handle);

				m_HandleRegistry.GetRefLemda([](AssetMetadata& metaData) 
					{
						metaData.State = AssetState::LostConection;
					}, handle);
				
				std::lock_guard<std::mutex> lockChang(m_ChangesMutex);
				m_FileChanges = true;
				m_RegestryChanges = true;
			}
			else
			{
				const AssetHandle handle = GetAssetHandle(assetPath);
				if (!IsAssetLoaded(handle))
				{
					m_HandleRegistry.GetRefLemda([this](AssetMetadata& metaData)
						{
							if (metaData.State != AssetState::NotLoaded)
							{
								metaData.State = AssetState::LostConection;
								m_FileChanges = true;
								m_RegestryChanges = true;
							}
						}, handle);
				}
				else
				{
					m_HandleRegistry.GetRefLemda([this](AssetMetadata& metaData)
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
			RY_CORE_WARN("Asset {} we don't add to regestry because extione {} is not AssetType", assetPathString, assetPath.extension().string());
		}
		else
		{
			RY_CORE_ASSERT(false, "Don't know how that can happen");
		}
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
		std::lock_guard<std::mutex> lockChange(m_ChangesMutex);
		m_HandleRegistry.GetRefLemda([](AssetMetadata& metadata)
			{
				metadata.State = AssetState::NotLoaded;
				RY_CORE_WARN("Try to UnLoadeing Asset {} from disc path {}", metadata.FilePath.string().c_str(), metadata.Name.c_str());
			}, handle);
		m_LoadedAssets.Remove(handle);

		
		m_FileChanges = true;
		m_RegestryChanges = true;
	}

#pragma endregion

#pragma endregion

#else

#pragma region EditorAssetManager

	EditorAssetManager::EditorAssetManager()
	{
	}

	void EditorAssetManager::OnAttach()
	{
		RY_CORE_WARN("Begin EditorAssetManager OnAttach");
		DeserialzeAssetRegistry();
		RY_CORE_INFO("Ende EditorAssetManager OnAttach");
	}

	void EditorAssetManager::OnDetach()
	{
		RY_CORE_WARN("Begin EditorAssetManager OnDetach");
		SerialzeAssetRegistry();
		RY_CORE_INFO("Ende EditorAssetManager OnDetach");
	}

	Ref<Asset> EditorAssetManager::GetAsset(AssetHandle handle)
	{
		if (m_AssetRegistry.IsAssetInRegistry(handle))
		{
			Ref<Asset> asset = Ref<Asset>();
			if (IsAssetLoaded(handle))
			{
				asset = m_LoadedAssets.at(handle);
			}
			else if(!IsAssetInteral(handle))
			{
				if(GetMetadata(handle).Interale)
				{
					RY_CORE_ASSERT(false, "Error on: 'EditorAssetManager::GetAsset' Asset is known In Regestry as Interale-Asset But got not Loadede! This cinde off error is a def Error!");
					return nullptr;
				}
				AssetMetadata& metadata = m_AssetRegistry.GetMetadata(handle);
				metadata.State = AssetState::Loading;
				asset = AssetImporter::ImportAsset(handle, metadata);
				metadata.State = AssetState::Ready;
				RY_CORE_ASSERT(asset, "Error on: 'EditorAssetManager::GetAsset' No Asset Lodead!");
				asset->Handle = handle;
				m_LoadedAssets[handle] = asset;
			}
			else if (IsAssetInteral(handle))
			{
				RY_CORE_ASSERT(false, "Error on: 'EditorAssetManager::GetAsset' Asset is known In Regestry as Interale-Asset But got not Loadede! This cinde off error is a def Error!");
				return nullptr;
			}
			else
			{
				RY_CORE_ASSERT(false, "Error on: 'EditorAssetManager::GetAsset' I Dont't know some isuse kan go to hear!");
				return nullptr;
			}
			return asset;
		}
		
		RY_CORE_ASSERT(false, "Error on: 'EditorAssetManager::GetAsset' No Asset handle found!");
		return nullptr;
	}

	Ref<Asset> EditorAssetManager::GetAssetAsync(AssetHandle handle)
	{
		return GetAsset(handle);
	}

	Ref<Asset> EditorAssetManager::GetAsset(const std::filesystem::path& path)
	{
		return GetAsset(m_AssetRegistry.GetAssetHandle(path));
	}


	bool EditorAssetManager::IsAssetHandleValid(AssetHandle handle) const 
	{
		return m_AssetRegistry.IsAssetInRegistry(handle);
	}

	bool EditorAssetManager::IsAssetHandleValid(const std::filesystem::path& path) const 
	{
		return m_AssetRegistry.IsAssetInRegistry(path);
	}

	bool EditorAssetManager::IsAssetLoaded(AssetHandle handle) const 
	{
		return m_LoadedAssets.find(handle) != m_LoadedAssets.end() && (handle!=0);
	}

	bool EditorAssetManager::IsAssetLoaded(const std::filesystem::path& path) const
	{
		return IsAssetLoaded(m_AssetRegistry.IsAssetInRegistry(path));
	}
	
	bool EditorAssetManager::IsAssetInteral(AssetHandle handle) const
	{
		return m_AssetRegistry.IsAssetInteral(handle);
	}

	AssetHandle EditorAssetManager::CreatLocaleAsset(Ref<Asset> asset, AssetMetadata& metadata)
	{
		AssetHandle handle = m_AssetRegistry.CreatLocaleAsset(asset, metadata);
		m_LoadedAssets[handle] = asset;
		if(IsAssetInteral(handle))
			return handle;
		return AssetHandle(0);
	}

	Ref<Asset> EditorAssetManager::GetLocaleAsset(AssetHandle handle)
	{
		RY_CORE_ASSERT(false, "This funktion is not any longer Sepoted! Use the funktion GetAsset");
		if (IsAssetLoaded(handle) && !IsAssetHandleValid(handle))
			return m_LoadedAssets.at(handle);
		return nullptr;
	}

	void EditorAssetManager::DeleteLocaleAsset(AssetHandle handle)
	{
		if (IsAssetLoaded(handle) && !IsAssetHandleValid(handle))
			m_LoadedAssets.erase(handle);
	}


	void EditorAssetManager::CreateAsset(const std::filesystem::path& path, Ref<Asset>& asset, AssetMetadata metadata)
	{
		std::string extensionString = path.extension().string();
		if (!(extensionString.rfind(".ryframe-", 0) == 0 || extensionString.rfind(".ryarray-", 0) == 0 || extensionString == ".ryarray-i"))
		{
			if (IsAssetHandleValid(path))
			{
				// std::string extension = path.extension().string();
				// std::string fileName = path.filename().string();
				// size_t posL = fileName.find(extension);
				// fileName.erase(posL, fileName.end());
				// path = path
				RY_CORE_ASSERT(false, "new Crated Asset is Alrady in path existens!");
			}
			while (IsAssetHandleValid(asset->Handle))
			{
				asset->Handle = AssetHandle();
				RY_CORE_WARN("Gerat new AssetHandle Ther is a identikle Asset Alrady");
			}
		}
		
		if (IsAssetHandleValid(path))
			asset->Handle = GetAssetHandle(path);
		else
			CreateAsset(path, asset->Handle, metadata);
		
		
		
		if (IsAssetHandleValid(asset->Handle))
		{
			if (!IsAssetLoaded(asset->Handle))
			{
				AssetMetadata& meta = m_AssetRegistry.GetMetadata(asset->Handle);
				m_LoadedAssets[asset->Handle] = asset;
				meta.State = AssetState::Ready;
			}
		}
		else
		{
			RY_CORE_ASSERT(false, "new Crated Asset is not identikle There!");
		}
	}

	

	void EditorAssetManager::ReLoadeAsset(AssetHandle handle)
	{
		RY_CORE_ASSERT(handle != 0, "This Handle is not Accipteble");
		RY_CORE_WARN("Reloning Asset: {0}", GetMetadata(handle).FilePath.string().c_str());
		if (IsAssetLoaded(handle))
		{
			AssetMetadata& metadata = GetMetadata(handle);
			metadata.State = AssetState::Updateing;
			AssetImporter::ReLoadeAsset(handle,metadata);
			metadata.State = AssetState::Ready;
			
			RY_CORE_INFO("Asset: {0} Is Now Reloded", GetMetadata(handle).FilePath.string().c_str());
		}
		else
		{
			RY_CORE_INFO("don't need Reloding Asset: {0}", GetMetadata(handle).FilePath.string().c_str());
		}
		
	}

	void EditorAssetManager::ReLoadeAsset(const std::filesystem::path& path)
	{	
		AssetHandle handle = GetAssetHandle(path);
		if (handle != 0)
			ReLoadeAsset(handle);
		else
			RY_CORE_ERROR("Try to Realoding On Path: {0} but It is not An Rynex Asset probebly!", path.string().c_str());
	}

	void EditorAssetManager::LoadeDownAsset(AssetHandle handle)
	{
		RY_CORE_ASSERT(handle != 0, "Asset is not known!");
		RY_CORE_WARN("Down Lode Asset: {0}", GetMetadata(handle).FilePath.string().c_str());
		if (IsAssetLoaded(handle))
		{
			AssetMetadata& metadata = GetMetadata(handle);
			metadata.State = AssetState::Ready;
			m_LoadedAssets[handle].reset();
			m_LoadedAssets.erase(handle);
			metadata.State = AssetState::NotLoaded;
		}
		else
		{
			RY_CORE_INFO("don't can down lownding Asset: {0}", GetMetadata(handle).FilePath.string().c_str());
		}
	}

	void EditorAssetManager::LoadeDownAsset(const std::filesystem::path& path)
	{
		LoadeDownAsset(GetAssetHandle(path));
	}

	void EditorAssetManager::OnLoadeAsset(AssetHandle handle)
	{
		if (IsAssetHandleValid(handle) && IsAssetLoaded(handle))
		{
			AssetMetadata& metadata = GetMetadata(handle);
			m_LoadedAssets[handle].reset();
			metadata.State = AssetState::Ready;
			m_LoadedAssets.erase(handle);
			metadata.State = AssetState::NotLoaded;
		}
	}

	void EditorAssetManager::OnLoadeAsset(const std::filesystem::path& path)
	{
		AssetHandle handle = GetAssetHandle(path);
		if (handle != 0)
			OnLoadeAsset(handle);
		else
			RY_CORE_ERROR("Try to Realoding On Path: {0} but It is not An Rynex Asset probebly!", path.string().c_str());
	}

	

	void EditorAssetManager::SerialzeAssetRegistry()
	{
		//std::scoped_lock<std::mutex> lock(m_RegistryThreedQueueMutex);
		RY_CORE_INFO("SerialzeAsseRegistry Asset Regestriey");
		auto path = Project::GetActiveAssetRegistryPath();

		EditorAssetMangerSerialzation::Serilze(path, m_AssetRegistry);
	}

	bool EditorAssetManager::DeserialzeAssetRegistry()
	{
		RY_CORE_INFO("DeserialzeAssetRegistry Asset Regestriey");
		auto path = Project::GetActiveAssetRegistryPath();
		
		EditorAssetMangerSerialzation::Deserilze(path, m_AssetRegistry);
		return true;
	}

#endif

#pragma endregion

}