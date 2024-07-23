#include "rypch.h"
#include "EditorAssetManager.h"
#include "Base/AssetImporter.h"
#include "Rynex/Serializers/YAML.h"
#include "Rynex/Project/Project.h"

#include <yaml-cpp/emitfromevents.h>
#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>



namespace Rynex {

	EditorAssetManager::EditorAssetManager()
	{
		RY_PROFILE_FUNCTION();
		RY_CORE_ERROR("EditorAssetManager Constructor");
	}

	bool EditorAssetManager::IsAssetHandleValid(AssetHandle handle) const
	{
		RY_PROFILE_FUNCTION();
		// RY_CORE_ASSERT(false, "EditorAssetManager::IsAssetHandleValid");
		return handle != 0 && m_AssetRegistry.find(handle) != m_AssetRegistry.end();
	}

	Ref<Asset> EditorAssetManager::GetAsset(AssetHandle handle)
	{
		RY_PROFILE_FUNCTION();
		// RY_CORE_ASSERT(false, "EditorAssetManager::GetAsset");
		if (!IsAssetHandleValid(handle))
			return nullptr;
		Ref<Asset> asset;
		if (IsAssetLoaded(handle))
		{
			asset = m_LoadedAssets.at(handle);
		}
		else
		{
			const AssetMetadata& metadata = GetMetadata(handle);
			m_AssetRegistry[handle].State = AssetState::Loading;
			asset = AssetImporter::ImportAsset(handle, metadata);
			asset->Handle = handle;
			m_AssetRegistry[handle].State = AssetState::Ready;
			if (!asset) {}
			m_LoadedAssets[handle] = asset;
			
			
		}
		return asset;
		
	}

	bool EditorAssetManager::IsAssetLoaded(AssetHandle handle) const
	{
		RY_PROFILE_FUNCTION();
		// RY_CORE_ASSERT(false, "EditorAssetManager::IsAssetLoaded");
		return m_LoadedAssets.find(handle) != m_LoadedAssets.end();
		// return false;
	}

	void EditorAssetManager::ImportAsset(const std::filesystem::path& filepath)
	{
		RY_PROFILE_FUNCTION();
		AssetHandle handle;
		AssetMetadata metadata;

		metadata.FilePath = filepath;
		metadata.Type = GetAssetTypeFromFilePath(filepath);
		metadata.State = AssetState::Loading;
		Ref<Asset> asset = AssetImporter::ImportAsset(handle, metadata);
		metadata.State = AssetState::Ready;
		
		if(asset)
		{
			asset->Handle = handle;
			m_LoadedAssets[handle] = asset;
			m_AssetRegistry[handle] = metadata;
		}

	}

	const AssetMetadata& EditorAssetManager::GetMetadata(AssetHandle handle) const
	{
		RY_PROFILE_FUNCTION();
		static AssetMetadata s_NullMetadata;
		auto it = m_AssetRegistry.find(handle);
		if (it == m_AssetRegistry.end())
			return s_NullMetadata;

		return it->second;
	}

	void EditorAssetManager::SetMetadataState(AssetHandle handle, AssetState state)
	{
		RY_PROFILE_FUNCTION();
		if (m_AssetRegistry.find(handle) != m_AssetRegistry.end())
			m_AssetRegistry[handle].State = state;
			
	}

	const std::filesystem::path& EditorAssetManager::GetFilePath(AssetHandle handle) const
	{
		RY_PROFILE_FUNCTION();
		return GetMetadata(handle).FilePath;
	}
	

	const AssetHandle& EditorAssetManager::GetAssetHandle(const std::filesystem::path& path)
	{
		RY_PROFILE_FUNCTION();
		return m_PathRegistry[path] ? m_PathRegistry[path] :  AddFileToRegistry(path);
	}

	const AssetHandle& EditorAssetManager::AddFileToRegistry(const std::filesystem::path& filepath)
	{
		RY_PROFILE_FUNCTION();
		if(m_PathRegistry.find(filepath) != m_PathRegistry.end())
		{
			return m_PathRegistry[filepath];
		}

		AssetHandle handle;
		AssetMetadata& metadata = m_AssetRegistry[handle];
		m_PathRegistry[filepath] = handle;
		metadata.FilePath = filepath;
		metadata.LoadingTime = std::filesystem::last_write_time(filepath);
		metadata.Type = GetAssetTypeFromFilePath(filepath);
		metadata.State = AssetState::NotLoaded;
		return handle;
	}
	
	const void EditorAssetManager::ReLoadeAsset(AssetHandle handle) const
	{
		RY_PROFILE_FUNCTION();
		AssetMetadata metadata = GetMetadata(handle);
		metadata.State = AssetState::Loading;
		Ref<Asset> asset = AssetImporter::ImportAsset(handle, metadata);
		metadata.State = AssetState::Ready;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const std::string_view& v)
	{
		RY_PROFILE_FUNCTION();
		out << std::string(v.data(), v.size());
		return out;
	}

	void EditorAssetManager::CreateDirektoryRegestriy(const std::filesystem::path& curentPath)
	{
		RY_PROFILE_FUNCTION();
		AssetFileDirectory& assetDirectory = m_AssetDirectorys[curentPath];
		assetDirectory.FolderName = curentPath.filename().string();
		assetDirectory.FolderPath = curentPath;
	}

	void EditorAssetManager::AddAssetToDirektory(const std::filesystem::path& path, const std::filesystem::path& curentPath)
	{
		RY_PROFILE_FUNCTION();
		m_AssetDirectorys[curentPath].Files.push_back(AddFileToRegistry(path));
	}

	void EditorAssetManager::AddDirektoryToDirektory(const std::filesystem::path& path, const std::filesystem::path& curentPath)
	{
		RY_PROFILE_FUNCTION();
		m_AssetDirectorys[curentPath].Folders.push_back(path);
	}


	void EditorAssetManager::SerialzeAssetRegestriy()
	{
		RY_PROFILE_FUNCTION();
		auto path = Project::GetActiveAssetRegistryPath();

		YAML::Emitter out;
		{
			out << YAML::BeginMap;
			out << YAML::Key << "AssetRegistry" << YAML::Value;
			out << YAML::BeginSeq;
			for (const auto& [handle, metadata] : m_AssetRegistry)
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

	bool EditorAssetManager::DeserialzeAssetRegestriy()
	{
		RY_PROFILE_FUNCTION();
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
			AssetHandle handle = node["Handle"].as<uint64_t>();
			auto& metadata = m_AssetRegistry[handle];
			
			metadata.FilePath = node["FilePath"].as<std::string>();
			m_PathRegistry[metadata.FilePath] = handle;
			metadata.Type = AssetTypeFromString(node["Type"].as<std::string>());
			metadata.Name = node["Name"].as<std::string>();
		}

		return true;
	}

}