#include "rypch.h"
#include "ProjectSerialiazer.h"
#include <Rynex/Project/Project.h>


#include "YAML.h"

#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#define YAML_CPP_STATIC_DEFINE
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

			constexpr const char* markerProject = RY_PATH_PROJECT_MARKER_STR;
			std::string markedPathStr = data.as<std::string>();
			if (markedPathStr.empty())
				return std::filesystem::path("");

			std::filesystem::path absolutPath = Project::ReplaceMarkerWithePath(markedPathStr, markerProject, base);
			return absolutPath;
		}

		static void SerlizeFilePathNode(YAML::Emitter& out, const std::string& name, const std::filesystem::path& path, const std::filesystem::path& base)
		{
			constexpr const char* markerProject = RY_PATH_PROJECT_MARKER_STR;
			std::string realtiveFromBaseStr = "";
			if (path != "")
			{
				std::filesystem::path realtiveFromBase = "";
				if (path.is_absolute())
					realtiveFromBase = std::filesystem::relative(path, base);
				else
					realtiveFromBase = path;

				realtiveFromBase = realtiveFromBase.lexically_normal();
				realtiveFromBaseStr = realtiveFromBase.generic_string();
				realtiveFromBase = realtiveFromBaseStr;
				realtiveFromBase = Project::SetMarker(realtiveFromBase, markerProject);
				realtiveFromBaseStr = realtiveFromBase.string();
			}

			out << YAML::Key << name << YAML::Value << realtiveFromBaseStr;
		}
	}


	ProjectSerialiazer::ProjectSerialiazer(Ref<Project> project)
		: m_Project(project)
	{
	}

	bool ProjectSerialiazer::Serlize(const std::filesystem::path& filepath)
	{
		ProjectConfig& config = m_Project->GetConfig();
		RY_LOG_DISABLE_NUMBER;

		YAML::Emitter out;
		{
			out << YAML::BeginMap; // Root
			out << YAML::Key << "Project" << YAML::Value;
			{
				out << YAML::BeginMap;// Project
				out << YAML::Key << "Name" << YAML::Value << config.Name;
				
				std::filesystem::path projectFolder = config.ProjectPath;
				SERLIZE_PATH(ProjectPath);
				SERLIZE_PATH(AssetDirectory);
				SERLIZE_PATH(AssetRegistryPath);
				
				SERLIZE_PATH(LastScene);
				SERLIZE_PATH(StartScene);
				
				SERLIZE_PATH(ScriptCorePath);
				SERLIZE_PATH(ScriptAppPath);

				
				if (config.CreateDate == "00:00-00.00.0000")
					config.CreateDate = AssetRegistry::GetCurrentTimeStr();
				
				out << YAML::Key << "CreateDate" << YAML::Value << config.CreateDate;
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

	bool ProjectSerialiazer::Deserlize(const std::filesystem::path& filepath)
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

		auto projectNode = data["Project"];
		if (!projectNode)
			return false;

		std::filesystem::path parentProjectFile = filepath.parent_path();
		std::string parentProjectFileStr = parentProjectFile.generic_string();
		parentProjectFile = parentProjectFileStr;
		config.Name = projectNode["Name"].as<std::string>();

		DESERLIZE_PATH(ProjectPath);
		DESERLIZE_PATH(AssetDirectory);
		DESERLIZE_PATH(AssetRegistryPath);

		DESERLIZE_PATH(StartScene);
		DESERLIZE_PATH(LastScene);
		DESERLIZE_PATH(ScriptAppPath);
		DESERLIZE_PATH(ScriptCorePath);
		
		
		config.LastOpenDate = projectNode["LastOpenDate"].as<std::string>();
		config.ProjectRady = true;
		RY_LOG_ENABLE_NUMBER;

		return true;
	}

}
