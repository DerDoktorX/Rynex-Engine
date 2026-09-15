#pragma once
#include <Rynex/Asset/RuntimeAssetManager.h>
#include <Rynex/Scripting/Mono/ScriptingEngine.h>

namespace Rynex {

	class EditorAssetManagerThread;

	struct ProjectConfig
	{
		std::string name = "Default-Project";
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
		// std::filesystem::path ProjectPath = "C:/dev/Rynex-Enine/Rynex-Editor/SandboxProject";
		std::filesystem::path ProjectPath = "EMPTY";

		std::string RynexVersion = "In Dev Version Cuurent Date Only";
		bool ProjectRady = false;

		ProjectConfig() = default;

		bool operator!=(const ProjectConfig& profectConfig) const
		{
			return ((profectConfig.name != name) &&
				(profectConfig.CreateDate != CreateDate) &&
				(profectConfig.LastOpenDate != LastOpenDate) &&
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
		Project();
		~Project();
#pragma region PathFunction
		[[nodiscard]] const std::filesystem::path& GetProjectDirectory() const { return m_Config.ProjectPath; }
		[[nodiscard]] const std::filesystem::path& GetAssetDirectory() const { return m_Config.AssetDirectory; }
		[[nodiscard]] const std::filesystem::path& GetAssetRegistryPath() const { return m_Config.AssetRegistryPath; }
		void SetLastSceneAsset(const std::filesystem::path& sceneAsset) { m_Config.LastScene = sceneAsset; }


		std::filesystem::path& GetAppDirectory() { return m_Config.AppDirektory; }

		static const std::filesystem::path& GetActiveProjectDirectory()
		{
			RY_CORE_ASSERT(s_ActiveInstancProject, "Error: Project::GetProjectDirectory()");
			return s_ActiveInstancProject->GetProjectDirectory();
		}


#pragma endregion


		static const std::filesystem::path& GetActiveAssetDirectory()
		{
			RY_CORE_ASSERT(s_ActiveInstancProject, "Erroe: Project::GetAssetDirectory()");
			return s_ActiveInstancProject->GetAssetDirectory();
		}

		static std::filesystem::path GetActiveAssetRegistryPath()
		{
			RY_CORE_ASSERT(s_ActiveInstancProject, "Erroe: Project::GetActiveAssetRegistryPath()");
			return s_ActiveInstancProject->GetAssetRegistryPath();
		}

		static const std::filesystem::path& GetActiveScriptingAppDirektory()
		{
			RY_CORE_ASSERT(s_ActiveInstancProject, "Erroe: Project::GetProjectDirectory()");
			return s_ActiveInstancProject->m_Config.ScriptAppPath;
		}

		static const std::filesystem::path& GetActiveProjectScriptingCoreDirektory()
		{
			RY_CORE_ASSERT(s_ActiveInstancProject, "Erroe: Project::GetActiveAssetProjectScriptingCoreDirektory()");
			return s_ActiveInstancProject->m_Config.ScriptCorePath;
		}

		static void SetActiveLastSceneAsset(const std::filesystem::path& sceneAsset)
		{
			RY_CORE_ASSERT(s_ActiveInstancProject, "Erroe: Project::GetActiveAssetProjectScriptingCoreDirektory()");
			if(s_ActiveInstancProject->m_AssetManger->IsAssetHandleValid(sceneAsset))
				s_ActiveInstancProject->SetActiveLastSceneAsset(sceneAsset);
		}

		static void Init()
		{
			RY_CORE_WARN("Projekt Init! Beginn");
			RY_CORE_ASSERT(!s_ActiveInstancProject, "Project!");
			if(!s_ActiveInstancProject)
			{
				s_ActiveInstancProject = CreateRef<Project>();
				
				RY_CORE_INFO("Projekt Init! Sucess");
			}
			
			
		}

		static void ShutDown()
		{
			RY_CORE_WARN("Projekt ShutDown! Beginn");
			if (s_ActiveInstancProject->m_Config.ProjectRady)
			{
				s_ActiveInstancProject->m_AssetManger->OnDetach();
				s_ActiveInstancProject->m_AssetManger.reset();
				RY_DESTROY_REF(s_ActiveInstancProject);
				RY_CORE_INFO("Projekt ShutDown! Sucess");
			}
		}	

		

		ProjectConfig& GetConfig() { return m_Config; }

		static Ref<Project> GetActive() { return s_ActiveInstancProject; }

		Ref<AssetManagerBase> GetAssetManger() { return m_AssetManger; }
		Ref<RuntimeAssetManager> GetRuntimeAssetManger() const { return std::static_pointer_cast<RuntimeAssetManager>(m_AssetManger); }

		Ref<EditorAssetManagerThread> GetEditorAssetManger() const { return  std::static_pointer_cast<EditorAssetManagerThread>(m_AssetManger); }

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
		
		inline static Ref<Project> s_ActiveInstancProject;
	};


}