#include "rypch.h"
#include "Project.h"

#include "Rynex/Serializers/ProjectSerialiazer.h"
#include "Rynex/Utils/PlatformUtils.h"
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
            if (!CreateProjectScriptPremake(pConfig.ProjectPath, rynexRootDir, origePreamke))
                return false;
            
            std::filesystem::path premakeDir = std::filesystem::path("vendor/bin/premake/premake5.exe");
            std::filesystem::path scriptPath = rynexRootParentDir / premakeDir;
            RY_CREATE_SETUP_SCRIPT(pConfig.ProjectPath, scriptPath);
           
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
            pConfig.LastOpenDate = ConvertTimeAsStringTimePoint(now_c);
            pConfig.CreateDate = ConvertTimeAsStringTimePoint(now_c);

           

            std::filesystem::path assetRegistryFileName = std::filesystem::path("AssetRegistry.ryr");
            pConfig.AssetRegistryPath = pConfig.ProjectPath / assetRegistryFileName;

            pConfig.ProjectRady = true;
            
        }


    }


    Project::Project()
    {
    }

    Project::~Project()
    {
        RY_DESTROY_REF(m_AssetManger);
    }
#pragma region PathFunction

    std::filesystem::path Project::GenarteProjectAbosult(const std::filesystem::path& path)
    {
        if (path.is_absolute())
            return path;
        std::string pathStr = path.string();
        uint32_t index = pathStr.find("../Rynex-Editor");
        uint32_t count = pathStr.size();
        if (index < count)
        {
            std::filesystem::path absolutePath = std::filesystem::absolute(path);
            return absolutePath;
        }
        else if (pathStr.empty())
        {
            const std::filesystem::path& projectFolder = Project::GetActiveProjectDirectory();
            std::filesystem::path absolutePath = projectFolder / path;
            return absolutePath;
        }
        else
        {
            return "";
        }
    }

    std::filesystem::path Project::GenarteProjectRaltivPath(const std::filesystem::path& path)
    {
        return path;
    }

    std::string Project::CreateAssetInterlPathFormatProject(const std::filesystem::path& path)
    {
        constexpr const char* name = "Project";
        constexpr const char* marker = RY_PATH_PROJECT_MARKER_STR;
        std::filesystem::path pathCreate = path;
        std::string pathCreateStr = path.string();
        const std::filesystem::path& projectFolder = Project::GetActiveProjectDirectory();
        if (pathCreate.is_relative())
        {
            if (!Project::IsPathExisting(path, projectFolder, name))
                return "";
        }
        else if (pathCreate.is_absolute())
        {
            pathCreate = std::filesystem::relative(pathCreate, projectFolder);
            if (!Project::IsPathExisting(pathCreate))
                return "";
        }

        pathCreateStr = SetMarker(pathCreate, marker);

        return pathCreateStr;
    }

    bool Project::IsPathExisting(const std::filesystem::path& path)
    {
        if (!std::filesystem::exists(path))
        {
            RY_CORE_FATAL("File path, has not found on disk ({}) on your PC!", path);
            return false;
        }
        return true;
    }

    bool Project::IsPathExisting(const std::filesystem::path& path, const std::filesystem::path& base, const std::string& baseName)
    {
        std::filesystem::path absoluteFilePath = base / path;
        if (!std::filesystem::exists(absoluteFilePath))
        {
            RY_CORE_FATAL("File path, has not found on disk ({}) in sinde the {} directory!", path, baseName);
            RY_CORE_ERROR("Abosult file path was ({})", absoluteFilePath);
            return false;
        }
        return true;

    }

    std::string Project::SetMarker(const std::filesystem::path& path, const std::string& marker)
    {
        constexpr const char* directoryDiffern = "/";
        std::string pathStr = path.generic_string();
        pathStr = marker + directoryDiffern + pathStr;
        
        return pathStr;
    }

    size_t Project::SearchInPathFor(const std::filesystem::path& path, const std::string& searchItem)
    {
        constexpr size_t searchFrom = 0ull;
        std::string searchePath = path.string();
        size_t pos = SearchInPathFor(searchePath, searchItem);
        return pos;
    }

    

    size_t Project::SearchInPathFor(const std::string& pathStr, const std::string& searchItem)
    {
        constexpr size_t searchFrom = 0ull;
        size_t pos = pathStr.find(searchItem, searchFrom);
        return pos;
    }

 
    

    bool Project::HasStringInPath(const std::filesystem::path& path, const std::string& searchItem)
    {
        std::string searchePath = path.string();
        
        bool hasMarker = HasStringInPath(searchePath, searchItem);
        return hasMarker;
    }
    
    bool Project::HasStringInPath(const std::string& pathStr, const std::string& searchItem)
    {
        size_t size = pathStr.size();
        size_t pos = SearchInPathFor(pathStr, searchItem);
        bool hasMarker = pos < size;
        return hasMarker;
    }


    size_t Project::PositionMarker(const std::filesystem::path& path, const std::string& marker)
    {
        std::string searchePath = path.string();
        size_t pos = PositionMarker(searchePath, marker);
        return pos;
    }

    size_t Project::PositionMarker(const std::string& pathStr, const std::string& marker)
    {
        constexpr const char* directoryDiffern = "/";
        constexpr size_t searchFrom = 0ull;
        std::string searcheMarker = marker + directoryDiffern;
        size_t pos = SearchInPathFor(pathStr, searcheMarker);
        return pos;
    }

    bool Project::HasMarker(const std::string& pathStr, const std::string& markerStr)
    {
        std::string searchePath = pathStr;
        size_t size = searchePath.size();
        size_t pos = PositionMarker(searchePath, markerStr);
        bool hasMarker = pos < size;
        return hasMarker;
    }

    bool Project::HasSomeMarker(const std::string& pathStr)
    {
        constexpr const char* markerSymbols = "#!#";
        constexpr size_t markerSymbolsSize = 2;
        constexpr size_t expextedCount = 1;

        const std::string& searchePath = pathStr;
        size_t offset = 0ull;
        size_t size = searchePath.size();
        size_t countFound = 0ull;
        size_t i = 0ull;
        do
        {
            countFound++;
            offset = searchePath.find(markerSymbols, offset);
            offset += markerSymbolsSize;
            i++;
        } while (offset < size && i < expextedCount);

        bool hasMarker = expextedCount <= countFound;
        {
            
            FileSystem::Path path(pathStr);
            bool check = hasMarker && path.IsMarked();
            RY_CORE_INFO_IF(!check, "Pass Test: Has Marker Path!");
            RY_CORE_ERROR_IF(check, "FAILD Test: Has Marker Path!");
           
            RY_CORE_ASSERT(check);
        }

        return hasMarker;
    }

    std::filesystem::path Project::RemoveMarker(const std::string& pathStr, const std::string& marker)
    {
        constexpr size_t offset = 1;
        const std::string& searchePath = pathStr;
        size_t size = searchePath.size();
        size_t pos = PositionMarker(searchePath, marker);
        size_t sizeMarker = marker.size();
        size_t endPosMarker = sizeMarker + pos + offset;
        std::string pathWithoutMarkerStr = searchePath.substr(endPosMarker);
        std::filesystem::path pathWithoutMarker = pathWithoutMarkerStr;
        pathWithoutMarkerStr = pathWithoutMarker.generic_string();
        pathWithoutMarker = pathWithoutMarkerStr;
        return pathWithoutMarker;
    }


    std::string Project::ExtraxtMarker(const std::string& pathStr)
    {
        constexpr size_t offset = 0ull;
        constexpr uint32_t markerCount = 2;
        constexpr const char* markersArray[markerCount] = {
            RY_PATH_PROJECT_MARKER_STR,
            RY_PATH_ENGINE_MARKER_STR
        };
        const std::string& searchePath = pathStr;
        size_t size = searchePath.size();
        size_t pos = MAXSIZE_T;
        uint32_t i = 0;
        do {
            pos = PositionMarker(searchePath, markersArray[i]);
            i++;
        } while (i < markerCount && size <= pos);
        if (size <= pos)
        {
            RY_CORE_ERROR("No Marker Found To Extraxt in Path: {}", pathStr);
            return "";
        }
        size_t markerIndex = i - 1;
        std::string markerInside = markersArray[markerIndex];
        return markerInside;
    }

    

    std::filesystem::path Project::ReplaceMarkerWitheAbsolutePath(const std::string& pathStr)
    {
        constexpr uint32_t markerCount = 2;
        constexpr const char* markersArray[markerCount] = {
            RY_PATH_PROJECT_MARKER_STR,
            RY_PATH_ENGINE_MARKER_STR
        };
        std::filesystem::path markersPathsArray[markerCount] = {
            Project::GetActiveProjectDirectory(),
            std::filesystem::current_path()
        };

        if (!HasSomeMarker(pathStr))
        {
            RY_CORE_ERROR("File Path has no Marker! {}", pathStr);
            return std::filesystem::path("");
        }
        std::string marker = ExtraxtMarker(pathStr);
        if (!IsMarkerVaild(marker))
        {
            RY_CORE_ERROR("Did't found a vaild Marker! {}", marker);
            return std::filesystem::path("");
        }

        std::filesystem::path base = GetAbsulteFilePathFormMarker(marker);
        if (base.empty())
        {
            RY_CORE_ERROR("Has No Valid Path Base! {}", base);
            return std::filesystem::path("");
        }


        std::filesystem::path absultePath = ReplaceMarkerWithePath(pathStr, marker, base);
        
        return absultePath;
    }

    std::filesystem::path Project::ReplaceMarkerWithePath(const std::string& pathStr, const std::string& marker, const std::filesystem::path& base)
    {
        std::filesystem::path withOutMarker = RemoveMarker(pathStr, marker);
        std::filesystem::path absultePath = base / withOutMarker;
        std::string absultePathStr = absultePath.generic_string();
        absultePath = absultePathStr;
        absultePath = absultePath.lexically_normal();
        {

            FileSystem::Path fileSystemPath(pathStr);
            std::filesystem::path pathAbsulte = fileSystemPath.GetAbsolutePath();
            bool check = absultePath == pathAbsulte;
            RY_CORE_INFO_IF(!check, "Pass Test: Remplaced Marked Path withe Absoulte!");
            RY_CORE_ERROR_IF(check, "FAILD Test: Remplaced Marked Path withe Absoulte!");
            RY_CORE_ERROR_IF(check, "FileSystem::Path: {}", pathAbsulte);
            RY_CORE_ERROR_IF(check, "Orignele:         {}", absultePath);
            RY_CORE_ASSERT(check);
        }
        return absultePath;
    }

    

    uint32_t Project::GetMarkerVaild(const std::string& marker)
    {
        constexpr uint32_t markerCount = 4;
        constexpr uint32_t markerVaildCount = 2;
        constexpr const char* markersArray[markerCount] = {
            RY_PATH_PROJECT_MARKER_STR,
            RY_PATH_ENGINE_MARKER_STR,
            RY_PATH_NO_VALID_MARKER_STR,
            ""
        };
        uint32_t i = 0;
        while (i < markerCount && marker != markersArray[i])
        {
            i++;
        }
        return i < markerVaildCount;
    }

    bool Project::IsMarkerVaild(const std::string& marker)
    {
        constexpr uint32_t markerCount = 4;
        constexpr uint32_t markerVaildCount = 2;
        constexpr const char* markersArray[markerCount] = {
            RY_PATH_PROJECT_MARKER_STR,
            RY_PATH_ENGINE_MARKER_STR,
            RY_PATH_NO_VALID_MARKER_STR,
            ""
        };
        uint32_t i = 0;
        while (i < markerCount && marker != markersArray[i])
        {
            i++;
        }
        return i < markerVaildCount;
    }

    std::string Project::GeanrateRealtivePathWitheMarker(const std::filesystem::path& path)
    {
        std::pair<std::string, std::filesystem::path> pair = GeanrateRealtivePathAndMarker(path);
        const std::string& marker = pair.first;
        const  std::filesystem::path& realtive = pair.second;
        if (!IsMarkerVaild(marker))
        {
            RY_CORE_ERROR("Did't found a vaild Marker! {}", marker);
            return std::string();
        }
        std::filesystem::path base = Project::GetAbsulteFilePathFormMarker(marker);
        std::filesystem::path realtiveFromBase;
        if (realtive.is_absolute())
            realtiveFromBase = std::filesystem::relative(realtive, base);
        else
            realtiveFromBase = realtive;


        std::string pathWitheMarker = SetMarker(realtiveFromBase, marker);
        std::replace(pathWitheMarker.begin(), pathWitheMarker.end(), '\\', '/');
        {

            FileSystem::Path fileSystemPath(path);
            std::string markerPathStr = fileSystemPath.GetMarkedPathString();
            bool check = markerPathStr == pathWitheMarker;
            RY_CORE_INFO_IF(!check, "Pass Test: Create Marked Path!");
            RY_CORE_ERROR_IF(check, "FAILD Test: Create Marked Path!");
            RY_CORE_ERROR_IF(check, "FileSystem::Path: {}", markerPathStr);
            RY_CORE_ERROR_IF(check, "Orignele:         {}", pathWitheMarker);
            RY_CORE_ASSERT(check);
        }
        return pathWitheMarker;
    }

    std::filesystem::path Project::GetAbsulteFilePathFormMarker(const std::string& marker)
    {
        constexpr uint32_t markerCount = 3;
        constexpr uint32_t markerVaildCount = 2;
        constexpr const char* markersArray[markerCount] = {
            RY_PATH_PROJECT_MARKER_STR,
            RY_PATH_ENGINE_MARKER_STR,
            RY_PATH_NO_VALID_MARKER_STR
        };
        std::filesystem::path markersPathsArray[markerCount] = {
            Project::GetActiveProjectDirectory(),
            std::filesystem::current_path(),
            std::filesystem::path("")
        };

        uint32_t i = 0;
        while (i < markerCount && marker != markersArray[i])
        {
            i++;
        }
        if (markerVaildCount < i)
        {
            RY_CORE_ERROR("Did't found a vaild Marker! {}", marker);
            return std::filesystem::path("");
        }
        std::filesystem::path absultePathMarker = markersPathsArray[i];
        return absultePathMarker;
    }

    std::pair<std::string, std::filesystem::path> Project::GeanrateRealtivePathAndMarker(const std::filesystem::path& path)
    {
        std::string marker = GetExpextedMarker(path);
        std::filesystem::path absultePath = GetAbsulteFilePathFormMarker(marker);
        if (path.empty())
        {
            RY_CORE_ERROR("Did't found a vaild Path! {}", path);
            return std::pair<std::string, std::filesystem::path>(RY_PATH_NO_VALID_MARKER_STR, "");
        }

        std::pair<std::string, std::filesystem::path> pair = GeanrateRealtivePathFromMarkerAndBase(path, absultePath, marker);
        return pair;
    }

    std::pair<std::string, std::filesystem::path> Project::GeanrateRealtivePathFromMarkerAndBase(const std::filesystem::path& path, const std::filesystem::path& base, const std::string& marker)
    {
        std::filesystem::path realtiveFromAbsultePath;
        if (path.is_absolute())
            realtiveFromAbsultePath = std::filesystem::relative(path, base);
        else
            realtiveFromAbsultePath = path;

        std::string realtiveFromBaseStr = realtiveFromAbsultePath.generic_string();
        realtiveFromAbsultePath = realtiveFromBaseStr;
        std::pair<std::string, std::filesystem::path> pair(marker, realtiveFromBaseStr);
        return pair;
    }

    std::string Project::GetExpextedMarker(const std::filesystem::path& path)
    {
        constexpr const char* projectMarker = RY_PATH_PROJECT_MARKER_STR;
        constexpr const char* engineMarker = RY_PATH_ENGINE_MARKER_STR;
        constexpr const char* engineRealtive = RY_PATH_EXPECT_ENGINE_RELATIVE_START_STR;

        std::filesystem::path projectPath = GetActiveProjectDirectory();
        std::filesystem::path appPath = std::filesystem::current_path();

        std::filesystem::path fileNamePath = std::filesystem::path(engineRealtive);
        std::filesystem::path appPathEngine = appPath / fileNamePath;
        std::string appPathEngineStr = appPathEngine.generic_string();

        std::string marker = "";
        if (path.is_relative())
        {
            if (Project::HasStringInPath(path, engineRealtive) || Project::HasStringInPath(path, appPathEngineStr))
            {
                marker = engineMarker;
            }
            else
            {
                marker = projectMarker;
            }
        }
        else if (path.is_absolute())
        {
            if (Project::IsAbsultePathSubPath(path, projectPath))
                marker = projectMarker;
            else if (Project::HasStringInPath(path, engineRealtive) || Project::HasStringInPath(path, appPathEngineStr))
                marker = engineMarker;
            else
                marker = projectMarker;
        }
        {

            FileSystem::Path fileSystemPath(path);
           
            FileSystem::Path::Origin origin = marker == projectMarker ? FileSystem::Path::Origin::Project 
                : (marker == engineMarker ? FileSystem::Path::Origin::Engine : FileSystem::Path::Origin::None);

            bool check = origin == fileSystemPath.GetOrigin();

            RY_CORE_INFO_IF(!check, "Pass Test: Expexted Path!");
            RY_CORE_ERROR_IF(check, "FAILD Test: Expexted Path!");

            RY_CORE_ASSERT(check);
        }

        return marker;
    }

    bool Project::IsAbsultePathSubPath(const std::filesystem::path& pathAbsolut, const std::filesystem::path& baseAbsolut)
    {
        RY_CORE_ASSERT(pathAbsolut.is_absolute() && baseAbsolut.is_absolute(), "We expext no Relative only Absulte Paths!");
        std::filesystem::path realitveTo = std::filesystem::relative(pathAbsolut, baseAbsolut);
        std::string realitveToStr = realitveTo.generic_string();
        bool areTheSame = !realitveToStr.empty() && realitveToStr.find("..") != 0;
        if (areTheSame && !Project::IsPathExisting(pathAbsolut))
            RY_CORE_WARN("The Path is Vaild but File is not Found {}", realitveToStr);
        return areTheSame;
    }

#pragma endregion

    Ref<Project> Project::New()
    {
        s_ActiveInstancProject = CreateRef<Project>();
        return s_ActiveInstancProject;
    }

  

    Ref<Project> Project::CreatNewPorject()
    {
        if(s_ActiveInstancProject&& s_ActiveInstancProject->m_Config.ProjectRady)
        {
            char answer;
            printf_s("\n Did you want that your current progress in the curent Project get lost?\n> (y/n): ");
            scanf_s("%c", &answer);
            switch (answer)
            {
            case 'y':
                SaveActive(s_ActiveInstancProject->m_Config.ProjectPath);
                break;
            case 'n':
                break;
            default:
                break;
            }
        }
        s_ActiveInstancProject.reset();
        s_ActiveInstancProject = CreateRef<Project>();
        ProjectConfig& pConfig = s_ActiveInstancProject->m_Config;
        Utils::CreateProject(pConfig);
        std::filesystem::path fileNamePath = pConfig.Name + ".ryproj";
        std::filesystem::path filePath = pConfig.ProjectPath / filePath;
        Project::SaveActive(filePath);
        Ref<EditorAssetManegerThreade> editorAssetManager = s_ActiveInstancProject->GetEditorAssetManger();

       if (editorAssetManager)
       {
           editorAssetManager->SerialzeAssetRegistry();
       }
       else
       {
           Ref<EditorAssetManegerThreade> editorAssetManagerN = CreateRef<EditorAssetManegerThreade>();
           editorAssetManagerN->SerialzeAssetRegistry();
           s_ActiveInstancProject->m_AssetManger = editorAssetManagerN;
       }
       return s_ActiveInstancProject;
    }

    Ref<Project> Project::CreatNewPorject(const std::filesystem::path& projectPath, const std::string& name)
    {
        
        Ref<Project> project = CreateRef<Project>();
        ProjectConfig& pConfig = s_ActiveInstancProject->m_Config;
        Utils::CreateProject(pConfig, projectPath, name);
        std::filesystem::path fileNamePath = pConfig.Name + ".ryproj";
        std::filesystem::path filePath = pConfig.ProjectPath / filePath;

        Project::SaveActive(filePath);
        Ref<EditorAssetManegerThreade> editorAssetManager = s_ActiveInstancProject->GetEditorAssetManger();



        if (editorAssetManager)
        {
            editorAssetManager->SerialzeAssetRegistry();
        }
        else
        {
            Ref<EditorAssetManegerThreade> editorAssetManagerN = CreateRef<EditorAssetManegerThreade>();
            editorAssetManagerN->SerialzeAssetRegistry();
            s_ActiveInstancProject->m_AssetManger = editorAssetManagerN;
        }
        s_ActiveInstancProject = project;
        return project;
    }

   

    Ref<Project> Project::Load(const std::filesystem::path& path)
    {
        RY_CORE_INFO("Load Project from Path: '{0}'",path.string().c_str());
        if(!s_ActiveInstancProject)
        {
            RY_DESTROY_REF(s_ActiveInstancProject);
        }
        
        Ref<Project> project = Project::New();

        ProjectSerialiazer serialiazer(project);
        if (serialiazer.Deserlize(path))
        {
            project->m_Config.ProjectPath = path.parent_path();
            
            Ref<EditorAssetManegerThreade> editorAssetManager = CreateRef<EditorAssetManegerThreade>();
            editorAssetManager->OnAttach();
            s_ActiveInstancProject->m_AssetManger = editorAssetManager;

            editorAssetManager->DeserialzeAssetRegistry();
            RY_CORE_ERROR("Project Loading For Editor Sucese");

            return s_ActiveInstancProject;
        }
        RY_CORE_ERROR("Project Loading Faild");
        
        return nullptr;
    }

    bool Project::SaveActive(const std::filesystem::path& path)
    {
        RY_CORE_ASSERT(path.has_filename(), "no File!");
        ProjectSerialiazer serializer(s_ActiveInstancProject);
        if (serializer.Serlize(path))
        {
            s_ActiveInstancProject->m_Config.ProjectPath = s_ActiveInstancProject->m_Config.ProjectPath.empty() ? path.parent_path() : s_ActiveInstancProject->m_Config.ProjectPath;
            RY_CORE_INFO("Save Project Settings Under '{}'", path.string().c_str());
            return true;
        }
        RY_CORE_ERROR("Don#t Save Project Settings Under '{}'", path.string().c_str());
        return false;
    }



}

