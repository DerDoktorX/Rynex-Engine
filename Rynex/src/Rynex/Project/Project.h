#pragma once

#include <string>

#include "Rynex/Core/Log.h"
#include "Rynex/Core/Base.h"

#include "Rynex/Asset/RuntimeAssetManager.h"
#include "Rynex/Asset/EditorAssetManager.h"



namespace Rynex {

	struct ProjectConfig
	{
		std::string Name;

		std::filesystem::path StartScene;

		std::filesystem::path AppDirektory = "";

		std::filesystem::path AssetDirectory;
		std::filesystem::path AssetRegistryPath = "AssetRegistry.ryr"; // Relative to AssetDirectory
		std::filesystem::path ScriptModulePath;
		std::filesystem::path ProjectPath = "C:/dev/Rynex-Enine/Rynex-Editor/SandboxProject";
	};

	class Project
	{
	public:
		const std::filesystem::path& GetProjectDirectory() { return s_ActiveProject->m_Config.ProjectPath; }
		std::filesystem::path GetAssetDirectory() { return GetProjectDirectory() / s_ActiveProject->m_Config.AssetDirectory; }
		std::filesystem::path& GetAssetRegistryPath() { return s_ActiveProject->m_Config.AssetRegistryPath; }
		
		// TODO(Yan): move to asset manager when we have one
		std::filesystem::path GetAssetFileSystemPath(const std::filesystem::path& path) { return GetAssetDirectory() / path; }
		std::filesystem::path& GetAppDirektory() { return s_ActiveProject->m_Config.AppDirektory; }

		static const std::filesystem::path& GetActiveProjectDirectory()
		{
			RY_CORE_ASSERT(s_ActiveProject, "Erroe: Project::GetProjectDirectory()");
			return s_ActiveProject->GetProjectDirectory();
		}

		static const std::filesystem::path& GetActiveAssetDirectory()
		{
			RY_CORE_ASSERT(s_ActiveProject, "Erroe: Project::GetAssetDirectory()");
			return s_ActiveProject->GetAssetRegistryPath();
		}

		static const std::filesystem::path& GetActiveAssetFileSystemPath(const std::filesystem::path& path)
		{
			RY_CORE_ASSERT(s_ActiveProject, "Erroe: Project::GetAssetDirectory()");
			return s_ActiveProject->GetAssetFileSystemPath(path);
		}

		static std::filesystem::path GetActiveAssetRegistryPath()
		{
			RY_CORE_ASSERT(s_ActiveProject, "Erroe: Project::GetActiveAssetRegistryPath()");
			return s_ActiveProject->GetAssetRegistryPath();
		}

		static const std::filesystem::path GetActiveAssetScriptingDirektory()
		{
			RY_CORE_ASSERT(s_ActiveProject, "Erroe: Project::GetProjectDirectory()");
			return (s_ActiveProject->GetProjectDirectory() / s_ActiveProject->m_Config.ScriptModulePath).generic_string();
		}

		static const std::filesystem::path GetActiveAssetProjectScriptingCoreDirektory()
		{
			RY_CORE_ASSERT(s_ActiveProject, "Erroe: Project::GetActiveAssetProjectScriptingCoreDirektory()");
			return "Resources/Scripts/Rynex-ScriptingCore.dll";
		}

		static void ShutDown()
		{
			RY_CORE_WARN("Projekt ShutDown! Beginn");
			s_ActiveProject->m_AssetManger->OnDetach();
			RY_CORE_INFO("Projekt ShutDown! Sucess");
		}

		ProjectConfig& GetConfig() { return m_Config; }

		static Ref<Project> GetActive() { return s_ActiveProject; }

		Ref<AssetManagerBase> GetAssetManger() { return m_AssetManger; }
		Ref<RuntimeAssetManager> GetRuntimeAssetManger() const { return std::static_pointer_cast<RuntimeAssetManager>(m_AssetManger); }
		Ref<EditorAssetManager> GetEditorAssetManger() const { return  std::static_pointer_cast<EditorAssetManager>(m_AssetManger); }

		static Ref<Project> New();
		static Ref<Project> Load(const std::filesystem::path& path);
		static bool SaveActive(const std::filesystem::path& path);
	private:
		ProjectConfig m_Config;
		Ref<AssetManagerBase> m_AssetManger;
		
		inline static Ref<Project> s_ActiveProject;
	};


}