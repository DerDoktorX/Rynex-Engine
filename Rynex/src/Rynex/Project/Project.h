#pragma once
#include "Rynex/Core/Log.h"
#include "Rynex/Core/Base.h"

#include "Rynex/Asset/RuntimeAssetManager.h"

#if RY_EDITOR_ASSETMANGER_THREADE
	// #include "Rynex/Asset/EditorAssetManegerThreade.h"
#else
	#include "Rynex/Asset/EditorAssetManager.h"
#endif

#include "Rynex/Renderer/Rendering/Renderer.h"
#include "Rynex/Scripting/ScriptingEngine.h"

namespace Rynex {
	class EditorAssetManegerThreade;

	struct ProjectConfig
	{
		std::string Name = "Default-Project";
		std::string CreateDate = "00:00-00.00.0000";
		std::string LastOpenDate = "00:00-00.00.0000";
		std::filesystem::path StartScene = "";
		std::filesystem::path LastScene = "";

		std::filesystem::path AppDirektory = "";
		std::filesystem::path AssetDirectory = "Assets";
		std::filesystem::path AssetRegistryPath = "AssetRegistry.ryr";
		std::filesystem::path ProjectScriptPremake5 = "premake5.lua";
		std::filesystem::path ScriptAppPath = "SandboxProject/Assets/Scripts/Binaries/Sanbox.dll";
		std::filesystem::path ScriptCorePath = "Resources/Scripts/Rynex-ScriptingCore.dll";
		std::filesystem::path ProjectPath = "C:/dev/Rynex-Enine/Rynex-Editor/SandboxProject";
		std::string RynexVersion = "In Dev Version Cuurent Date Only";
		bool ProjectRady = false;

		ProjectConfig() = default;

		bool operator!=(const ProjectConfig& profectConfig) const
		{
			return ((profectConfig.Name != Name) &&
				(profectConfig.CreateDate != CreateDate) &&
				(profectConfig.LastOpenDate != StartScene) &&
				(profectConfig.LastScene != LastScene) &&
				(profectConfig.AppDirektory != AppDirektory) &&
				(profectConfig.AssetDirectory != AssetDirectory) &&
				(profectConfig.AssetRegistryPath != AssetRegistryPath) &&
				(profectConfig.ProjectScriptPremake5 != ProjectScriptPremake5) &&
				(profectConfig.ScriptAppPath != ScriptAppPath) &&
				(profectConfig.ScriptCorePath != ScriptCorePath) &&
				(profectConfig.ProjectPath != ProjectPath) &&
				(profectConfig.ProjectRady != ProjectRady));
		}
	};

	class RYNEX_API Project
	{
	public:
		const std::filesystem::path& GetProjectDirectory() { return s_ActiveProject->m_Config.ProjectPath; }
		const std::filesystem::path& GetAssetDirectory() { return s_ActiveProject->m_Config.AssetDirectory; }
		const std::filesystem::path& GetAssetRegistryPath() { return s_ActiveProject->m_Config.AssetRegistryPath; }
		void SetLastSceneAsset(const std::filesystem::path& sceneAsset) { s_ActiveProject->m_Config.LastScene = sceneAsset; }
		
		// TODO(Yan): move to asset manager when we have one
		const std::filesystem::path& GetAssetFileSystemPath(const std::filesystem::path& path) { return GetAssetDirectory() / path; }
		std::filesystem::path& GetAppDirektory() { return s_ActiveProject->m_Config.AppDirektory; }

		static const std::filesystem::path& GetActiveProjectDirectory()
		{
			RY_CORE_ASSERT(s_ActiveProject, "Erroe: Project::GetProjectDirectory()");
			return s_ActiveProject->GetProjectDirectory();
		}

		static const std::filesystem::path& GetActiveAssetDirectory()
		{
			RY_CORE_ASSERT(s_ActiveProject, "Erroe: Project::GetAssetDirectory()");
			return s_ActiveProject->GetAssetDirectory();
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

		static const std::filesystem::path& GetActiveScriptingAppDirektory()
		{
			RY_CORE_ASSERT(s_ActiveProject, "Erroe: Project::GetProjectDirectory()");
			return s_ActiveProject->m_Config.ScriptAppPath;
		}

		static const std::filesystem::path& GetActiveProjectScriptingCoreDirektory()
		{
			RY_CORE_ASSERT(s_ActiveProject, "Erroe: Project::GetActiveAssetProjectScriptingCoreDirektory()");
			return s_ActiveProject->m_Config.ScriptCorePath;
		}

		static void SetActiveLastSceneAsset(const std::filesystem::path& sceneAsset)
		{
			RY_CORE_ASSERT(s_ActiveProject, "Erroe: Project::GetActiveAssetProjectScriptingCoreDirektory()");
			if(s_ActiveProject->m_AssetManger->IsAssetHandleValid(sceneAsset))
				s_ActiveProject->SetActiveLastSceneAsset(sceneAsset);
		}

		static void Init()
		{
			RY_CORE_WARN("Projekt Init! Beginn");
			RY_CORE_ASSERT(!s_ActiveProject, "Project!");
			if(!s_ActiveProject)
			{
				s_ActiveProject = CreateRef<Project>();
				
				RY_CORE_INFO("Projekt Init! Sucess");
			}
			
			
		}

		static void ShutDown()
		{
			RY_CORE_WARN("Projekt ShutDown! Beginn");
			if (s_ActiveProject->m_Config.ProjectRady)
			{
				s_ActiveProject->m_AssetManger->OnDetach();
				RY_CORE_INFO("Projekt ShutDown! Sucess");
			}
		}	

		

		ProjectConfig& GetConfig() { return m_Config; }

		static Ref<Project> GetActive() { return s_ActiveProject; }

		Ref<AssetManagerBase> GetAssetManger() { return m_AssetManger; }
		Ref<RuntimeAssetManager> GetRuntimeAssetManger() const { return std::static_pointer_cast<RuntimeAssetManager>(m_AssetManger); }

#if RY_EDITOR_ASSETMANGER_THREADE
		Ref<EditorAssetManegerThreade> GetEditorAssetManger() const { return  std::static_pointer_cast<EditorAssetManegerThreade>(m_AssetManger); }
#else
		Ref<EditorAssetManager> GetEditorAssetManger() const { return  std::static_pointer_cast<EditorAssetManager>(m_AssetManger); }
#endif

		static Ref<Project> New();
		static Ref<Project> CreatNewPorject();
		static Ref<Project> CreatNewPorject(const std::filesystem::path& projectPath, const std::string& name);
		static Ref<Project> Load(const std::filesystem::path& path);
		static bool SaveActive(const std::filesystem::path& path);

		bool operator!=(const Ref<Project>& project)
		{
			return project->m_Config != m_Config;
		}
	private:
		ProjectConfig m_Config;

		Ref<AssetManagerBase> m_AssetManger;
		
		inline static Ref<Project> s_ActiveProject;
	};


}