#include "rypch.h"
#include "ProjectSerializer.h"
#include <Rynex/Project/Project.h>


#include <yaml-cpp/yaml.h>
#include <fstream>

#define YAML_AKTIV 0
#define	DESERLIZE_PATH(name)	config.name = \
		Utils::DeserlizeFilePathNode(projectNode[RY_STRINGIFY_MOAKRO(name)], parentProjectFile)

#define	SERLIZE_PATH(name)	Utils::SerlizeFilePathNode(\
	out, RY_STRINGIFY_MOAKRO(name), \
	config.name, \
	projectFolder)

namespace Rynex {

	namespace Utils {

		static std::filesystem::path DeserlizeFilePathNode(YAML::Node data, const std::filesystem::path& base)
		{
			if(!data)
				return std::filesystem::path("");


			std::string markedPathStr = data.as<std::string>();
		    FileSystem::Path path(markedPathStr);
			std::filesystem::path absolutPath = path.GetPath();
			return absolutPath;
		}

		static void SerlizeFilePathNode(YAML::Emitter& out, const std::string& name, const std::filesystem::path& path, const std::filesystem::path& base)
		{
			constexpr const char* markerProject = RY_PATH_PROJECT_MARKER_STR;
			std::string realtiveFromBaseStr;
			if (!path.empty())
			{
				std::filesystem::path realtiveFromBase;
				if (path.is_absolute())
					realtiveFromBase = std::filesystem::relative(path, base);
				else
					realtiveFromBase = path;

			    FileSystem::Path pathSystem(realtiveFromBase);
				realtiveFromBaseStr = pathSystem.GetMarkedPathString();
			}

			out << YAML::Key << name << YAML::Value << realtiveFromBaseStr;
		}
	}


	ProjectSerializer::ProjectSerializer(Ref<Project> project)
		: m_Project(project)
	{
	}

	bool ProjectSerializer::Serlize(const std::filesystem::path& filepath)
	{
		ProjectConfig& config = m_Project->GetConfig();
		RY_LOG_DISABLE_NUMBER;

		YAML::Emitter out;
		{
			out << YAML::BeginMap; // Root
			out << YAML::Key << "Project" << YAML::Value;
			{
				out << YAML::BeginMap;// Project
				out << YAML::Key << "Name" << YAML::Value << config.m_Name;
				
				std::filesystem::path projectFolder = config.m_ProjectPath;
				SERLIZE_PATH(m_ProjectPath);
				SERLIZE_PATH(m_AssetDirectory);
				SERLIZE_PATH(m_AssetRegistryPath);
				
				SERLIZE_PATH(m_LastScene);
				SERLIZE_PATH(m_StartScene);
				
				SERLIZE_PATH(m_ScriptCorePath);
				SERLIZE_PATH(m_ScriptAppPath);

				
				if ("00:00-00.00.0000" == config.m_CreateDate)
					config.m_CreateDate = AssetRegistry::GetCurrentTimeStr();
				
				out << YAML::Key << "CreateDate" << YAML::Value << config.m_CreateDate;
				out << YAML::Key << "LastOpenDate" << YAML::Value << AssetRegistry::GetCurrentTimeStr();
				
				out << YAML::EndMap; // Project
			}
			out << YAML::EndMap; // Root
		}

		std::ofstream fout(filepath);
		RY_CORE_ASSERT(fout);
		fout << out.c_str();

		RY_LOG_ENABLE_NUMBER;

		return true;
	}

	bool ProjectSerializer::Deserlize(const std::filesystem::path& filepath)
	{
		ProjectConfig& config = m_Project->GetConfig();
		RY_LOG_DISABLE_NUMBER;

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

		YAML::Node projectNode = data["Project"];
		if (!projectNode)
			return false;

		std::filesystem::path parentProjectFile = filepath.parent_path();
		std::string parentProjectFileStr = parentProjectFile.generic_string();
		parentProjectFile = parentProjectFileStr;
		config.m_Name = projectNode["Name"].as<std::string>();
		config.m_ProjectPath = parentProjectFileStr;

		config.m_ProjectPath = Utils::DeserlizeFilePathNode(projectNode["ProjectPath"], parentProjectFile);
		config.m_AssetDirectory = Utils::DeserlizeFilePathNode(projectNode["AssetDirectory"], parentProjectFile);
		config.m_AssetRegistryPath = Utils::DeserlizeFilePathNode(projectNode["AssetRegistryPath"], parentProjectFile);

		config.m_StartScene = Utils::DeserlizeFilePathNode(projectNode["StartScene"], parentProjectFile);
		config.m_LastScene = Utils::DeserlizeFilePathNode(projectNode["LastScene"], parentProjectFile);
		config.m_ScriptAppPath = Utils::DeserlizeFilePathNode(projectNode["ScriptAppPath"], parentProjectFile);
		config.m_ScriptCorePath = Utils::DeserlizeFilePathNode(projectNode["ScriptCorePath"], parentProjectFile);
		
		
		config.m_LastOpenDate = projectNode["LastOpenDate"].as<std::string>();
		config.m_ProjectRady = true;
		RY_LOG_ENABLE_NUMBER;

		return true;
	}

}
