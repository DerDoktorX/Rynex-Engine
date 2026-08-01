#include "rypch.h"
#include "EditorAssetMangerSerialzation.h"
#include "YAML.h"

#include "Rynex/Asset/EditorAssetManager.h"
#include "Rynex/Renderer/API/Buffer.h"
#include "Rynex/Project/Project.h"

#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>
#include <fstream>




namespace Rynex {

#if RY_EDITOR_ASSETMANGER_THREADE

#if 1
	bool EditorAssetMangerSerialzation::SerilzeThread(const std::filesystem::path& filepath, std::map<AssetHandle, AssetMetadata>* handleReg)
	{
		RY_LOG_DISABLE_NUMBER;

		YAML::Emitter out;
		{
			out << YAML::BeginMap;
			out << YAML::Key << "AssetRegistry" << YAML::Value;
			out << YAML::BeginSeq;
			for (const auto& [handle, metadata] : *handleReg)
			{
				
				if (metadata.GetIntern())
					continue;
				out << YAML::BeginMap;
				out << YAML::Key << "Handle" << YAML::Value << handle;
				std::filesystem::path path = metadata.FilePath.lexically_normal();
				
				std::string filePathStr = path.generic_string();
				
				std::pair <std::string, std::filesystem::path> pair  = Project::GeanrateRealtivePathAndMarker(filePathStr);
				const std::string& marker = pair.first;
				const std::filesystem::path& realtivePath = pair.second;
				std::filesystem::path basePath = Project::GetAbsulteFilePathFormMarker(marker);
				std::filesystem::path pathAbosulte = basePath / realtivePath;
				std::string pathAbosulteStr = pathAbosulte.generic_string();
				pathAbosulte = pathAbosulteStr;

				std::filesystem::path filePathMarker = Project::SetMarker(realtivePath, marker);

				// out << YAML::Key << "FilePath" << YAML::Value << filePathStr;
				// out << YAML::Key << "FilePath-Absolute" << YAML::Value << pathAbosulte.string();
				// out << YAML::Key << "FilePath-Realtiv" << YAML::Value << realtivePath.string();
				out << YAML::Key << "FilePath-ProjectMarker" << YAML::Value << filePathMarker.string();

				out << YAML::Key << "Type" << YAML::Value << Asset::AssetTypeToString(metadata.Type);
				out << YAML::Key << "Name" << YAML::Value << metadata.Name;
				out << YAML::Key << "ChangeTime" << YAML::Value << metadata.ChangeTime;

				out << YAML::EndMap;
			}
			out << YAML::EndSeq;
			out << YAML::EndMap;
		}

		std::ofstream fout(filepath);
		fout << out.c_str();
		fout.close();

		RY_LOG_ENABLE_NUMBER;

		return true;
	}

	bool EditorAssetMangerSerialzation::DeserilzeThread(const std::filesystem::path& filepath, std::map<AssetHandle, AssetMetadata>* handleReg, std::map<std::filesystem::path, AssetHandle>* pathReg)
	{
		RY_CORE_INFO("Deserialze Path: '{0}'", filepath.string().c_str());
		RY_LOG_DISABLE_NUMBER;

		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filepath.string());
			// data = YAML::LoadFile("AssetRegistry.ryr");
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
			std::string filePathStr = "";
			std::string filePathAbsoluteStr = "";
			std::string filePathRealtivStr = "";
			if(YAML::Node nodePath = node["FilePath-Absolute"])
				filePathAbsoluteStr = nodePath.as<std::string>();
			if (YAML::Node nodePath = node["FilePath-Realtiv"])
				filePathRealtivStr = nodePath.as<std::string>();
			std::string filePathMarkerStr = node["FilePath-ProjectMarker"].as<std::string>();


			std::filesystem::path filePathMarker = filePathMarkerStr;
			filePathMarkerStr = filePathMarker.generic_string();
			filePathMarker = filePathMarkerStr;
			
		
#if 0
			

			std::string marker = Project::ExtraxtMarker(filePathMarker);
			std::filesystem::path realtivePath = Project::RemoveMarker(filePathMarker, marker);
			std::filesystem::path absultePath = Project::ReplaceMarkerWitheAbsolutePath(filePathMarker);
			if (realtivePath == origFilePath)
			{
				metadata.SetMarkedFilePath(filePathMarker, origFilePath);
			}
			else if(absultePath == origFilePath)
			{
				metadata.SetMarkedFilePath(filePathMarker, origFilePath);
			}
			else
			{
				RY_CORE_WARN("No Realtiv and no Absolute Pathe Mathed The Origenel Path {}", origFilePath);
				metadata.SetFilePath(origFilePath);
			}
#elif 1
			if (YAML::Node nodePath = node["FilePath"])
			{
				filePathStr = nodePath.as<std::string>();

				std::filesystem::path origFilePath = filePathStr;
				filePathStr = origFilePath.generic_string();
				origFilePath = filePathStr;

				metadata.SetMarkedFilePath(filePathMarker, origFilePath);

			}
			else 
			{
				metadata.SetMarkedFilePath(filePathMarker);
			}
#else	
			std::filesystem::path origFilePath = filePathStr;
			filePathStr = origFilePath.generic_string();
			origFilePath = filePathStr;

			metadata.SetMarkedFilePath(filePathMarker, origFilePath);
#endif
			

			metadata.Type = Asset::AssetTypeFromString(node["Type"].as<std::string>());
			AssetType assetType = Asset::GetAssetTypeFromFilePath(metadata.FilePath);
			if (metadata.Type != assetType)
				metadata.Type = assetType;


			metadata.Name = node["Name"].as<std::string>();
			if (node["ChangeTime"])
				metadata.ChangeTime = node["ChangeTime"].as<std::string>();
			else
				metadata.ChangeTime = AssetRegistry::GetCurrentTimeStr();
			// metadata.Aktive = true;
			// metadata.Interale = false;
			metadata.SetActive(true);
			metadata.SetIntern(false);
			metadata.State = AssetState::LostConection;

			pathReg->insert_or_assign(metadata.RealtivePath, handle);
			pathReg->insert_or_assign(metadata.AbsolutePath, handle);
			pathReg->insert_or_assign(metadata.FilePath, handle);
			handleReg->insert_or_assign(handle, metadata);
		}
		RY_LOG_ENABLE_NUMBER;

		return true;
	}
#endif

#endif
}