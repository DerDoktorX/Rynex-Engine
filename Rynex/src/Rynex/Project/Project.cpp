#include "rypch.h"
#include "Project.h"

#include "Rynex/Serializers/ProjectSerialiazer.h"
#include "Rynex/Utils/PlatformUtils.h"

namespace Rynex {

    namespace Utils {

 #ifdef RY_PLATFORM_WINDOWS

        static void CreateSetupScriptWin(const std::filesystem::path& pFolder, const std::filesystem::path& rynexRootPermake)
        {
            RY_CORE_INFO("The SetubProject File is for vs2020 conig");
            std::filesystem::path filePath = pFolder / "SetubProject.bat";
            std::string setupFile = "call " + std::filesystem::relative(rynexRootPermake, pFolder).string() + " vs2022\nPAUSE";
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

            std::ofstream fout(pFolder/"premake5.lua");
            fout << projectPremake;
            fout.close();
            return true;
        }

        static bool BulidScriptingProject(ProjectConfig& pConfig)
        {
            std::filesystem::path rynexRootDir = std::filesystem::absolute(std::filesystem::current_path());
            if (!CreateProjectScriptPremake(pConfig.ProjectPath, rynexRootDir, rynexRootDir / "SandboxProject\\Assets\\Scripts\\premake5.lua"))
                return false;
            RY_CREATE_SETUP_SCRIPT(pConfig.ProjectPath, rynexRootDir.parent_path() / "vendor\\bin\\premake\\premake5.exe");
           
            return true;
        }

       static std::filesystem::path SetupFolder(const std::filesystem::path& pFolder,const std::string& name)
        {
            
            // std::filesystem::path absultPath = std::filesystem::absolute(pFolder);
            std::filesystem::path absultPath = pFolder / name;

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
            pConfig.Name = name=="" ? "Project-Create-Test": name;
            std::filesystem::path pwFolder = SetupFolder(pFolder, pConfig.Name);
            
            pConfig.ProjectPath = pwFolder;
            pConfig.AppDirektory = pwFolder.parent_path();
            pConfig.AssetDirectory = SetupFolder(pwFolder, "Assets");
            SetupFolder(pwFolder, "Assets/Scene");
            SetupFolder(pwFolder, "Assets/Texture");
            SetupFolder(pwFolder, "Assets/Models");
            SetupFolder(pwFolder, "Assets/Scripts");
            SetupFolder(pwFolder, "Assets/Shaders");

            pConfig.ScriptAppPath = SetupFolder(pwFolder, "Binaries"); 
            pConfig.ScriptCorePath = pConfig.ScriptAppPath;
            pConfig.ScriptAppPath /= "Sanbox.dll";
            pConfig.ScriptCorePath /= "Rynex-ScriptingCore.dll";
            SetupFolder(pwFolder, "Intermediates");

            result = true;
            
            return result;
        }

        

        static void CreateProject(ProjectConfig& pConfig, std::filesystem::path pFolder = "", const std::string& name = "")
        {
            if(pFolder == "")
                pFolder = FileDialoges::SelectFolder();
            

            // pConfig.StartScene = pwFolder/"Assets/Scene/Default.ryscene";
            CreateProjectFolderStruct(pConfig, pFolder, name);
            if (!BulidScriptingProject(pConfig))
                return;

            auto now = std::chrono::system_clock::now();
            std::time_t now_c = std::chrono::system_clock::to_time_t(now);
            std::tm* local_time = std::localtime(&now_c);
            pConfig.LastOpenDate = std::put_time(local_time, "%d.%m.%Y %H:%M:%S")._Fmtfirst;
            pConfig.CreateDate = std::put_time(local_time, "%d.%m.%Y %H:%M:%S")._Fmtfirst;
            pConfig.AssetRegistryPath = pConfig.ProjectPath / "AssetRegistry.ryr";

            pConfig.ProjectRady = true;
            
        }

    }


    Ref<Project> Project::New()
    {
        s_ActiveProject = CreateRef<Project>();
        return s_ActiveProject;
    }

    Ref<Project> Project::CreatNewPorject()
    {
        if(s_ActiveProject&& s_ActiveProject->m_Config.ProjectRady)
        {
            char answer;
            printf_s("\n Did you want that your current progress in the curent Project get lost?\n> (y/n): ");
            scanf_s("%c", &answer);
            switch (answer)
            {
            case 'y':
                SaveActive(s_ActiveProject->m_Config.ProjectPath);
                break;
            case 'n':
                break;
            default:
                break;
            }
        }
        s_ActiveProject.reset();
        s_ActiveProject = CreateRef<Project>();
        ProjectConfig& pConfig = s_ActiveProject->m_Config;
       Utils::CreateProject(pConfig);
       Project::SaveActive((pConfig.ProjectPath / (pConfig.Name + ".rproj")));
#if RY_EDITOR_ASSETMANGER_THREADE
       Ref<EditorAssetManegerThreade>& editorAssetManager = s_ActiveProject->GetEditorAssetManger();
#else
       Ref<EditorAssetManager>& editorAssetManager = s_ActiveProject->GetEditorAssetManger();
#endif

       if (editorAssetManager)
       {
           editorAssetManager->SerialzeAssetRegistry();
       }
       else
       {
#if RY_EDITOR_ASSETMANGER_THREADE
           Ref<EditorAssetManegerThreade> editorAssetManagerN = CreateRef<EditorAssetManegerThreade>();
           editorAssetManagerN->SerialzeAssetRegistry();
           s_ActiveProject->m_AssetManger = editorAssetManagerN;
#else
           Ref<EditorAssetManager> editorAssetManagerN = CreateRef<EditorAssetManager>();
           editorAssetManagerN->SerialzeAssetRegistry();
           s_ActiveProject->m_AssetManger= editorAssetManagerN;
#endif
       }
       return s_ActiveProject;
    }

    Ref<Project> Project::CreatNewPorject(const std::filesystem::path& projectPath, const std::string& name)
    {
        
        Ref<Project> project = CreateRef<Project>();
        ProjectConfig& pConfig = s_ActiveProject->m_Config;
        Utils::CreateProject(pConfig, projectPath, name);
        Project::SaveActive((pConfig.ProjectPath / (pConfig.Name + ".rproj")));
#if RY_EDITOR_ASSETMANGER_THREADE
        Ref<EditorAssetManegerThreade>& editorAssetManager = s_ActiveProject->GetEditorAssetManger();
#else
        Ref<EditorAssetManager>& editorAssetManager = s_ActiveProject->GetEditorAssetManger();
#endif


        if (editorAssetManager)
        {
            editorAssetManager->SerialzeAssetRegistry();
        }
        else
        {
#if RY_EDITOR_ASSETMANGER_THREADE
            Ref<EditorAssetManegerThreade> editorAssetManagerN = CreateRef<EditorAssetManegerThreade>();
            editorAssetManagerN->SerialzeAssetRegistry();
#else
            Ref<EditorAssetManager> editorAssetManagerN = CreateRef<EditorAssetManager>();
            editorAssetManagerN->SerialzeAssetRegistry();
#endif
            s_ActiveProject->m_AssetManger = editorAssetManagerN;
        }
        s_ActiveProject = project;
        return project;
    }

   

    Ref<Project> Project::Load(const std::filesystem::path& path)
    {
        RY_CORE_INFO("Load Project from Path: '{0}'",path.string().c_str());
        if(!s_ActiveProject)
        {
            s_ActiveProject.reset();
        }
        
        Ref<Project> project = CreateRef<Project>();

        ProjectSerialiazer serialiazer(project);
        if (serialiazer.Deserlize(path))
        {
            project->m_Config.ProjectPath = path.parent_path();
            s_ActiveProject = project;
#if RY_EDITOR_ASSETMANGER_THREADE
            Ref<EditorAssetManegerThreade> editorAssetManager = CreateRef<EditorAssetManegerThreade>();
            editorAssetManager->OnAttach();
            s_ActiveProject->m_AssetManger = editorAssetManager;

            editorAssetManager->DeserialzeAssetRegistry();
#else
            Ref<EditorAssetManager> editorAssetManager = CreateRef<EditorAssetManager>();
            editorAssetManager->OnAttach();
            s_ActiveProject->m_AssetManger = editorAssetManager; 

            editorAssetManager->DeserialzeAssetRegistry();
#endif
            RY_CORE_ERROR("Project Loading For Editor Sucese");
            return s_ActiveProject;
        }
        RY_CORE_ERROR("Project Loading Faild");
        
        return nullptr;
    }

    bool Project::SaveActive(const std::filesystem::path& path)
    {
        RY_CORE_ASSERT(path.has_filename(), "no File!");
        ProjectSerialiazer serializer(s_ActiveProject);
        if (serializer.Serlize(path))
        {
            s_ActiveProject->m_Config.ProjectPath = s_ActiveProject->m_Config.ProjectPath.empty() ? path.parent_path() : s_ActiveProject->m_Config.ProjectPath;
            RY_CORE_INFO("Save Project Settings Under '{}'", path.string().c_str());
            return true;
        }
        RY_CORE_ERROR("Don#t Save Project Settings Under '{}'", path.string().c_str());
        return false;
    }



}

