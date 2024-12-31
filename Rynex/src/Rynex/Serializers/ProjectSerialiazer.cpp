#include "rypch.h"
#include "ProjectSerialiazer.h"
#include "Rynex/Project/Project.h"


#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>
#include <fstream>

#define YAML_AKTIV 0


namespace YAML {

}


namespace Rynex {


	ProjectSerialiazer::ProjectSerialiazer(Ref<Project> project)
		: m_Project(project)
	{
	}

	bool ProjectSerialiazer::Serlize(const std::filesystem::path& filepath)
	{
		const auto& config = m_Project->GetConfig();

		YAML::Emitter out;
		{
			out << YAML::BeginMap; // Root
			out << YAML::Key << "Project" << YAML::Value;
			{
				out << YAML::BeginMap;// Project
				out << YAML::Key << "Name" << YAML::Value << config.Name;
				out << YAML::Key << "StartScene" << YAML::Value << config.StartScene.lexically_normal().string();
				if(config.AssetRegistryPath != "")
					out << YAML::Key << "AssetRegistryPath" << YAML::Value << config.AssetRegistryPath.string();
				out << YAML::Key << "AssetDirectory" << YAML::Value << config.AssetDirectory.string();
				if (config.AssetRegistryPath != "")
					out << YAML::Key << "ScriptCorePath" << YAML::Value << config.ScriptCorePath.string();
				out << YAML::Key << "ScriptAppPath" << YAML::Value << config.ScriptAppPath.string();
				if(config.LastScene!="")
					out << YAML::Key << "LastScene" << YAML::Value << config.LastScene.string();
				out << YAML::Key << "CreateDate" << YAML::Value << config.CreateDate.c_str();
				out << YAML::Key << "LastOpenDate" << YAML::Value << config.LastOpenDate.c_str();
				out << YAML::Key << "ProjectPath" << YAML::Value << config.ProjectPath.string();
				out << YAML::EndMap; // Project
			}
			out << YAML::EndMap; // Root
		}

		std::ofstream fout(filepath);
		fout << out.c_str();

		return true;
	}

	bool ProjectSerialiazer::Deserlize(const std::filesystem::path& filepath)
	{
		auto& config = m_Project->GetConfig();

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

		config.Name = projectNode["Name"].as<std::string>();
		config.StartScene = projectNode["StartScene"].as<std::string>();
		config.AssetDirectory = projectNode["AssetDirectory"].as<std::string>();
		if (projectNode["AssetRegistryPath"])
			config.AssetRegistryPath = projectNode["AssetRegistryPath"].as<std::string>();
		config.ScriptAppPath = projectNode["ScriptAppPath"].as<std::string>();
		if (projectNode["ScriptCorePath"])
			config.ScriptCorePath = projectNode["ScriptCorePath"].as<std::string>();
		config.ProjectPath = projectNode["ProjectPath"].as<std::string>();
		config.LastOpenDate = projectNode["LastOpenDate"].as<std::string>();
		if (projectNode["LastScene"])
			config.LastScene = projectNode["LastScene"].as<std::string>();
		config.ProjectRady = true;
		return true;
	}

}
