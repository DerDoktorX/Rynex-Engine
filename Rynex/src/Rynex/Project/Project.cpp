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
#if 0
        if (!path.is_absolute())
            return path;

        const std::filesystem::path& projectFolder = Project::GetActiveProjectDirectory();
        std::filesystem::path relativPath = std::filesystem::relative(path, projectFolder);
        return relativPath;
#elif 0
        if (path.is_absolute())
            return path;

        std::filesystem::path absolutePath = std::filesystem::absolute(path);
        return absolutePath;
#else
        return path;
#endif
    }

    std::filesystem::path Project::CreateAssetInterlPathFormatProject(const std::filesystem::path& path)
    {
        constexpr const char* name = "Project";
        constexpr const char* marker = RY_PATH_PROJECT_MARKER_STR;
        std::filesystem::path pathCreate = path;
        const std::filesystem::path& projectFolder = Project::GetActiveProjectDirectory();
        if (pathCreate.is_relative())
        {
            if (!Project::IsPathExisting(pathCreate, projectFolder, name))
                return "";
        }
        else if (pathCreate.is_absolute())
        {
            pathCreate = std::filesystem::relative(pathCreate, projectFolder);
            if (!Project::IsPathExisting(pathCreate))
                return "";
        }

        pathCreate = SetMarker(pathCreate, marker);

        return pathCreate;
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

    std::filesystem::path Project::SetMarker(const std::filesystem::path& path, const std::string& marker)
    {
        constexpr const char* directoryDiffern = "/";
        std::string pathStr = path.generic_string();
        pathStr = marker + directoryDiffern + pathStr;
        std::filesystem::path markedPath = std::filesystem::path(pathStr);
        std::string markedPathStr = markedPath.generic_string();
        markedPath = markedPathStr;
        return markedPath;
    }

    size_t Project::SearchInPathFor(const std::filesystem::path& path, const std::string& searchItem)
    {
        constexpr size_t searchFrom = 0ull;
        std::string searchePath = path.string();
        size_t pos = SearchInPathFor(searchePath, searchItem);
        return pos;
    }

    size_t Project::SearchInPathFor(const std::filesystem::path& path, const std::wstring& searchItem)
    {
        constexpr size_t searchFrom = 0ull;
        std::wstring searchePath = path.wstring();
        size_t pos = SearchInPathFor(searchePath, searchItem);
        return pos;
    }

    size_t Project::SearchInPathFor(const std::string& pathStr, const std::string& searchItem)
    {
        constexpr size_t searchFrom = 0ull;
        size_t pos = pathStr.find(searchItem, searchFrom);
        return pos;
    }

    size_t Project::SearchInPathFor(const std::wstring& pathWstr, const std::wstring& searchItem)
    {
        constexpr size_t searchFrom = 0ull;
        size_t pos = pathWstr.find(searchItem, searchFrom);
        return pos;
    }

    bool Project::HasStringInPath(const std::filesystem::path& path, const std::string& searchItem)
    {
        std::string searchePath = path.string();
        size_t size = searchePath.size();
        size_t pos = SearchInPathFor(searchePath, searchItem);
        bool hasMarker = size < pos;
        return hasMarker;
    }

    size_t Project::PositionMarker(const std::filesystem::path& path, const std::string& marker)
    {
        std::string searchePath = path.string();
        size_t pos = PositionMarker(searchePath, marker);
        return pos;
    }

    size_t Project::PositionMarker(const std::filesystem::path& path, const std::wstring& marker)
    {
        std::wstring searchePath = path.wstring();
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

    size_t Project::PositionMarker(const std::wstring& pathWstr, const std::string& marker)
    {
        constexpr const char directoryDiffern = '/';
        constexpr size_t searchFrom = 0ull;
        std::string searcheMarkerStr = marker + '/';
        std::wstring searcheMarkerWstr(searcheMarkerStr.begin(), searcheMarkerStr.end());

        size_t pos = SearchInPathFor(pathWstr, searcheMarkerWstr);
        return pos;
    }

    bool Project::HasMarker(const std::filesystem::path& path, const std::string& marker)
    {
        std::string searchePath = path.string();
        size_t size = searchePath.size();
        size_t pos = PositionMarker(searchePath, marker);
        bool hasMarker = pos < size;
        return hasMarker;
    }

    bool Project::HasSomeMarker(const std::filesystem::path& path)
    {
        constexpr const char* markerSymbols = "#!#";
        constexpr size_t markerSymbolsSize = 2;
        constexpr size_t expextedCount = 1;

        std::string searchePath = path.string();
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

        return hasMarker;
    }

    std::filesystem::path Project::RemoveMarker(const std::filesystem::path& path, const std::string& marker)
    {
        constexpr size_t offset = 1;
        std::string searchePath = path.string();
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

    std::filesystem::path Project::RemoveMarkerW(const std::filesystem::path& path, const std::string& marker)
    {
        constexpr size_t offset = 1;
        std::wstring searchePath = path.wstring();
        size_t size = searchePath.size();
        size_t pos = PositionMarker(searchePath, marker);
        size_t sizeMarker = marker.size();
        size_t endPosMarker = sizeMarker + pos + offset;
        std::wstring pathWithoutMarker = searchePath.substr(endPosMarker);
        return std::filesystem::path(pathWithoutMarker);
    }

    std::string Project::ExtraxtMarker(const std::filesystem::path& path)
    {
        constexpr size_t offset = 0ull;
        constexpr uint32_t markerCount = 2;
        constexpr const char* markersArray[markerCount] = {
            RY_PATH_PROJECT_MARKER_STR,
            RY_PATH_ENGINE_MARKER_STR
        };
        std::string searchePath = path.string();
        size_t size = searchePath.size();
        size_t pos = MAXSIZE_T;
        uint32_t i = 0;
        do {
            pos = PositionMarker(searchePath, markersArray[i]);
            i++;
        } while (i < markerCount && size <= pos);
        if (size <= pos)
        {
            RY_CORE_ERROR("No Marker Found To Extraxt in Path: {}", path);
            return "";
        }
        size_t markerIndex = i - 1;
#if 0
        size_t sizeMarker = std::strlen(markersArray[markerIndex]);
        size_t startPosMarker = 0ull;
        size_t endPosMarker = pos + sizeMarker - offset;

        std::string markerInside = searchePath.substr(startPosMarker, endPosMarker);
#else
        std::string markerInside = markersArray[markerIndex];
#endif
        return markerInside;
    }

    std::wstring Project::ExtraxtMarkerW(const std::filesystem::path& path)
    {
        constexpr size_t offset = 0ull;
        constexpr uint32_t markerCount = 2;
        constexpr const wchar_t* markersArray[markerCount] = {
            RY_PATH_PROJECT_MARKER_WSTR,
            RY_PATH_ENGINE_MARKER_WSTR
        };
        std::wstring searchePath = path.wstring();
        size_t size = searchePath.size();
        size_t pos = MAXSIZE_T;
        uint32_t i = 0;
        do {
            pos = PositionMarker(searchePath, markersArray[i]);
            i++;
        } while (i < markerCount && size <= pos);
        if (size <= pos)
        {
            RY_CORE_ERROR("No Marker Found To Extraxt in Path: {}", path);
            return L"";
        }
        size_t markerIndex = i - 1;
#if 0
        size_t sizeMarker = std::strlen(markersArray[markerIndex]);
        size_t startPosMarker = 0ull;
        size_t endPosMarker = pos + sizeMarker - offset;

        std::string markerInside = searchePath.substr(startPosMarker, endPosMarker);
#else
        std::wstring markerInside = markersArray[markerIndex];
#endif
        return markerInside;
    }

    std::filesystem::path Project::ReplaceMarkerWitheAbsolutePath(const std::filesystem::path& path)
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

        if (!HasSomeMarker(path))
        {
            RY_CORE_ERROR("File Path has no Marker! {}", path);
            return std::filesystem::path("");
        }
        std::string marker = ExtraxtMarker(path);
        if (!IsMarkerVaild(marker))
        {
            RY_CORE_ERROR("Did't found a vaild Marker! {}", marker);
            return std::filesystem::path("");
        }

        std::filesystem::path base = GetAbsulteFilePathFormMarker(marker);
        if (base == "")
        {
            RY_CORE_ERROR("Has No Valid Path Base! {}", base);
            return std::filesystem::path("");
        }


        std::filesystem::path absultePath = ReplaceMarkerWithePath(path, marker, base);
        return absultePath;


    }

    std::filesystem::path Project::ReplaceMarkerWithePath(const std::filesystem::path& path, const std::string& marker, const std::filesystem::path& base)
    {
        std::filesystem::path withOutMarker = RemoveMarker(path, marker);
        std::filesystem::path absultePath = base / withOutMarker;
        std::string absultePathStr = absultePath.generic_string();
        absultePath = absultePathStr;
        return absultePath;
    }

    std::filesystem::path Project::ReplaceMarkerWitheAbsolutePathW(const std::filesystem::path& path)
    {
        constexpr uint32_t markerCount = 2;
        constexpr const wchar_t* markersArray[markerCount] = {
            RY_PATH_PROJECT_MARKER_WSTR,
            RY_PATH_ENGINE_MARKER_WSTR
        };
        std::filesystem::path markersPathsArray[markerCount] = {
            Project::GetActiveProjectDirectory(),
            std::filesystem::current_path()
        };

        if (!HasSomeMarker(path))
        {
            RY_CORE_ERROR("File Path has no Marker! {}", path);
            return std::filesystem::path("");
        }
        std::string marker = ExtraxtMarker(path);
        if (!IsMarkerVaild(marker))
        {
            RY_CORE_ERROR("Did't found a vaild Marker! {}", marker);
            return std::filesystem::path("");
        }

        std::filesystem::path base = GetAbsulteFilePathFormMarker(marker);
        if (base == "")
        {
            RY_CORE_ERROR("Has No Valid Path Base! {}", base);
            return std::filesystem::path("");
        }


        std::filesystem::path withOutMarker = RemoveMarkerW(path, marker);
        std::filesystem::path absultePath = base / withOutMarker;
        std::wstring absultePathWstr = absultePath.generic_wstring();
        absultePath = absultePathWstr;
        return absultePath;
    }

    uint32_t Project::GetMarkerVaild(const std::string& marker)
    {
        constexpr uint32_t markerCount = 4;
        constexpr uint32_t markerVaildCount = 2;
        constexpr const char* markersArray[markerCount] = {
            RY_PATH_PROJECT_MARKER_STR,
            RY_PATH_ENGINE_MARKER_STR,
            RY_PATH_NO_VAILD_MARKER_STR,
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
            RY_PATH_NO_VAILD_MARKER_STR,
            ""
        };
        uint32_t i = 0;
        while (i < markerCount && marker != markersArray[i])
        {
            i++;
        }
        return i < markerVaildCount;
    }

    std::filesystem::path Project::GeanrateRealtivePathWitheMarker(const std::filesystem::path& path)
    {
        std::pair<std::string, std::filesystem::path> pair = GeanrateRealtivePathAndMarker(path);
        const std::string& marker = pair.first;
        const  std::filesystem::path& base = pair.second;
        if (!IsMarkerVaild(marker))
        {
            RY_CORE_ERROR("Did't found a vaild Marker! {}", marker);
            return std::filesystem::path("");
        }

        std::filesystem::path realtiveFromBase;
        if (path.is_absolute())
            realtiveFromBase = std::filesystem::relative(path, base);
        else
            realtiveFromBase = path;
        std::filesystem::path pathWitheMarker = SetMarker(realtiveFromBase, marker);

        std::string pathWitheMarkerStr = pathWitheMarker.generic_string();
        pathWitheMarker = pathWitheMarkerStr;
        return pathWitheMarker;
    }

    std::filesystem::path Project::GetAbsulteFilePathFormMarker(const std::string& marker)
    {
        constexpr uint32_t markerCount = 3;
        constexpr uint32_t markerVaildCount = 2;
        constexpr const char* markersArray[markerCount] = {
            RY_PATH_PROJECT_MARKER_STR,
            RY_PATH_ENGINE_MARKER_STR,
            RY_PATH_NO_VAILD_MARKER_STR
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
        if (path == std::filesystem::path(""))
        {
            RY_CORE_ERROR("Did't found a vaild Path! {}", path);
            return std::pair<std::string, std::filesystem::path>(RY_PATH_NO_VAILD_MARKER_STR, "");
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
        constexpr const char* engineRealtive = RY_PATH_EXPEXT_ENGINE_RELATIV_START_STR;

        std::filesystem::path projectPath = GetActiveProjectDirectory();
        std::filesystem::path appPath = std::filesystem::current_path();

        std::filesystem::path appPathEngine = appPath / engineRealtive;
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
#if 0
        RY_CORE_TRACE("We think that Path: {} is part of {}", path, marker);
#endif
        return marker;
    }

    bool Project::IsAbsultePathSubPath(const std::filesystem::path& pathAbsolut, const std::filesystem::path& baseAbsolut)
    {
        RY_CORE_ASSERT(pathAbsolut.is_absolute() && baseAbsolut.is_absolute(), "We expext no Realtive only Absulte Paths!");
        std::filesystem::path realitveTo = std::filesystem::relative(pathAbsolut, baseAbsolut);
        std::string realitveToStr = realitveTo.generic_string();
        bool areTheSame = !realitveToStr.empty() && realitveToStr.find("..") != 0;
        if (areTheSame && !Project::IsPathExisting(pathAbsolut))
            RY_CORE_WARN("The Path is Vaild but File is not Found {}", realitveToStr);
        return areTheSame;
    }
#if 0
#pragma region OldPathFuntion
    std::string Project::SetProjectMarker(const std::filesystem::path& path)
    {
        std::string projectMarker = RY_PATH_PROJECT_MARKER_STR "/";
        std::string pathStr = path.string();
        if (pathStr.find(projectMarker) < pathStr.size())
        {
            RY_CORE_WARN("This file Path has alrady a Marker! {}", pathStr);
            return pathStr;
        }
        else if (path.is_absolute())
        {
            const std::filesystem::path& projectAbsultPath = GetActiveProjectDirectory();
            std::filesystem::path filePathRaltive = std::filesystem::relative(path, projectAbsultPath);
            pathStr = projectMarker + filePathRaltive.string();
            return pathStr;
        }
        else
        {
            std::filesystem::path rootDir = std::filesystem::absolute(path);
            std::filesystem::path projectPath = GetActiveProjectDirectory();
            std::filesystem::path projectDirPathName = projectPath.filename();

            std::filesystem::path projectAssetPath = GetActiveAssetDirectory();
            std::filesystem::path projectAssetPathName = projectAssetPath.filename();

            if (projectDirPathName == rootDir)
            {
                pathStr = projectMarker + pathStr;
                return pathStr;
            }
            else if (projectAssetPathName == rootDir)
            {
                std::filesystem::path filePathRaltive = std::filesystem::relative(projectPath, projectAssetPath);
                std::string raeltive = filePathRaltive.string();
                pathStr = projectMarker + raeltive;
                return pathStr;
            }

            RY_CORE_FATAL("rootDir: {}\n\tprojectPath: {}\n\tprojectDirPathName: {}\n\tprojectAssetPath: {}\n\tprojectAssetPathName: {}",
                rootDir, projectPath, projectDirPathName, projectAssetPath, projectAssetPathName);


        }

        RY_CORE_FATAL("File Path: {}\n\t Path str curend Build: {}\n\t Marker: {}", path, pathStr, projectMarker);
        // RY_CORE_ASSERT(false, "Unexpexted Run");
        return "";

    }

    std::string Project::RemoveProjectMarker(const std::string& pathStr)
    {
        std::string projectMarker = RY_PATH_PROJECT_MARKER_STR "/";
        uint32_t index = pathStr.find(projectMarker);
        uint32_t count = pathStr.size();
        if (index < count)
        {
            std::string pathWithoutMarker = pathStr.substr(index + projectMarker.size());
            return pathWithoutMarker;
        }
        else
        {
            RY_CORE_WARN("No Prject-Marker Found in {}", pathStr);
            return pathStr;
        }

    }

    std::string Project::RemoveProjectMarker(const std::filesystem::path& path)
    {
        std::string pathStr = path.string();
        return RemoveProjectMarker(pathStr);
    }
#pragma endregion
#endif
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
       Project::SaveActive((pConfig.ProjectPath / (pConfig.Name + ".ryproj")));
#if RY_EDITOR_ASSETMANGER_THREADE
       Ref<EditorAssetManegerThreade>& editorAssetManager = s_ActiveInstancProject->GetEditorAssetManger();
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
           s_ActiveInstancProject->m_AssetManger = editorAssetManagerN;
#else
           Ref<EditorAssetManager> editorAssetManagerN = CreateRef<EditorAssetManager>();
           editorAssetManagerN->SerialzeAssetRegistry();
           s_ActiveProject->m_AssetManger= editorAssetManagerN;
#endif
       }
       return s_ActiveInstancProject;
    }

    Ref<Project> Project::CreatNewPorject(const std::filesystem::path& projectPath, const std::string& name)
    {
        
        Ref<Project> project = CreateRef<Project>();
        ProjectConfig& pConfig = s_ActiveInstancProject->m_Config;
        Utils::CreateProject(pConfig, projectPath, name);
        Project::SaveActive((pConfig.ProjectPath / (pConfig.Name + ".ryproj")));
#if RY_EDITOR_ASSETMANGER_THREADE
        Ref<EditorAssetManegerThreade>& editorAssetManager = s_ActiveInstancProject->GetEditorAssetManger();
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
            
#if RY_EDITOR_ASSETMANGER_THREADE
            Ref<EditorAssetManegerThreade> editorAssetManager = CreateRef<EditorAssetManegerThreade>();
            editorAssetManager->OnAttach();
            s_ActiveInstancProject->m_AssetManger = editorAssetManager;

            editorAssetManager->DeserialzeAssetRegistry();
           
#else
            Ref<EditorAssetManager> editorAssetManager = CreateRef<EditorAssetManager>();
            editorAssetManager->OnAttach();
            s_ActiveProject->m_AssetManger = editorAssetManager; 

            editorAssetManager->DeserialzeAssetRegistry();
#endif
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

