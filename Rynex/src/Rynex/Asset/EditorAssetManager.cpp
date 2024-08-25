#include "rypch.h"
#include "EditorAssetManager.h"

#include "Base/AssetImporter.h"
#include "Rynex/Serializers/YAML.h"
#include "Rynex/Project/Project.h"

#include <yaml-cpp/emitfromevents.h>
#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>

namespace Rynex {


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

	void AssetRegistry::CreateAsset(const std::filesystem::path& path, AssetHandle handle, AssetMetadata metadata, bool findDirectOnDisc)
	{
		
		if (!IsAssetInRegistry(path) )
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
		else if (IsAssetInRegistry(path)&& findDirectOnDisc)
		{
			AssetMetadata& metadata = m_HandleRegistry[GetAssetHandle(path)];
			metadata.State = AssetState::NotLoaded;
		}
	}

	bool AssetRegistry::AddDirectoryToParent(const std::filesystem::path& path)
	{
		std::string& parentGenaric = path.parent_path().generic_string();
		if (IsDirectoryInRegistry(parentGenaric))
		{
			AssetFileDirectory& assetFileDirectory = m_DirectoryRegistry[parentGenaric];
			assetFileDirectory.Folders.push_back(path.generic_string());
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
		AssetHandle handle = AssetHandle();
		CreateAsset(path, handle);
		return handle;
	}

	AssetMetadata& AssetRegistry::GetMetadata(AssetHandle handle)
	{
		if (IsAssetInRegistry(handle))
		{
			return m_HandleRegistry[handle];
		}
		return AssetMetadata();
	}

	AssetMetadata& AssetRegistry::GetMetadata(const std::filesystem::path& path)
	{
		return GetMetadata(GetAssetHandle(path));
	}

	bool DeserialzeAsset(const YAML::detail::iterator_value& node, AssetRegistry& assetRegistry)
	{
		AssetHandle handle = node["Handle"].as<uint64_t>();

		AssetMetadata metadata;
		metadata.FilePath = node["FilePath"].as<std::string>();
		metadata.Type = AssetTypeFromString(node["Type"].as<std::string>());
		metadata.Name = node["Name"].as<std::string>();
		metadata.Aktive = true;
		
		assetRegistry.CreateAsset(metadata.FilePath, handle, metadata, false);
		return false;
	}

	
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////


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
		SerialzeAsseRegistry();
	}

	Ref<Asset> EditorAssetManager::GetAsset(AssetHandle handle)
	{
		if (m_AssetRegistry.IsAssetInRegistry(handle))
		{
			Ref<Asset> asset = Ref<Asset>();
			if (m_LoadedAssets.find(handle) != m_LoadedAssets.end())
			{
				asset = m_LoadedAssets.at(handle);
			}
			else
			{
				AssetMetadata& metadata = m_AssetRegistry.GetMetadata(handle);
				metadata.State = AssetState::Loading;
				asset = AssetImporter::ImportAsset(handle, metadata);
				metadata.State = AssetState::Ready;
				asset->Handle = handle;
				if (!asset) {}
				m_LoadedAssets[handle] = asset;
			}
			return asset;
		}
		RY_CORE_ASSERT(false, "Error on: 'EditorAssetManager::GetAsset' No Asset handle found!");
		return Ref<Asset>();
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
		return m_LoadedAssets.find(handle) != m_LoadedAssets.end();
	}

	bool EditorAssetManager::IsAssetLoaded(const std::filesystem::path& path) const
	{
		return IsAssetLoaded(m_AssetRegistry.IsAssetInRegistry(path));
	}


	YAML::Emitter& operator<<(YAML::Emitter& out, const std::string_view& v)
	{
		RY_PROFILE_FUNCTION();
		out << std::string(v.data(), v.size());
		return out;
	}

	void EditorAssetManager::SerialzeAsseRegistry()
	{
		//std::scoped_lock<std::mutex> lock(m_RegistryThreedQueueMutex);
		RY_CORE_INFO("SerialzeAsseRegistry Asset Regestriey");
		RY_PROFILE_FUNCTION();
		auto path = Project::GetActiveAssetRegistryPath();

		YAML::Emitter out;
		{
			out << YAML::BeginMap;
			out << YAML::Key << "AssetRegistry" << YAML::Value;
			out << YAML::BeginSeq;
			for (const auto& [handle, metadata] : m_AssetRegistry.GetHandleRegistry())
			{
				out << YAML::BeginMap;
				out << YAML::Key << "Handle" << YAML::Value << handle;
				out << YAML::Key << "FilePath" << YAML::Value << metadata.FilePath.generic_string();
				out << YAML::Key << "Type" << YAML::Value << AssetTypeToString(metadata.Type);
				out << YAML::Key << "Name" << YAML::Value << metadata.Name;
				out << YAML::EndMap;
			}
			out << YAML::EndSeq;
			out << YAML::EndMap;
		}

		std::ofstream fout(path);
		fout << out.c_str();
	}

	bool EditorAssetManager::DeserialzeAssetRegistry()
	{
		RY_PROFILE_FUNCTION();
		RY_CORE_INFO("SerialzeAsseRegistry Asset Regestriey");
		auto path = Project::GetActiveAssetRegistryPath();

		YAML::Node data;
		try
		{
			data = YAML::LoadFile(path.string());
			//data = YAML::LoadFile("AssetRegistry.ryr");
		}
		catch (YAML::ParserException e)
		{
			RY_CORE_ERROR("Failed to load project file '{0}'\n     {1}", path.string(), e.what());
			return false;
		}

		auto rootNode = data["AssetRegistry"];
		if (!rootNode)
			return false;
		for (const auto& node : rootNode)
		{
#if 0
			AssetHandle handle = node["Handle"].as<uint64_t>();
			auto& metadata = m_AssetRegistry.GetMetadata(handle);

			metadata.FilePath = node["FilePath"].as<std::string>();
			m_PathRegistry[metadata.FilePath] = handle;
			metadata.Type = AssetTypeFromString(node["Type"].as<std::string>());
			metadata.Name = node["Name"].as<std::string>();
#else
			DeserialzeAsset(node, m_AssetRegistry);
#endif
		}

		return true;
	}


	
	bool AssetInternalRegistry::IsAssetInteralInRegistry(AssetHandle handle) const
	{
		return m_HandleRegistry.find(handle)!=m_HandleRegistry.end();
	}

	void AssetInternalRegistry::CreateAssetInteral(Ref<Asset>& asset, AssetMetadata metadata, AssetHandle handle)
	{
		m_HandleRegistry[handle] = metadata;
		asset->Handle = handle;
		m_AssetMap[handle] = asset;
	}


	AssetMetadata& AssetInternalRegistry::GetInteralMetadata(AssetHandle handle)
	{
		if (IsAssetInteralInRegistry(handle))
		{
			return m_HandleRegistry[handle];
		}
		return AssetMetadata();
	}

	Ref<Asset> AssetInternalRegistry::GetAssetInteral(AssetHandle handle)
	{
		if (IsAssetInteralInRegistry(handle))
		{
			return m_AssetMap[handle];
		}
		
		return nullptr;
	}

}