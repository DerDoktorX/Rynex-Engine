#include "rypch.h"
#include "EditorAssetMangerSerializer.h"
#include "YAML.h"

#include <Rynex/Asset/EditorAssetManager.h>
#include <Rynex/Renderer/API/Buffer.h>
#include <Rynex/Project/Project.h>


#include <yaml-cpp/yaml.h>
#include <fstream>




namespace Rynex {



	bool EditorAssetMangerSerializer::SerilzeThread(const std::filesystem::path& filepath, std::map<AssetHandle, AssetMetadata>* handleReg)
	{
		
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

				std::string pathStr = metadata.m_PathMarker;


				// out << YAML::Key << "FilePath" << YAML::Value << filePathStr;
				// out << YAML::Key << "FilePath-Absolute" << YAML::Value << pathAbosulte.string();
				// out << YAML::Key << "FilePath-Realtiv" << YAML::Value << realtivePath.string();
				out << YAML::Key << "FilePath-ProjectMarker" << YAML::Value << metadata.m_Path.GetMarkedPathString();

				out << YAML::Key << "Type" << YAML::Value << Asset::AssetTypeToString(metadata.m_Type);
				out << YAML::Key << "Name" << YAML::Value << metadata.m_Name;
				out << YAML::Key << "ChangeTime" << YAML::Value << metadata.m_ChangeTime;

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

	bool EditorAssetMangerSerializer::DeserilzeThread(const std::filesystem::path& filepath, std::map<AssetHandle, AssetMetadata>* handleReg, std::map<FileSystem::Path, AssetHandle>* pathReg)
	{
		RY_CORE_INFO("Deserialize Path: '{0}'", filepath.string().c_str());


		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filepath.string());
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

#if 1
			FileSystem::Path pathSystemMarked = FileSystem::Path(filePathMarkerStr);
			// FileSystem::Path pathSystemRealtiv = FileSystem::Path(filePathAbsoluteStr);
			// FileSystem::Path pathSystemAbsoulte = FileSystem::Path(filePathRealtivStr);
#else
			RY_REMBER_FUNC_CHANGE("Test #if FileSystem::Path with out put!");
#endif
			
			
			std::replace(filePathMarkerStr.begin(), filePathMarkerStr.end(), '\\', '/');
		
			if (YAML::Node nodePath = node["FilePath"])
			{
				filePathStr = nodePath.as<std::string>();

				std::filesystem::path origFilePath = filePathStr;
				filePathStr = origFilePath.generic_string();
				origFilePath = filePathStr;

				metadata.SetMarkedFilePath(filePathMarkerStr, origFilePath);

			}
			else 
			{
				metadata.SetMarkedFilePath(filePathMarkerStr);
			}
			

			metadata.m_Type = Asset::AssetTypeFromString(node["Type"].as<std::string>());
			AssetType assetType = Asset::GetAssetTypeFromFilePath(metadata.m_FilePath);
			if (metadata.m_Type != assetType)
				metadata.m_Type = assetType;


			metadata.m_Name = node["Name"].as<std::string>();
			if (node["ChangeTime"])
				metadata.m_ChangeTime = node["ChangeTime"].as<std::string>();
			else
				metadata.m_ChangeTime = AssetRegistry::GetCurrentTimeStr();
			metadata.SetActive(true);
			metadata.SetIntern(false);
			metadata.SetState(AssetState::LostConnection);
            const FileSystem::Path& pathKey = metadata.m_Path;
			pathReg->insert_or_assign(pathKey, handle);
			handleReg->insert_or_assign(handle, metadata);
		}

		return true;
	}

}