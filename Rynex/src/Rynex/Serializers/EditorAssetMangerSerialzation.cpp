#include "rypch.h"
#include "EditorAssetMangerSerialzation.h"

#include "Rynex/Asset/EditorAssetManager.h"
#include "Rynex/Renderer/API/Buffer.h"
#include "Rynex/Project/Project.h"

#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>
#include <fstream>



namespace YAML {

	Emitter& operator<<(Emitter& out, const std::string_view& v);

}


namespace Rynex {



	bool EditorAssetMangerSerialzation::Serilze(const std::filesystem::path& filepath, AssetRegistry& assetRegistry)
	{
		YAML::Emitter out;
		{
			out << YAML::BeginMap;
			out << YAML::Key << "AssetRegistry" << YAML::Value;
			out << YAML::BeginSeq;
			for (const auto& [handle, metadata] : assetRegistry.GetHandleRegistry())
			{
				// if (metadata.Interale)
				if (metadata.GetIntern())
					continue;
				out << YAML::BeginMap;
				out << YAML::Key << "Handle" << YAML::Value << handle;
				out << YAML::Key << "FilePath" << YAML::Value << metadata.FilePath.generic_string();
				out << YAML::Key << "Type" << YAML::Value << Asset::AssetTypeToString(metadata.Type);
				out << YAML::Key << "Name" << YAML::Value << metadata.Name;
				out << YAML::EndMap;
			}
			out << YAML::EndSeq;
			out << YAML::EndMap;
		}

		std::ofstream fout(filepath);
		fout << out.c_str();
		fout.close();
		return true;
	}

	bool EditorAssetMangerSerialzation::Deserilze(const std::filesystem::path& filepath, AssetRegistry& assetRegistry)
	{

		RY_CORE_INFO("Deserialze Path: '{0}'", filepath.string().c_str());
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filepath.string());
			//data = YAML::LoadFile("AssetRegistry.ryr");
		}
		catch (YAML::ParserException e)
		{
			RY_CORE_ERROR("Failed to load project file '{0}'\n     {1}", filepath.string(), e.what());
			return false;
		}

		YAML::Node rootNode = data["AssetRegistry"];
		if (!rootNode)
			return false;
		for (const YAML::detail::iterator_value& node : rootNode)
		{
#if 0
			AssetHandle handle = node["Handle"].as<uint64_t>();
			auto& metadata = m_AssetRegistry.GetMetadata(handle);

			metadata.FilePath = node["FilePath"].as<std::string>();
			m_PathRegistry[metadata.FilePath] = handle;
			metadata.Type = AssetTypeFromString(node["Type"].as<std::string>());
			metadata.Name = node["Name"].as<std::string>();
#else
#endif
			AssetHandle handle = node["Handle"].as<uint64_t>();

			AssetMetadata metadata;
			metadata.FilePath = node["FilePath"].as<std::string>();
			metadata.Type = Asset::AssetTypeFromString(node["Type"].as<std::string>());
			metadata.Name = node["Name"].as<std::string>();
			// metadata.Aktive = true;
			metadata.SetActive(true);

			assetRegistry.CreateAsset(metadata.FilePath, handle, metadata, false);
		}
		return true;
	}
#if RY_EDITOR_ASSETMANGER_THREADE

#if 1
	bool EditorAssetMangerSerialzation::SerilzeThread(const std::filesystem::path& filepath, std::map<AssetHandle, AssetMetadata>* handleReg)
	{
		YAML::Emitter out;
		{
			out << YAML::BeginMap;
			out << YAML::Key << "AssetRegistry" << YAML::Value;
			out << YAML::BeginSeq;
			for (const auto& [handle, metadata] : *handleReg)
			{
				// if (metadata.Interale)
				if (metadata.GetIntern())
					continue;
				out << YAML::BeginMap;
				out << YAML::Key << "Handle" << YAML::Value << handle;
				out << YAML::Key << "FilePath" << YAML::Value << metadata.FilePath.generic_string();
				out << YAML::Key << "Type" << YAML::Value << Asset::AssetTypeToString(metadata.Type);
				out << YAML::Key << "Name" << YAML::Value << metadata.Name;
				out << YAML::EndMap;
			}
			out << YAML::EndSeq;
			out << YAML::EndMap;
		}

		std::ofstream fout(filepath);
		fout << out.c_str();
		fout.close();
		return true;
	}

	bool EditorAssetMangerSerialzation::DeserilzeThread(const std::filesystem::path& filepath, std::map<AssetHandle, AssetMetadata>* handleReg, std::map<std::filesystem::path, AssetHandle>* pathReg)
	{
		RY_CORE_INFO("Deserialze Path: '{0}'", filepath.string().c_str());
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filepath.string());
			//data = YAML::LoadFile("AssetRegistry.ryr");
		}
		catch (YAML::ParserException e)
		{
			RY_CORE_ERROR("Failed to load project file '{0}'\n     {1}", filepath.string(), e.what());
			return false;
		}

		YAML::Node rootNode = data["AssetRegistry"];
		if (!rootNode)
			return false;
		for (const YAML::detail::iterator_value& node : rootNode)
		{
#if 0
			AssetHandle handle = node["Handle"].as<uint64_t>();
			auto& metadata = m_AssetRegistry.GetMetadata(handle);

			metadata.FilePath = node["FilePath"].as<std::string>();
			m_PathRegistry[metadata.FilePath] = handle;
			metadata.Type = AssetTypeFromString(node["Type"].as<std::string>());
			metadata.Name = node["Name"].as<std::string>();
#else
#endif
			AssetHandle handle = node["Handle"].as<uint64_t>();

			AssetMetadata metadata;
			metadata.FilePath = node["FilePath"].as<std::string>();
			metadata.Type = Asset::AssetTypeFromString(node["Type"].as<std::string>());
			metadata.Name = node["Name"].as<std::string>();
			// metadata.Aktive = true;
			// metadata.Interale = false;
			metadata.SetActive(true);
			metadata.SetIntern(false);
			metadata.State = AssetState::LostConection;
			pathReg->insert_or_assign(metadata.FilePath, handle);
			handleReg->insert_or_assign(handle, metadata);
		}
		return true;
	}
#endif

#endif
}