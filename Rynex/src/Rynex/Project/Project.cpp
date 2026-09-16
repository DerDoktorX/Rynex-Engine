#include "rypch.h"
#include "Project.h"

#include <Rynex/Serializers/ProjectSerialiazer.h>
#include <Rynex/Utils/PlatformUtils.h>
#include <Rynex/Project/Path.h>

namespace Rynex {

    namespace Utils {

 #ifdef RY_PLATFORM_WINDOWS

        static void CreateSetupScriptWin(const std::filesystem::path& pFolder, const std::filesystem::path& rynexRootPermake)
        {
            RY_CORE_INFO("The SetubProject File is for vs2026 conig");
            std::filesystem::path setubProjectFileName = std::filesystem::path("SetubProject.bat");
            std::filesystem::path filePath = pFolder / setubProjectFileName;
            std::filesystem::path relativeFilePath = std::filesystem::relative(rynexRootPermake, pFolder);
            std::string relativeFilePathStr = relativeFilePath.generic_string();

            std::string setupFile = "call " + relativeFilePathStr + " vs2026\nPAUSE";
            std::ofstream fout(filePath);
            fout << setupFile;
            fout.close();
            
        }

#define RY_CREATE_SETUP_SCRIPT(...) CreateSetupScriptWin(__VA_ARGS__)

#else

#defien RY_CREATE_SETUP_SCRIPT(...) RY_CORE_ERROR("We Cound't Create A SetupProject File for you! Becaouse we seport not all platforms Yet!")
// Alternative you can exexute in your console a script that execute in your Rynex directary in vendor\bin\premake\premake5.exe
#endif
        static bool CreateProjectScriptPremake(const std::filesystem::path& pFolder, const std::filesystem::path& rynexRootDir, const std::filesystem::path& origePreamke)
        {
            std::ifstream file(origePreamke, std::ios::binary);
            std::string line, projectPremake;
            try
            {

                projectPremake = "local RynexRootDir =  '" + std::filesystem::relative(rynexRootDir.parent_path(), pFolder).generic_string() + "'\n";
                while (std::getline(file, line))
                {
                    if (line == "local RynexRootDir = '../../../..'\r" || line == "outputdir = \"% {cfg.buildcfg} - % {cfg.system} - % {cfg.architecture}\" \r")
                        continue;
                    else if (line == "		--\"Assets/Scripts/**.cs\",\r")
                        line = "		\"Assets/Scripts/**.cs\",\r";
                    else if (line == "		--\"Assets/**.cs\",\r")
                        line = "		\"Assets/**.cs\",\r";
                    projectPremake += line ;
                }
                file.close();
            }
            catch (const std::exception e)
            {
                RY_CORE_ERROR("Error: {}", e.what());
                return false;
            }
            std::filesystem::path premakeFileName = std::filesystem::path("premake5.lua");
            std::filesystem::path premakePath = pFolder / premakeFileName;
            std::ofstream fout(premakePath);
            fout << projectPremake;
            fout.close();
            return true;
        }

        static bool BulidScriptingProject(ProjectConfig& pConfig)
        {
            std::filesystem::path rynexRootDir = std::filesystem::absolute(std::filesystem::current_path());
            std::filesystem::path rynexRootParentDir = rynexRootDir.parent_path();

            std::filesystem::path origePreamke = rynexRootDir / std::filesystem::path("SandboxProject/Assets/Scripts/premake5.lua");
            if (!CreateProjectScriptPremake(pConfig.m_ProjectPath, rynexRootDir, origePreamke))
                return false;
            
            std::filesystem::path premakeDir = std::filesystem::path("vendor/bin/premake/premake5.exe");
            std::filesystem::path scriptPath = rynexRootParentDir / premakeDir;
            RY_CREATE_SETUP_SCRIPT(pConfig.m_ProjectPath, scriptPath);
           
            return true;
        }

        static std::filesystem::path SetupFolder(const std::filesystem::path& pFolder,const std::string& name)
        {
            std::filesystem::path pathName = std::filesystem::path(name);
            std::filesystem::path absultPath = pFolder / pathName;
            try 
            {
                RY_CORE_ASSERT(std::filesystem::create_directory(absultPath), "Folder Existig alraedy!");
                return absultPath;
            }
            catch (const std::filesystem::filesystem_error& e) 
            {
                RY_CORE_ERROR("Error: {}", e.what());
                return "";
            }
            return "";
        }

        static bool CreateProjectFolderStruct(ProjectConfig& pConfig, const std::filesystem::path& pFolder, const std::string& name = "")
        {
            bool result = false;
            pConfig.m_Name = name=="" ? "Project-Create-Test": name;
            std::filesystem::path pwFolder = SetupFolder(pFolder, pConfig.m_Name);
            
            pConfig.m_ProjectPath = pwFolder;
            pConfig.m_AppDirectory = pwFolder.parent_path();
            pConfig.m_AssetDirectory = SetupFolder(pwFolder, "Assets");
            SetupFolder(pwFolder, "Assets/Scene");
            SetupFolder(pwFolder, "Assets/Texture");
            SetupFolder(pwFolder, "Assets/Models");
            SetupFolder(pwFolder, "Assets/Scripts");
            SetupFolder(pwFolder, "Assets/Shaders");

            pConfig.m_ScriptAppPath = SetupFolder(pwFolder, "Binaries"); 
            pConfig.m_ScriptCorePath = pConfig.m_ScriptAppPath;
            pConfig.m_ScriptAppPath /= "Sanbox.dll";
            pConfig.m_ScriptCorePath /= "Rynex-ScriptingCore.dll";
            SetupFolder(pwFolder, "Intermediates");

            result = true;
            
            return result;
        }

        static std::string ConvertTimeAsStringTimePoint(std::time_t time)
        {
            std::tm* localTimePtr = std::localtime(&time);
            std::ostringstream oss;
            char buffer[80];
            std::strftime(buffer, sizeof(buffer), "%d.%m.%Y %H:%M:%S", localTimePtr);
            std::string timePoint = buffer;
            return timePoint;
        }

        static void CreateProject(ProjectConfig& pConfig, std::filesystem::path pFolder = "", const std::string& name = "")
        {
            if(pFolder == "")
                pFolder = FileDialoges::SelectFolder();
            
            CreateProjectFolderStruct(pConfig, pFolder, name);
            if (!BulidScriptingProject(pConfig))
                return;

            auto now = std::chrono::system_clock::now();
            std::time_t now_c = std::chrono::system_clock::to_time_t(now);
            pConfig.m_LastOpenDate = ConvertTimeAsStringTimePoint(now_c);
            pConfig.m_CreateDate = ConvertTimeAsStringTimePoint(now_c);

           

            std::filesystem::path assetRegistryFileName = std::filesystem::path("AssetRegistry.ryr");
            pConfig.m_AssetRegistryPath = pConfig.m_ProjectPath / assetRegistryFileName;

            pConfig.m_ProjectRady = true;
            
        }


    }


    Project::Project()
    {
    }

    Project::~Project()
    {
        RY_DESTROY_REF(m_AssetManger);
    }

    Ref<Project> Project::New()
    {
        s_ActiveInstanceProject = CreateRef<Project>();
        return s_ActiveInstanceProject;
    }

  

    Ref<Project> Project::CreatNewProject()
    {
        if(s_ActiveInstanceProject && s_ActiveInstanceProject->m_Config.m_ProjectRady)
        {
            char answer;
            printf_s("\n Did you want that your current progress in the current Project get lost?\n> (y/n): ");
            scanf_s("%c", &answer);
            switch (answer)
            {
            case 'y':
                SaveActive(s_ActiveInstanceProject->m_Config.m_ProjectPath);
                break;
            case 'n':
                break;
            default:
                break;
            }
        }
        s_ActiveInstanceProject.reset();
        s_ActiveInstanceProject = CreateRef<Project>();
        ProjectConfig& pConfig = s_ActiveInstanceProject->m_Config;
        Utils::CreateProject(pConfig);
        std::filesystem::path fileNamePath = pConfig.m_Name + ".ryproj";
        std::filesystem::path filePath = pConfig.m_ProjectPath / filePath;
        Project::SaveActive(filePath);
        Ref<EditorAssetManagerThread> editorAssetManager = s_ActiveInstanceProject->GetEditorAssetManger();

       if (editorAssetManager)
       {
           editorAssetManager->SerializeAssetRegistry();
       }
       else
       {
           Ref<EditorAssetManagerThread> editorAssetManagerN = CreateRef<EditorAssetManagerThread>();
           editorAssetManagerN->SerializeAssetRegistry();
           s_ActiveInstanceProject->m_AssetManger = editorAssetManagerN;
       }
       return s_ActiveInstanceProject;
    }

    Ref<Project> Project::CreatNewProject(const std::filesystem::path& projectPath, const std::string& name)
    {
        
        Ref<Project> project = CreateRef<Project>();
        ProjectConfig& pConfig = s_ActiveInstanceProject->m_Config;
        Utils::CreateProject(pConfig, projectPath, name);
        std::filesystem::path fileNamePath = pConfig.m_Name + ".ryproj";
        std::filesystem::path filePath = pConfig.m_ProjectPath / filePath;

        Project::SaveActive(filePath);
        Ref<EditorAssetManagerThread> editorAssetManager = s_ActiveInstanceProject->GetEditorAssetManger();



        if (editorAssetManager)
        {
            editorAssetManager->SerializeAssetRegistry();
        }
        else
        {
            Ref<EditorAssetManagerThread> editorAssetManagerN = CreateRef<EditorAssetManagerThread>();
            editorAssetManagerN->SerializeAssetRegistry();
            s_ActiveInstanceProject->m_AssetManger = editorAssetManagerN;
        }
        s_ActiveInstanceProject = project;
        return project;
    }

   

    Ref<Project> Project::Load(const std::filesystem::path& path)
    {
        RY_CORE_INFO("Load Project from Path: '{0}'",path.string().c_str());
        if(!s_ActiveInstanceProject)
        {
            RY_DESTROY_REF(s_ActiveInstanceProject);
        }
        
        Ref<Project> project = Project::New();

        ProjectSerialiazer serialiazer(project);
        if (serialiazer.Deserlize(path))
        {
            project->m_Config.m_ProjectPath = FileSystem::Path(path.parent_path()).GetAbsolutePath();

            Ref<EditorAssetManagerThread> editorAssetManager = CreateRef<EditorAssetManagerThread>();
            editorAssetManager->OnAttach();
            s_ActiveInstanceProject->m_AssetManger = editorAssetManager;

            // editorAssetManager->DeserializeAssetRegistry();
            RY_CORE_ERROR("Project Loading For Editor Sucese");

            return s_ActiveInstanceProject;
        }
        RY_CORE_ERROR("Project Loading Faild");
        
        return nullptr;
    }

    bool Project::SaveActive(const std::filesystem::path& path)
    {
        RY_CORE_ASSERT(path.has_filename(), "no File!");
        ProjectSerialiazer serializer(s_ActiveInstanceProject);
        if (serializer.Serlize(path))
        {
            s_ActiveInstanceProject->m_Config.m_ProjectPath = s_ActiveInstanceProject->m_Config.m_ProjectPath.empty() ? path.parent_path() : s_ActiveInstanceProject->m_Config.m_ProjectPath;
            RY_CORE_INFO("Save Project Settings Under '{}'", path.string().c_str());
            return true;
        }
        RY_CORE_ERROR("Don#t Save Project Settings Under '{}'", path.string().c_str());
        return false;
    }



}

