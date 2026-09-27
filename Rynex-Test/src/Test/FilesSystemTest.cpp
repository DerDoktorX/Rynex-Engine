
#include <rypch.h>
#include "FilesSystemTest.h"
#include <Rynex/Project/Project.h>

void FilesSystemTest::SetUp()
{
    m_Project = Rynex::Project::New();
    Rynex::ProjectConfig& config = m_Project->GetConfig();
    config.m_ProjectPath = "D:/dev/Test/Project/Rynex/GTest/";
}

void FilesSystemTest::TearDown()
{
    m_Project->Shutdown();
    RY_DESTROY_REF(m_Project);
}

// ===========================================================================
// EngineFilePathClassTestChangeOrigen
// ===========================================================================

// Block 1 — Project-marked input path; SetMarker(Engine) must re-origin it to Engine.
TEST_F(FilesSystemTest, EnginePathChangeOrigin_FromProjectMarker)
{
    constexpr Rynex::FileSystem::Path::Origin expectOutPutOrigin = Rynex::FileSystem::Path::Origin::Engine;
    constexpr Rynex::AssetType expectOutPutAssetType = Rynex::AssetType::Shader;

    constexpr const char* expectOutPutName      = "TextureTransform2.glsl";
    constexpr const char* checkPath = RY_PATH_PROJECT_MARKER_STR "/Engine-Resources/Editor-Assets/shaders/TextureTransform2.glsl";
    constexpr const char* expectOutPutRelative = "Engine-Resources/Editor-Assets/shaders/TextureTransform2.glsl";
    constexpr const char* expectOutPutMarked = RY_PATH_ENGINE_MARKER_STR "/Engine-Resources/Editor-Assets/shaders/TextureTransform2.glsl";

    const std::filesystem::path engineDir    = Rynex::FileSystem::Path::GetEngineDirectory();
    const std::string expectOutPutAbsolute   = (engineDir / expectOutPutRelative).lexically_normal().generic_string();

    Rynex::FileSystem::Path fileSystemPath(checkPath);
    fileSystemPath.SetMarker(expectOutPutOrigin);

    EXPECT_EQ(expectOutPutAbsolute,                 fileSystemPath.GetPathString());
    EXPECT_EQ(expectOutPutAbsolute,                 fileSystemPath.GetAbsolutePathString());
    EXPECT_EQ(std::string(expectOutPutRelative),    fileSystemPath.GetRelativePathString());
    EXPECT_EQ(std::string(expectOutPutMarked),      fileSystemPath.GetMarkedPathString());
    EXPECT_EQ(expectOutPutOrigin,                   fileSystemPath.GetOrigin());
    EXPECT_EQ(expectOutPutAssetType,                fileSystemPath.GetAssetFileType());
    EXPECT_EQ(expectOutPutName,                     fileSystemPath.GetNamePathString());
}

// Block 2 — bare relative path; SetMarker(Engine) promotes it to the Engine origin.
TEST_F(FilesSystemTest, EnginePathChangeOrigin_FromRelative)
{
    constexpr Rynex::FileSystem::Path::Origin expectOutPutOrigin = Rynex::FileSystem::Path::Origin::Engine;
    constexpr Rynex::AssetType expectOutPutAssetType = Rynex::AssetType::Texture2D;

    constexpr const char* expectOutPutName      = "ChernoLogo.png";
    constexpr const char* checkPath             = "Engine-Resources/Editor-Assets/Texture/ChernoLogo.png";
    constexpr const char* expectOutPutRelative  = "Engine-Resources/Editor-Assets/Texture/ChernoLogo.png";
    constexpr const char* expectOutPutMarked    =  RY_PATH_ENGINE_MARKER_STR "/Engine-Resources/Editor-Assets/Texture/ChernoLogo.png";

    const std::filesystem::path engineDir    = Rynex::FileSystem::Path::GetEngineDirectory();
    const std::string expectOutPutAbsolute   = (engineDir / expectOutPutRelative).lexically_normal().generic_string();

    Rynex::FileSystem::Path fileSystemPath(checkPath);
    fileSystemPath.SetMarker(expectOutPutOrigin);

    EXPECT_EQ(expectOutPutAbsolute,                 fileSystemPath.GetPathString());
    EXPECT_EQ(expectOutPutAbsolute,                 fileSystemPath.GetAbsolutePathString());
    EXPECT_EQ(std::string(expectOutPutRelative),    fileSystemPath.GetRelativePathString());
    EXPECT_EQ(std::string(expectOutPutMarked),      fileSystemPath.GetMarkedPathString());
    EXPECT_EQ(expectOutPutOrigin,                   fileSystemPath.GetOrigin());
    EXPECT_EQ(expectOutPutAssetType,                fileSystemPath.GetAssetFileType());
    EXPECT_EQ(expectOutPutName,                     fileSystemPath.GetNamePathString());
}

// Block 3 — absolute Engine path constructed with a Project hint; SetMarker(Engine) must correct it.
TEST_F(FilesSystemTest, EnginePathChangeOrigin_FromAbsoluteProjectHint)
{
    constexpr Rynex::FileSystem::Path::Origin expectOutPutOrigin = Rynex::FileSystem::Path::Origin::Engine;
    constexpr Rynex::AssetType expectOutPutAssetType = Rynex::AssetType::MeshStatic;

    constexpr const char* expectOutPutName      = "Cube.rystmesh";
    constexpr const char* expectOutPutRelative = "Engine-Resources/Editor-Assets/Assets/Models/Cube.rystmesh";
    constexpr const char* expectOutPutMarked = "Engine#!#/Engine-Resources/Editor-Assets/Assets/Models/Cube.rystmesh";

    const std::filesystem::path engineDir    = Rynex::FileSystem::Path::GetEngineDirectory();
    const std::string expectOutPutAbsolute   = (engineDir / expectOutPutRelative).lexically_normal().generic_string();

    Rynex::FileSystem::Path fileSystemPath(expectOutPutAbsolute, Rynex::FileSystem::Path::Origin::Project);
    fileSystemPath.SetMarker(expectOutPutOrigin);

    EXPECT_EQ(expectOutPutAbsolute,                 fileSystemPath.GetPathString());
    EXPECT_EQ(expectOutPutAbsolute,                 fileSystemPath.GetAbsolutePathString());
    EXPECT_EQ(std::string(expectOutPutRelative),    fileSystemPath.GetRelativePathString());
    EXPECT_EQ(std::string(expectOutPutMarked),      fileSystemPath.GetMarkedPathString());
    EXPECT_EQ(expectOutPutOrigin,                   fileSystemPath.GetOrigin());
    EXPECT_EQ(expectOutPutAssetType,                fileSystemPath.GetAssetFileType());
    EXPECT_EQ(expectOutPutName,                     fileSystemPath.GetNamePathString());
}

// ===========================================================================
// ProjectFilePathClassTestWindowsPathConvention
// ===========================================================================

// Block 1 — Project-marked path using Windows backslashes; must normalise to forward slashes.
TEST_F(FilesSystemTest, ProjectPathWindowsConvention_MarkedBackslash)
{
    constexpr Rynex::FileSystem::Path::Origin expectOutPutOrigin = Rynex::FileSystem::Path::Origin::Project;
    constexpr Rynex::AssetType expectOutPutAssetType = Rynex::AssetType::Shader;

    constexpr const char* expectOutPutName      = "Plane3DShadow.glsl";
    constexpr const char* checkPath             = "Project#!#\\Assets\\Shaders\\Fetures\\Plane3DShadow.glsl";
    constexpr const char* expectOutPutRelative  = "Assets/Shaders/Fetures/Plane3DShadow.glsl";
    constexpr const char* expectOutPutMarked    = "Project#!#/Assets/Shaders/Fetures/Plane3DShadow.glsl";

    const std::filesystem::path projectDir   = Rynex::FileSystem::Path::GetProjectDirectory();
    const std::string expectOutPutAbsolute   = (projectDir / expectOutPutRelative).lexically_normal().generic_string();

    Rynex::FileSystem::Path fileSystemPath(checkPath);

    EXPECT_EQ(expectOutPutAbsolute,                 fileSystemPath.GetPathString());
    EXPECT_EQ(expectOutPutAbsolute,                 fileSystemPath.GetAbsolutePathString());
    EXPECT_EQ(std::string(expectOutPutRelative),    fileSystemPath.GetRelativePathString());
    EXPECT_EQ(std::string(expectOutPutMarked),      fileSystemPath.GetMarkedPathString());
    EXPECT_EQ(expectOutPutOrigin,                   fileSystemPath.GetOrigin());
    EXPECT_EQ(expectOutPutAssetType,                fileSystemPath.GetAssetFileType());
    EXPECT_EQ(expectOutPutName,                     fileSystemPath.GetNamePathString());
}

// Block 2 — relative path with backslashes only; must resolve to the Project directory.
TEST_F(FilesSystemTest, ProjectPathWindowsConvention_RelativeBackslash)
{
    constexpr Rynex::FileSystem::Path::Origin expectOutPutOrigin = Rynex::FileSystem::Path::Origin::Project;
    constexpr Rynex::AssetType expectOutPutAssetType = Rynex::AssetType::MeshStatic;

    constexpr const char* expectOutPutName      = "scene.rystmesh";
    constexpr const char* checkPath             = "Assets\\Models\\CV-Model\\scene.rystmesh";
    constexpr const char* expectOutPutRelative  = "Assets/Models/CV-Model/scene.rystmesh";
    constexpr const char* expectOutPutMarked    = "Project#!#/Assets/Models/CV-Model/scene.rystmesh";

    const std::filesystem::path projectDir   = Rynex::FileSystem::Path::GetProjectDirectory();
    const std::string expectOutPutAbsolute   = (projectDir / expectOutPutRelative).lexically_normal().generic_string();

    Rynex::FileSystem::Path fileSystemPath(checkPath);

    EXPECT_EQ(expectOutPutAbsolute,              fileSystemPath.GetPathString());
    EXPECT_EQ(expectOutPutAbsolute,              fileSystemPath.GetAbsolutePathString());
    EXPECT_EQ(std::string(expectOutPutRelative),  fileSystemPath.GetRelativePathString());
    EXPECT_EQ(std::string(expectOutPutMarked),   fileSystemPath.GetMarkedPathString());
    EXPECT_EQ(expectOutPutOrigin,                fileSystemPath.GetOrigin());
    EXPECT_EQ(expectOutPutAssetType,                fileSystemPath.GetAssetFileType());
    EXPECT_EQ(expectOutPutName,                     fileSystemPath.GetNamePathString());
}

// Block 3 — absolute Windows path (backslash + drive letter); relative stem must be preserved.
TEST_F(FilesSystemTest, ProjectPathWindowsConvention_AbsoluteBackslash)
{
    constexpr Rynex::FileSystem::Path::Origin expectOutPutOrigin = Rynex::FileSystem::Path::Origin::Project;
    constexpr Rynex::AssetType expectOutPutAssetType = Rynex::AssetType::Scene;

    constexpr const char* expectOutPutName =        "Cube2-Test-LigthShader.rynexscene";
    constexpr const char* expectOutPutRelative = "Assets/Scene/Cube2-Test-LigthShader.rynexscene";
    constexpr const char* expectOutPutMarked = "Project#!#/Assets/Scene/Cube2-Test-LigthShader.rynexscene";

    const std::filesystem::path projectDir   = Rynex::FileSystem::Path::GetProjectDirectory();
    const  std::filesystem::path checkPath = std::filesystem::path(projectDir / expectOutPutRelative);
    const std::string expectOutPutAbsolute   = checkPath.lexically_normal().generic_string();

    Rynex::FileSystem::Path fileSystemPath(checkPath);

    EXPECT_EQ(expectOutPutAbsolute,                 fileSystemPath.GetPathString());
    EXPECT_EQ(expectOutPutAbsolute,                 fileSystemPath.GetAbsolutePathString());
    EXPECT_EQ(std::string(expectOutPutRelative),    fileSystemPath.GetRelativePathString());
    EXPECT_EQ(std::string(expectOutPutMarked),      fileSystemPath.GetMarkedPathString());
    EXPECT_EQ(expectOutPutOrigin,                   fileSystemPath.GetOrigin());
    EXPECT_EQ(expectOutPutAssetType,                fileSystemPath.GetAssetFileType());
    EXPECT_EQ(expectOutPutName,                     fileSystemPath.GetNamePathString());
}

// ===========================================================================
// EngineFilePathClassTestWhrongOrgnieInput
// ===========================================================================

// Block 1 — Engine-marked path given with a Project origin hint; embedded marker must win.
TEST_F(FilesSystemTest, EnginePathWrongOriginHint_MarkedInput)
{
    constexpr Rynex::FileSystem::Path::Origin expectOutPutOrigin = Rynex::FileSystem::Path::Origin::Engine;
    constexpr Rynex::AssetType expectOutPutAssetType = Rynex::AssetType::Shader;

    constexpr const char* expectOutPutName =        "TextureTransform2.glsl";
    constexpr const char* checkPath =               "Engine#!#/Engine-Resources/Editor-Assets/shaders/TextureTransform2.glsl";
    constexpr const char* expectOutPutRelative =    "Engine-Resources/Editor-Assets/shaders/TextureTransform2.glsl";
    constexpr const char* expectOutPutMarked =      "Engine#!#/Engine-Resources/Editor-Assets/shaders/TextureTransform2.glsl";

    const std::filesystem::path engineDir    = Rynex::FileSystem::Path::GetEngineDirectory();
    const std::string expectOutPutAbsolute   = (engineDir / expectOutPutRelative).lexically_normal().generic_string();

    Rynex::FileSystem::Path fileSystemPath(checkPath, Rynex::FileSystem::Path::Origin::Project);

    EXPECT_EQ(expectOutPutAbsolute,                 fileSystemPath.GetPathString());
    EXPECT_EQ(expectOutPutAbsolute,                 fileSystemPath.GetAbsolutePathString());
    EXPECT_EQ(std::string(expectOutPutRelative),    fileSystemPath.GetRelativePathString());
    EXPECT_EQ(std::string(expectOutPutMarked),      fileSystemPath.GetMarkedPathString());
    EXPECT_EQ(expectOutPutOrigin,                   fileSystemPath.GetOrigin());
    EXPECT_EQ(expectOutPutAssetType,                fileSystemPath.GetAssetFileType());
    EXPECT_EQ(expectOutPutName,                     fileSystemPath.GetNamePathString());
}

// Block 2 — ambiguous relative path with an explicit Project hint; path stays in the Project tree.
TEST_F(FilesSystemTest, EnginePathWrongOriginHint_RelativeProjectHint)
{
    constexpr Rynex::FileSystem::Path::Origin expectOutPutOrigin = Rynex::FileSystem::Path::Origin::Project;
    constexpr Rynex::AssetType expectOutPutAssetType = Rynex::AssetType::Texture2D;

    constexpr const char* expectOutPutName = "ChernoLogo.png";
    constexpr const char* checkPath        = "Engine-Resources/Editor-Assets/Texture/ChernoLogo.png";
    constexpr const char* expectOutPutRelative = "Engine-Resources/Editor-Assets/Texture/ChernoLogo.png";
    constexpr const char* expectOutPutMarked = RY_PATH_PROJECT_MARKER_STR "/Engine-Resources/Editor-Assets/Texture/ChernoLogo.png";

    const std::filesystem::path projectDir   = Rynex::FileSystem::Path::GetProjectDirectory();
    const std::string expectOutPutAbsolute   = (projectDir / expectOutPutRelative).lexically_normal().generic_string();

    Rynex::FileSystem::Path fileSystemPath(checkPath, Rynex::FileSystem::Path::Origin::Project);

    EXPECT_EQ(expectOutPutAbsolute,                 fileSystemPath.GetPathString());
    EXPECT_EQ(expectOutPutAbsolute,                 fileSystemPath.GetAbsolutePathString());
    EXPECT_EQ(std::string(expectOutPutRelative),    fileSystemPath.GetRelativePathString());
    EXPECT_EQ(std::string(expectOutPutMarked),      fileSystemPath.GetMarkedPathString());
    EXPECT_EQ(expectOutPutOrigin,                   fileSystemPath.GetOrigin());
    EXPECT_EQ(expectOutPutAssetType,                fileSystemPath.GetAssetFileType());
    EXPECT_EQ(expectOutPutName,                     fileSystemPath.GetNamePathString());
}

// Block 3 — absolute Engine path with a Project origin hint; auto-detection must correct origin.
TEST_F(FilesSystemTest, EnginePathWrongOriginHint_AbsoluteEnginePathProjectHint)
{

    constexpr Rynex::FileSystem::Path::Origin expectOutPutOrigin = Rynex::FileSystem::Path::Origin::Engine;
    constexpr Rynex::AssetType expectOutPutAssetType = Rynex::AssetType::MeshStatic;

    constexpr const char* expectOutPutName = "Cube.rystmesh";
    constexpr const char* expectOutPutRelative = "Engine-Resources/Editor-Assets/Assets/Models/Cube.rystmesh";
    constexpr const char* expectOutPutMarked = "Engine#!#/Engine-Resources/Editor-Assets/Assets/Models/Cube.rystmesh";

    const std::filesystem::path engineDir    = Rynex::FileSystem::Path::GetEngineDirectory();
    const std::string expectOutPutAbsolute   = (engineDir / expectOutPutRelative).lexically_normal().generic_string();

    Rynex::FileSystem::Path fileSystemPath(expectOutPutAbsolute, Rynex::FileSystem::Path::Origin::Project);

    EXPECT_EQ(expectOutPutAbsolute,                 fileSystemPath.GetPathString());
    EXPECT_EQ(expectOutPutAbsolute,                 fileSystemPath.GetAbsolutePathString());
    EXPECT_EQ(std::string(expectOutPutRelative),    fileSystemPath.GetRelativePathString());
    EXPECT_EQ(std::string(expectOutPutMarked),      fileSystemPath.GetMarkedPathString());
    EXPECT_EQ(expectOutPutOrigin,                   fileSystemPath.GetOrigin());
    EXPECT_EQ(expectOutPutAssetType,                fileSystemPath.GetAssetFileType());
    EXPECT_EQ(expectOutPutName,                     fileSystemPath.GetNamePathString());
}

// ===========================================================================
// EngineFilePathClassTest
// ===========================================================================

// Block 1 — standard Engine-marked path; all properties must resolve against the Engine root.
TEST_F(FilesSystemTest, EnginePathStandard_MarkedInput)
{
    constexpr Rynex::FileSystem::Path::Origin expectOutPutOrigin = Rynex::FileSystem::Path::Origin::Engine;
    constexpr Rynex::AssetType expectOutPutAssetType = Rynex::AssetType::Shader;

    constexpr const char* expectOutPutName = "TextureTransform2.glsl";
    constexpr const char* checkPath = "Engine#!#/Engine-Resources/Editor-Assets/shaders/TextureTransform2.glsl";
    constexpr const char* expectOutPutRelative =  "Engine-Resources/Editor-Assets/shaders/TextureTransform2.glsl";
    constexpr const char* expectOutPutMarked = "Engine#!#/Engine-Resources/Editor-Assets/shaders/TextureTransform2.glsl";

    const std::filesystem::path engineDir    = Rynex::FileSystem::Path::GetEngineDirectory();
    const std::filesystem::path joined       = engineDir / expectOutPutRelative;
    const std::string expectOutPutAbsolute   = std::filesystem::absolute(joined).generic_string();

    Rynex::FileSystem::Path fileSystemPath(checkPath);

    EXPECT_EQ(expectOutPutAbsolute,                 fileSystemPath.GetPathString());
    EXPECT_EQ(expectOutPutAbsolute,                 fileSystemPath.GetAbsolutePathString());
    EXPECT_EQ(std::string(expectOutPutRelative),    fileSystemPath.GetRelativePathString());
    EXPECT_EQ(std::string(expectOutPutMarked),      fileSystemPath.GetMarkedPathString());
    EXPECT_EQ(expectOutPutOrigin,                   fileSystemPath.GetOrigin());
    EXPECT_EQ(expectOutPutAssetType,                fileSystemPath.GetAssetFileType());
    EXPECT_EQ(expectOutPutName,                     fileSystemPath.GetNamePathString());
}

// Block 2 — relative path whose prefix matches the Engine resource tree.
TEST_F(FilesSystemTest, EnginePathStandard_RelativeInput)
{
    constexpr Rynex::FileSystem::Path::Origin expectOutPutOrigin = Rynex::FileSystem::Path::Origin::Engine;
    constexpr Rynex::AssetType expectOutPutAssetType = Rynex::AssetType::Texture2D;
    constexpr const char* expectOutPutName = "ChernoLogo.png";
    constexpr const char* checkPath        = "Engine-Resources/Editor-Assets/Texture/ChernoLogo.png";
    constexpr const char* expectOutPutRelative = "Engine-Resources/Editor-Assets/Texture/ChernoLogo.png";
    constexpr const char* expectOutPutMarked = "Engine#!#/Engine-Resources/Editor-Assets/Texture/ChernoLogo.png";

    const std::filesystem::path engineDir    = Rynex::FileSystem::Path::GetEngineDirectory();
    const std::string expectOutPutAbsolute   = (engineDir / expectOutPutRelative).lexically_normal().generic_string();

    Rynex::FileSystem::Path fileSystemPath(checkPath);

    EXPECT_EQ(expectOutPutAbsolute,                 fileSystemPath.GetPathString());
    EXPECT_EQ(expectOutPutAbsolute,                 fileSystemPath.GetAbsolutePathString());
    EXPECT_EQ(std::string(expectOutPutRelative),    fileSystemPath.GetRelativePathString());
    EXPECT_EQ(std::string(expectOutPutMarked),      fileSystemPath.GetMarkedPathString());
    EXPECT_EQ(expectOutPutOrigin,                   fileSystemPath.GetOrigin());
    EXPECT_EQ(expectOutPutAssetType,                fileSystemPath.GetAssetFileType());
    EXPECT_EQ(expectOutPutName,                     fileSystemPath.GetNamePathString());
}

// Block 3 — absolute Engine path without a hint; auto-detection must classify it as Engine.
TEST_F(FilesSystemTest, EnginePathStandard_AbsoluteInput)
{
    constexpr Rynex::FileSystem::Path::Origin expectOutPutOrigin = Rynex::FileSystem::Path::Origin::Engine;
    constexpr Rynex::AssetType expectOutPutAssetType = Rynex::AssetType::MeshStatic;
    constexpr const char* expectOutPutName = "Cube.rystmesh";
    constexpr const char* expectOutPutRelative = "Engine-Resources/Editor-Assets/Assets/Models/Cube.rystmesh";

    constexpr const char* expectOutPutMarked = "Engine#!#/Engine-Resources/Editor-Assets/Assets/Models/Cube.rystmesh";

    const std::filesystem::path engineDir    = Rynex::FileSystem::Path::GetEngineDirectory();
    const std::string expectOutPutAbsolute   = (engineDir / expectOutPutRelative).lexically_normal().generic_string();

    Rynex::FileSystem::Path fileSystemPath(expectOutPutAbsolute);

    EXPECT_EQ(expectOutPutAbsolute,                 fileSystemPath.GetPathString());
    EXPECT_EQ(expectOutPutAbsolute,                 fileSystemPath.GetAbsolutePathString());
    EXPECT_EQ(std::string(expectOutPutRelative),    fileSystemPath.GetRelativePathString());
    EXPECT_EQ(std::string(expectOutPutMarked),      fileSystemPath.GetMarkedPathString());
    EXPECT_EQ(expectOutPutOrigin,                   fileSystemPath.GetOrigin());
    EXPECT_EQ(expectOutPutAssetType,                fileSystemPath.GetAssetFileType());
    EXPECT_EQ(expectOutPutName,                     fileSystemPath.GetNamePathString());
}

// ===========================================================================
// ProjectFilePathClassTest
// ===========================================================================

// Block 1 — Project-marked path; must resolve relative to the Project directory.
TEST_F(FilesSystemTest, ProjectPathStandard_MarkedInput)
{


    constexpr Rynex::FileSystem::Path::Origin expectOutPutOrigin = Rynex::FileSystem::Path::Origin::Project;
    constexpr Rynex::AssetType expectOutPutAssetType = Rynex::AssetType::Shader;
    constexpr const char* expectOutPutName = "Plane3DShadow.glsl";
    constexpr const char* checkPath           = "Project#!#/Assets/Shaders/Fetures/Plane3DShadow.glsl";
    constexpr const char* expectOutPutRelative = "Assets/Shaders/Fetures/Plane3DShadow.glsl";
    constexpr const char* expectOutPutMarked  = "Project#!#/Assets/Shaders/Fetures/Plane3DShadow.glsl";

    const std::filesystem::path projectDir   = Rynex::FileSystem::Path::GetProjectDirectory();
    const std::string expectOutPutAbsolute   = (projectDir / expectOutPutRelative).lexically_normal().generic_string();

    Rynex::FileSystem::Path fileSystemPath(checkPath);

    EXPECT_EQ(expectOutPutAbsolute,                 fileSystemPath.GetPathString());
    EXPECT_EQ(expectOutPutAbsolute,                 fileSystemPath.GetAbsolutePathString());
    EXPECT_EQ(std::string(expectOutPutRelative),    fileSystemPath.GetRelativePathString());
    EXPECT_EQ(std::string(expectOutPutMarked),      fileSystemPath.GetMarkedPathString());
    EXPECT_EQ(expectOutPutOrigin,                   fileSystemPath.GetOrigin());
    EXPECT_EQ(expectOutPutAssetType,                fileSystemPath.GetAssetFileType());
    EXPECT_EQ(expectOutPutName,                     fileSystemPath.GetNamePathString());
}

// Block 2 — bare relative path; auto-detection should pick Project as the origin.
TEST_F(FilesSystemTest, ProjectPathStandard_RelativeInput)
{
    constexpr Rynex::FileSystem::Path::Origin expectOutPutOrigin = Rynex::FileSystem::Path::Origin::Project;
    constexpr Rynex::AssetType expectOutPutAssetType = Rynex::AssetType::MeshStatic;

    constexpr const char* expectOutPutName = "scene.rystmesh";
    constexpr const char* expectOutPutRelative = "Assets/Models/CV-Model/scene.rystmesh";
    constexpr const char* expectOutPutMarked  = "Project#!#/Assets/Models/CV-Model/scene.rystmesh";

    const std::filesystem::path projectDir   = Rynex::FileSystem::Path::GetProjectDirectory();
    const std::string checkPath   = (projectDir / expectOutPutRelative).lexically_normal().generic_string();
    const std::string expectOutPutAbsolute   = checkPath;

    Rynex::FileSystem::Path fileSystemPath(checkPath);

    EXPECT_EQ(expectOutPutAbsolute,                 fileSystemPath.GetPathString());
    EXPECT_EQ(expectOutPutAbsolute,                 fileSystemPath.GetAbsolutePathString());
    EXPECT_EQ(std::string(expectOutPutRelative),    fileSystemPath.GetRelativePathString());
    EXPECT_EQ(std::string(expectOutPutMarked),      fileSystemPath.GetMarkedPathString());
    EXPECT_EQ(expectOutPutOrigin,                   fileSystemPath.GetOrigin());
    EXPECT_EQ(expectOutPutAssetType,                fileSystemPath.GetAssetFileType());
    EXPECT_EQ(expectOutPutName,                     fileSystemPath.GetNamePathString());
}

// Block 3 — absolute path from a different project root; relative stem must be re-rooted.
TEST_F(FilesSystemTest, ProjectPathStandard_AbsoluteInput)
{
    constexpr Rynex::FileSystem::Path::Origin expectOutPutOrigin = Rynex::FileSystem::Path::Origin::Project;
    constexpr Rynex::AssetType expectOutPutAssetType = Rynex::AssetType::Scene;

    constexpr const char* expectOutPutName = "Cube2-Test-LigthShader.rynexscene";
    constexpr const char* expectOutPutRelative = "Assets/Scene/Cube2-Test-LigthShader.rynexscene";
    constexpr const char* expectOutPutMarked = "Project#!#/Assets/Scene/Cube2-Test-LigthShader.rynexscene";

    const std::filesystem::path projectDir   = Rynex::FileSystem::Path::GetProjectDirectory();
    const std::string expectOutPutAbsolute   = (projectDir / expectOutPutRelative).lexically_normal().generic_string();

    Rynex::FileSystem::Path fileSystemPath(expectOutPutAbsolute);

    EXPECT_EQ(expectOutPutAbsolute,                 fileSystemPath.GetPathString());
    EXPECT_EQ(expectOutPutAbsolute,                 fileSystemPath.GetAbsolutePathString());
    EXPECT_EQ(std::string(expectOutPutRelative),    fileSystemPath.GetRelativePathString());
    EXPECT_EQ(std::string(expectOutPutMarked),      fileSystemPath.GetMarkedPathString());
    EXPECT_EQ(expectOutPutOrigin,                   fileSystemPath.GetOrigin());
    EXPECT_EQ(expectOutPutAssetType,                fileSystemPath.GetAssetFileType());
    EXPECT_EQ(expectOutPutName,                     fileSystemPath.GetNamePathString());
}

// ===========================================================================
// UnknownFilePathClassTest
// ===========================================================================

// Block 1 — relative-dot-dot path that escapes all known roots; origin must be Unknown.
TEST_F(FilesSystemTest, UnknownPath_RelativeDotDot)
{
    constexpr Rynex::FileSystem::Path::Origin expectOutPutOrigin = Rynex::FileSystem::Path::Origin::Unknown;
    constexpr Rynex::AssetType expectOutPutAssetType = Rynex::AssetType::TextFile;

    constexpr const char* expectOutPutName = "Notiz.txt";
    constexpr const char* checkPath            = "../../../Dokument/Notiz.txt";
    constexpr const char* expectOutPutAbsolute = "/../../../Dokument/Notiz.txt";
    constexpr const char* expectOutPutRelative  = "/../../../Dokument/Notiz.txt";
    constexpr const char* expectOutPutMarked   = "NotVaild!#/../../../Dokument/Notiz.txt";

    Rynex::FileSystem::Path fileSystemPath(checkPath);

    EXPECT_EQ(std::string(expectOutPutAbsolute), fileSystemPath.GetPathString());
    EXPECT_EQ(std::string(expectOutPutAbsolute), fileSystemPath.GetAbsolutePathString());
    EXPECT_EQ(std::string(expectOutPutRelative),  fileSystemPath.GetRelativePathString());
    EXPECT_EQ(std::string(expectOutPutMarked),   fileSystemPath.GetMarkedPathString());
    EXPECT_EQ(expectOutPutAssetType,                fileSystemPath.GetAssetFileType());
    EXPECT_EQ(expectOutPutName,                     fileSystemPath.GetNamePathString());
}

// Block 2 — already-invalid-marked path; marker must be preserved, origin stays Unknown.
TEST_F(FilesSystemTest, UnknownPath_InvalidMarker)
{
    constexpr Rynex::FileSystem::Path::Origin expectOutPutOrigin = Rynex::FileSystem::Path::Origin::Unknown;
    constexpr Rynex::AssetType expectOutPutAssetType = Rynex::AssetType::Texture2D;

    constexpr const char* expectOutPutName = "Bird.png";
    constexpr const char* checkPath            = "NotVaild!#/../../Pictures/Bird.png";
    constexpr const char* expectOutPutAbsolute = "/../../Pictures/Bird.png";
    constexpr const char* expectOutPutRelative  = "/../../Pictures/Bird.png";
    constexpr const char* expectOutPutMarked   = "NotVaild!#/../../Pictures/Bird.png";

    Rynex::FileSystem::Path fileSystemPath(checkPath);

    EXPECT_EQ(std::string(expectOutPutAbsolute), fileSystemPath.GetPathString());
    EXPECT_EQ(std::string(expectOutPutAbsolute), fileSystemPath.GetAbsolutePathString());
    EXPECT_EQ(std::string(expectOutPutRelative),  fileSystemPath.GetRelativePathString());
    EXPECT_EQ(std::string(expectOutPutMarked),   fileSystemPath.GetMarkedPathString());
    EXPECT_EQ(expectOutPutOrigin,                fileSystemPath.GetOrigin());
    EXPECT_EQ(expectOutPutAssetType,                fileSystemPath.GetAssetFileType());
    EXPECT_EQ(expectOutPutName,                     fileSystemPath.GetNamePathString());
}

// Block 3 — absolute Windows path outside both Engine and Project roots; must be Unknown.
TEST_F(FilesSystemTest, UnknownPath_AbsoluteWindowsDrive)
{
    constexpr Rynex::FileSystem::Path::Origin expectOutPutOrigin = Rynex::FileSystem::Path::Origin::Unknown;
    constexpr Rynex::AssetType expectOutPutAssetType = Rynex::AssetType::Texture2D;

    constexpr const char* expectOutPutName = "CheckeBord.png";
    constexpr const char* checkPath            = "C:/Users/Public/Pictures/CheckeBord.png";
    constexpr const char* expectOutPutAbsolute = "C:/Users/Public/Pictures/CheckeBord.png";
    constexpr const char* expectOutPutRelative  = "C:/Users/Public/Pictures/CheckeBord.png";
    constexpr const char* expectOutPutMarked   = "NotVaild!#/C:/Users/Public/Pictures/CheckeBord.png";

    Rynex::FileSystem::Path fileSystemPath(checkPath);

    EXPECT_EQ(std::string(expectOutPutAbsolute), fileSystemPath.GetPathString());
    EXPECT_EQ(std::string(expectOutPutAbsolute), fileSystemPath.GetAbsolutePathString());
    EXPECT_EQ(std::string(expectOutPutRelative),  fileSystemPath.GetRelativePathString());
    EXPECT_EQ(std::string(expectOutPutMarked),   fileSystemPath.GetMarkedPathString());
    EXPECT_EQ(expectOutPutOrigin,                fileSystemPath.GetOrigin());
    EXPECT_EQ(expectOutPutAssetType,                fileSystemPath.GetAssetFileType());
    EXPECT_EQ(expectOutPutName,                     fileSystemPath.GetNamePathString());
}