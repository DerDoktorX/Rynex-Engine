#include "rypch.h"
#include "EditorAssetManager.h"

#include "Base/AssetManager.h"
#include "Base/AssetImporter.h"
#include "Rynex/Project/Project.h"

#include "Rynex/Serializers/EditorAssetMangerSerialzation.h"
#include <Rynex/Renderer/API/Texture.h>

namespace Rynex {

#pragma region AssetRegistry

	bool AssetRegistry::IsAssetInRegistry(AssetHandle handle) const
	{
		return handle!=0 && m_HandleRegistry.find(handle) != m_HandleRegistry.end();
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
		return IsAssetInRegistry(handle) && GetMetadataConst(handle).Interale;
	}

	void AssetRegistry::CreateAsset(const std::filesystem::path& path, AssetHandle handle, AssetMetadata metadata, bool findDirectOnDisc)
	{
		if (!IsAssetInRegistry(path) && IsAssetPath(path))
		{
			
			if (!metadata)
			{
				metadata.Aktive = true;
				metadata.FilePath = path.generic_string();
				metadata.LoadingTime;
				metadata.Name = path.filename().generic_string();
				metadata.Type = GetAssetTypeFromFilePath(path);
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
					assetFileDirectory.Files.push_back(handle);
				}
				else
				{
					AssetFileDirectory& assetFileDirectory = m_DirectoryRegistry[parentGenaric];
					assetFileDirectory.Files.push_back(handle);
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
					assetFileDirectory.Files.push_back(handle);
				}
				else
				{
					AssetFileDirectory& assetFileDirectory = m_DirectoryRegistry[parentGenaric];
					assetFileDirectory.Files.push_back(handle);
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
		
	}

	AssetHandle AssetRegistry::CreatLocaleAsset(Ref<Asset>& asset, AssetMetadata& metadata)
	{
		while (asset->Handle == 0 && IsAssetInRegistry(asset->Handle))
			asset->Handle = AssetHandle();

		metadata.Interale = true;
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
					return true;
			}
			assetFileDirectory.Folders.push_back(pathGenaric);
			return true;
		}
		else
		{
			AssetFileDirectory& assetFileDirectory = m_DirectoryRegistry[parentGenaric];
			assetFileDirectory.FolderName = path.parent_path().filename().generic_string();
			assetFileDirectory.FolderPath = parentGenaric;
			assetFileDirectory.Folders.push_back(path.generic_string());
			RY_CORE_WARN("This Shoud Not Happend!");
			return true;
		}
	}

	AssetHandle AssetRegistry::GetAssetHandle(const std::filesystem::path& path)
	{
		std::string& pathGenaric = path.generic_string();
		if (IsAssetInRegistry(pathGenaric))
		{
			return m_PathRegistry[pathGenaric];
		}
		else if(IsAssetPath(path) && 0 != (int)GetAssetTypeFromFilePath(path))
		{
			AssetHandle handle;
			do
			{
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
			return m_HandleRegistry[handle];
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
	
#pragma region EditorAssetManager

	EditorAssetManager::EditorAssetManager()
	{
		OnAttach();
	}

	void EditorAssetManager::OnAttach()
	{
		DeserialzeAssetRegistry();
	}

	void EditorAssetManager::OnDetach()
	{
		SerialzeAssetRegistry();
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
					RY_CORE_ASSERT(false, "Error on: 'EditorAssetManager::GetAsset' Asset is known In Regestry as Interale-Asset But got not Loadede! This cinde off error is a def Error");
					return nullptr;
				}
				AssetMetadata& metadata = m_AssetRegistry.GetMetadata(handle);
				metadata.State = AssetState::Loading;
				asset = AssetImporter::ImportAsset(handle, metadata);
				metadata.State = AssetState::Ready;
				asset->Handle = handle;
				RY_CORE_ASSERT(asset, "Error on: 'EditorAssetManager::GetAsset' No Asset Lodead!");
				m_LoadedAssets[handle] = asset;
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
		
		RY_CORE_ASSERT(false, "Error on: 'EditorAssetManager::GetAsset' No Asset handle found!");
		return nullptr;
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

	bool EditorAssetManager::IsAssetInteral(AssetHandle handle) const
	{
		return m_AssetRegistry.IsAssetInteral(handle);
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
			metadata.State = AssetState::Loading;
			m_LoadedAssets[handle].reset();
			m_LoadedAssets.erase(handle);
			metadata.State = AssetState::Ready;
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

#pragma endregion


}