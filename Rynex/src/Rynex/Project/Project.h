#pragma once

#include <string>

#include "Rynex/Core/Log.h"
#include "Rynex/Core/Base.h"

#include "Rynex/Asset/RuntimeAssetManager.h"
#include "Rynex/Asset/EditorAssetManager.h"



namespace Rynex {

	struct ProjectConfig
	{
		std::string Name = "Project";

		std::filesystem::path StartScene;

		std::filesystem::path AssetDirectory;
		std::filesystem::path AssetRegistryPath = "AssetRegistry.ryr"; // Relative to AssetDirectory
		std::filesystem::path ScriptModulePath;
	};

	class Project
	{
	public:
		const std::filesystem::path& GetProjectDirectory() { return m_ProjectDircetory; }
		std::filesystem::path GetAssetDirectory() { return GetProjectDirectory() / s_ActiveProject->m_Config.AssetDirectory; }
		std::filesystem::path GetAssetRegistryPath() { return s_ActiveProject->m_Config.AssetRegistryPath; }
		// TODO(Yan): move to asset manager when we have one
		std::filesystem::path GetAssetFileSystemPath(const std::filesystem::path& path) { return GetAssetDirectory() / path; }


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


		ProjectConfig& GetConfig() { return m_Config; }

		static Ref<Project> GetActive() { return s_ActiveProject; }

		std::shared_ptr<AssetManagerBase> GetAssetManger() { return m_AssetManger; }
		std::shared_ptr<RuntimeAssetManager> GetRuntimeAssetManger() const { return std::static_pointer_cast<RuntimeAssetManager>(m_AssetManger); }
		std::shared_ptr<EditorAssetManager> GetEditorAssetManger() const { return  std::static_pointer_cast<EditorAssetManager>(m_AssetManger); }

		static Ref<Project> New();
		static Ref<Project> Load(const std::filesystem::path& path);
		static bool SaveActive(const std::filesystem::path& path);
	private:
		ProjectConfig m_Config;
		std::filesystem::path m_ProjectDircetory;
		Ref<AssetManagerBase> m_AssetManger;
		
		inline static Ref<Project> s_ActiveProject;
	};


}