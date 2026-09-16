#pragma once
#include <Rynex/Asset/RuntimeAssetManager.h>

namespace Rynex {

	class EditorAssetManagerThread;

	struct ProjectConfig
	{
		std::string m_Name = "Default-Project";
		std::string m_CreateDate = "00:00-00.00.0000";
		std::string m_LastOpenDate = "00:00-00.00.0000";
		std::filesystem::path m_StartScene = "";
		std::filesystem::path m_LastScene = "";

		std::filesystem::path m_AppDirectory = "";
		std::filesystem::path m_AssetDirectory = "Assets";
		std::filesystem::path m_AssetRegistryPath = "AssetRegistry.ryr";
		std::filesystem::path m_ProjectScriptPremake5 = "premake5.lua";
		std::filesystem::path m_ScriptAppPath = "SandboxProject/Assets/Scripts/Binaries/Sandbox.dll";
		std::filesystem::path m_ScriptCorePath = "Resources/Scripts/Rynex-ScriptingCore.dll";
		std::filesystem::path m_ProjectPath = "EMPTY";

		std::string m_RynexVersion = "In Dev Version Current Date Only";
		bool m_ProjectRady = false;

		ProjectConfig() = default;

		bool operator!=(const ProjectConfig& projectConfig) const
		{
		    bool resultName                   = (projectConfig.m_Name                     != m_Name);
		    bool resultCreateDate             = (projectConfig.m_CreateDate               != m_CreateDate);
		    bool resultLastOpenDate           = (projectConfig.m_LastOpenDate             != m_LastOpenDate);
		    bool resultLastScene              = (projectConfig.m_LastScene                != m_LastScene);
		    bool resultAppDirectory           = (projectConfig.m_AppDirectory             != m_AppDirectory);
		    bool resultAssetDirectory         = (projectConfig.m_AssetDirectory           != m_AssetDirectory);
		    bool resultAssetRegistryPath      = (projectConfig.m_AssetRegistryPath        != m_AssetRegistryPath);
		    bool resultProjectScriptPremake5  = (projectConfig.m_ProjectScriptPremake5    != m_ProjectScriptPremake5);
		    bool resultScriptAppPath          = (projectConfig.m_ScriptAppPath            != m_ScriptAppPath);
		    bool resultScriptCorePath         = (projectConfig.m_ScriptCorePath           != m_ScriptCorePath);
		    bool resultProjectPath            = (projectConfig.m_ProjectPath              != m_ProjectPath);
		    bool resultProjectRady            = (projectConfig.m_ProjectRady              != m_ProjectRady);


			return (resultName
			    && resultCreateDate
			    && resultCreateDate
			    && resultLastOpenDate
			    && resultLastScene
			    && resultAppDirectory
			    && resultAssetDirectory
			    && resultAssetRegistryPath
			    && resultProjectScriptPremake5
			    && resultScriptAppPath
			    && resultScriptCorePath
			    && resultProjectPath
			    && resultProjectRady);
		}
	};

	class RYNEX_API Project
	{
	public:
		Project();
		~Project();
#pragma region PathFunction
		[[nodiscard]] const std::filesystem::path& GetProjectDirectory() const { return m_Config.m_ProjectPath; }
		[[nodiscard]] const std::filesystem::path& GetAssetDirectory() const { return m_Config.m_AssetDirectory; }
		[[nodiscard]] const std::filesystem::path& GetAssetRegistryPath() const { return m_Config.m_AssetRegistryPath; }
		void SetLastSceneAsset(const std::filesystem::path& sceneAsset) { m_Config.m_LastScene = sceneAsset; }


		std::filesystem::path& GetAppDirectory() { return m_Config.m_AppDirectory; }

		static const std::filesystem::path& GetActiveProjectDirectory()
		{
			RY_CORE_ASSERT(s_ActiveInstanceProject, "Error: Project::GetProjectDirectory()");
			return s_ActiveInstanceProject->GetProjectDirectory();
		}


#pragma endregion


		static const std::filesystem::path& GetActiveAssetDirectory()
		{
			RY_CORE_ASSERT(s_ActiveInstanceProject, "Error: Project::GetActiveAssetDirectory()");
			return s_ActiveInstanceProject->GetAssetDirectory();
		}

		static std::filesystem::path GetActiveAssetRegistryPath()
		{
			RY_CORE_ASSERT(s_ActiveInstanceProject, "Error: Project::GetActiveAssetRegistryPath()");
			return s_ActiveInstanceProject->GetAssetRegistryPath();
		}

		static const std::filesystem::path& GetActiveScriptingAppDirectory()
		{
			RY_CORE_ASSERT(s_ActiveInstanceProject, "Error: Project::GetActiveScriptingAppDirectory()");
			return s_ActiveInstanceProject->m_Config.m_ScriptAppPath;
		}

		static const std::filesystem::path& GetActiveProjectScriptingCoreDirectory()
		{
			RY_CORE_ASSERT(s_ActiveInstanceProject, "Error: Project::GetActiveProjectScriptingCoreDirectory()");
			return s_ActiveInstanceProject->m_Config.m_ScriptCorePath;
		}

		static void SetActiveLastSceneAsset(const std::filesystem::path& sceneAsset)
		{
			RY_CORE_ASSERT(s_ActiveInstanceProject, "Error: Project::SetActiveLastSceneAsset()");
			if(s_ActiveInstanceProject->m_AssetManger->IsAssetHandleValid(sceneAsset))
				s_ActiveInstanceProject->SetActiveLastSceneAsset(sceneAsset);
		}

		static void Init()
		{
			RY_CORE_WARN("Project Init! Begin");
			RY_CORE_ASSERT(!s_ActiveInstanceProject, "Project!");
			if(!s_ActiveInstanceProject)
			{
				s_ActiveInstanceProject = CreateRef<Project>();
				
				RY_CORE_INFO("Project Init! Success");
			}
			
			
		}

		static void Shutdown()
		{
			RY_CORE_WARN("Project ShutDown! Begin");
			if (s_ActiveInstanceProject->m_Config.m_ProjectRady)
			{
				s_ActiveInstanceProject->m_AssetManger->OnDetach();
				s_ActiveInstanceProject->m_AssetManger.reset();
				RY_DESTROY_REF(s_ActiveInstanceProject);
				RY_CORE_INFO("Project Shutdown! Success");
			}
		}	

		

		ProjectConfig& GetConfig() { return m_Config; }

		static Ref<Project> GetActive() { return s_ActiveInstanceProject; }

		Ref<AssetManagerBase> GetAssetManger() { return m_AssetManger; }

		Ref<RuntimeAssetManager> GetRuntimeAssetManger() const { return std::static_pointer_cast<RuntimeAssetManager>(m_AssetManger); }
		Ref<EditorAssetManagerThread> GetEditorAssetManger() const { return  std::static_pointer_cast<EditorAssetManagerThread>(m_AssetManger); }

		static Ref<Project> New();
		static Ref<Project> CreatNewProject();
		static Ref<Project> CreatNewProject(const std::filesystem::path& projectPath, const std::string& name);
		static Ref<Project> Load(const std::filesystem::path& path);
		static bool SaveActive(const std::filesystem::path& path);

		bool operator!=(const Ref<Project>& project) const
        {
			return project->m_Config != m_Config;
		}
	private:
		ProjectConfig m_Config;

		Ref<AssetManagerBase> m_AssetManger;
		
		inline static Ref<Project> s_ActiveInstanceProject;
	};


}