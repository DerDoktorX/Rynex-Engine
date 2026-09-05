#include "rypch.h"
#include "EditorAssetMangerSerialzation.h"
#include "YAML.h"

#include <Rynex/Asset/EditorAssetManager.h>
#include <Rynex/Renderer/API/Buffer.h>
#include <Rynex/Project/Project.h>

#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>
#include <fstream>




namespace Rynex {

	namespace Test {
		
		static void EngineFilePathClassTestChangeOrigne()
		{
			constexpr const FileSystem::Path::Origin expextOutPutOrigne = FileSystem::Path::Origin::Engine;
			{
				constexpr const char* checkPath = RY_PATH_PROJECT_MARKER_STR "/../Rynex-Editor/Editor-Assets/shaders/TextureTransform2.glsl";
				constexpr const char* expextOutPutRealtiv = "../Rynex-Editor/Editor-Assets/shaders/TextureTransform2.glsl";
				const std::string expextOutPutAbsoulte = (FileSystem::Path::GetEngineDirectory() / expextOutPutRealtiv).lexically_normal().generic_string();
				constexpr const char* expextOutPutMarked = RY_PATH_ENGINE_MARKER_STR "/../Rynex-Editor/Editor-Assets/shaders/TextureTransform2.glsl";

				RY_CORE_TRACE("Check now: {1} {0}", checkPath, magic_enum::enum_name(expextOutPutOrigne).data());
				FileSystem::Path fileSystemPath(checkPath);
				fileSystemPath.SetMarker(expextOutPutOrigne);
				std::string pathStr = fileSystemPath.GetPath().string();
				std::string pathAbsoulteStr = fileSystemPath.GetAbsolutePath().string();
				std::string pathRelativeStr = fileSystemPath.GetRelativePath().string();
				std::string pathMarkedStr = fileSystemPath.GetMarkedPathString();

				bool resultPath = pathStr == expextOutPutAbsoulte;
				bool resultAbsoulte = pathAbsoulteStr == expextOutPutAbsoulte;
				bool resultRealtiv = pathRelativeStr == expextOutPutRealtiv;
				bool resultMarked = pathMarkedStr == expextOutPutMarked;

				FileSystem::Path::Origin origne = fileSystemPath.GetOrigin();
				bool resultOrigne = origne == expextOutPutOrigne;

				if (resultPath && resultOrigne && resultAbsoulte && resultRealtiv && resultMarked)
				{
					RY_CORE_INFO("Test 1 Marked Path: Pass");
				}
				else
				{
					RY_CORE_ERROR_IF(resultPath, "Test 1 Marked Path: FAILD! (From Path) {}", pathStr);
					RY_CORE_ERROR_IF(resultOrigne, "Test 1 Marked Path: FAILD! (From Origin) {}", magic_enum::enum_name(origne).data());
					RY_CORE_ERROR_IF(resultAbsoulte, "Test 1 Marked Path: FAILD! (From Absolute) {}", pathAbsoulteStr);
					RY_CORE_ERROR_IF(resultRealtiv, "Test 1 Marked Path: FAILD! (From Realtiv) {}", pathRelativeStr);
					RY_CORE_ERROR_IF(resultMarked, "Test 1 Marked Path: FAILD! (From Marked) {}", pathMarkedStr);
					RY_CORE_ASSERT(false);
				}
			}
			{
				constexpr const char* checkPath = "Editor-Assets/Texture/ChernoLogo.png";
				constexpr const char* expextOutPutRealtiv = "../Rynex-Editor/Editor-Assets/Texture/ChernoLogo.png";
				const std::string expextOutPutAbsoulte = (FileSystem::Path::GetEngineDirectory() / expextOutPutRealtiv).lexically_normal().generic_string();
				constexpr const char* expextOutPutMarked = RY_PATH_ENGINE_MARKER_STR "/../Rynex-Editor/Editor-Assets/Texture/ChernoLogo.png";

				RY_CORE_TRACE("Check now: {1} {0}", checkPath, magic_enum::enum_name(expextOutPutOrigne).data());
				FileSystem::Path fileSystemPath(checkPath);
				fileSystemPath.SetMarker(expextOutPutOrigne);
				std::string pathStr = fileSystemPath.GetPath().string();
				std::string pathAbsoulteStr = fileSystemPath.GetAbsolutePath().string();
				std::string pathRelativeStr = fileSystemPath.GetRelativePath().string();
				std::string pathMarkedStr = fileSystemPath.GetMarkedPathString();

				bool resultPath = pathStr == expextOutPutAbsoulte;
				bool resultAbsoulte = pathAbsoulteStr == expextOutPutAbsoulte;
				bool resultRealtiv = pathRelativeStr == expextOutPutRealtiv;
				bool resultMarked = pathMarkedStr == expextOutPutMarked;

				FileSystem::Path::Origin origne = fileSystemPath.GetOrigin();
				bool resultOrigne = origne == expextOutPutOrigne;

				if (resultPath && resultOrigne && resultAbsoulte && resultRealtiv && resultMarked)
				{
					RY_CORE_INFO("Test 2 Realtiv Path: Pass");
				}
				else
				{
					RY_CORE_ERROR_IF(resultPath, "Test 2 Realtiv Path: FAILD! (From Path) {}", pathStr);
					RY_CORE_ERROR_IF(resultOrigne, "Test 2 Realtiv Path: FAILD! (From Origin) {}", magic_enum::enum_name(origne).data());
					RY_CORE_ERROR_IF(resultAbsoulte, "Test 2 Realtiv Path: FAILD! (From Absolute) {}", pathAbsoulteStr);
					RY_CORE_ERROR_IF(resultRealtiv, "Test 2 Realtiv Marked Path: FAILD! (From Realtiv) {}", pathRelativeStr);
					RY_CORE_ERROR_IF(resultMarked, "Test 2 Realtiv Path: FAILD! (From Marked) {}", pathMarkedStr);
					RY_CORE_ASSERT(false);
				}
			}

			{
				// constexpr const char* checkPath = "D:/dev/Rynex-Rendering/Rynex-Editor/Editor-Assets/Assets/Models/Cube.rystmesh";
				constexpr const char* expextOutPutRealtiv = "../Rynex-Editor/Editor-Assets/Assets/Models/Cube.rystmesh";
				const std::string expextOutPutAbsoulte = (FileSystem::Path::GetEngineDirectory() / expextOutPutRealtiv).lexically_normal().generic_string();

				constexpr const char* expextOutPutMarked = "Engine#!#/../Rynex-Editor/Editor-Assets/Assets/Models/Cube.rystmesh";

				RY_CORE_TRACE("Check now: {1} {0}", expextOutPutAbsoulte, magic_enum::enum_name(expextOutPutOrigne).data());
				FileSystem::Path fileSystemPath(expextOutPutAbsoulte, FileSystem::Path::Origin::Project);
				fileSystemPath.SetMarker(expextOutPutOrigne);
				std::string pathStr = fileSystemPath.GetPath().string();
				std::string pathAbsoulteStr = fileSystemPath.GetAbsolutePath().string();
				std::string pathRelativeStr = fileSystemPath.GetRelativePath().string();
				std::string pathMarkedStr = fileSystemPath.GetMarkedPathString();

				bool resultPath = pathStr == expextOutPutAbsoulte;
				bool resultAbsoulte = pathAbsoulteStr == expextOutPutAbsoulte;
				bool resultRealtiv = pathRelativeStr == expextOutPutRealtiv;
				bool resultMarked = pathMarkedStr == expextOutPutMarked;

				FileSystem::Path::Origin origne = fileSystemPath.GetOrigin();
				bool resultOrigne = origne == expextOutPutOrigne;

				if (resultPath && resultOrigne && resultAbsoulte && resultRealtiv && resultMarked)
				{
					RY_CORE_INFO("Test 3 Absolute Path: Pass");
				}
				else
				{
					RY_CORE_ERROR_IF(resultPath, "Test 3 Absolute Path: FAILD! (From Path) {}", pathStr);
					RY_CORE_ERROR_IF(resultOrigne, "Test 3 Absolute Path: FAILD! (From Origin) {}", magic_enum::enum_name(origne).data());
					RY_CORE_ERROR_IF(resultAbsoulte, "Test 3 Absolute Path: FAILD! (From Absolute) {}", pathAbsoulteStr);
					RY_CORE_ERROR_IF(resultRealtiv, "Test 3 Absolute Marked Path: FAILD! (From Realtiv) {}", pathRelativeStr);
					RY_CORE_ERROR_IF(resultMarked, "Test 3 Absolute Path: FAILD! (From Marked) {}", pathMarkedStr);
					RY_CORE_ASSERT(false);
				}

			}
		}

		static void ProjectFilePathClassTestWindowsPathConvention()
		{
			constexpr const FileSystem::Path::Origin expextOutPutOrigne = FileSystem::Path::Origin::Project;
			{
				constexpr const char* checkPath = "Project#!#\\Assets\\Shaders\\Fetures\\Plane3DShadow.glsl";
				constexpr const char* expextOutPutRealtiv = "Assets/Shaders/Fetures/Plane3DShadow.glsl";
				const std::string expextOutPutAbsoulte = (FileSystem::Path::GetProjectDirectory() / expextOutPutRealtiv).lexically_normal().generic_string();

				constexpr const char* expextOutPutMarked = "Project#!#/Assets/Shaders/Fetures/Plane3DShadow.glsl";

				RY_CORE_TRACE("Check now: {1} {0}", checkPath, magic_enum::enum_name(expextOutPutOrigne).data());
				FileSystem::Path fileSystemPath(checkPath);
				std::string pathStr = fileSystemPath.GetPath().string();
				std::string pathAbsoulteStr = fileSystemPath.GetAbsolutePath().string();
				std::string pathRelativeStr = fileSystemPath.GetRelativePath().string();
				std::string pathMarkedStr = fileSystemPath.GetMarkedPathString();

				bool resultPath = pathStr == expextOutPutAbsoulte;
				bool resultAbsoulte = pathAbsoulteStr == expextOutPutAbsoulte;
				bool resultRealtiv = pathRelativeStr == expextOutPutRealtiv;
				bool resultMarked = pathMarkedStr == expextOutPutMarked;

				FileSystem::Path::Origin origne = fileSystemPath.GetOrigin();
				bool resultOrigne = origne == expextOutPutOrigne;

				if (resultPath && resultOrigne && resultAbsoulte && resultRealtiv && resultMarked)
				{
					RY_CORE_INFO("Test 1 Marked Path: Pass");
				}
				else
				{
					RY_CORE_ERROR_IF(resultPath, "Test 1 Marked Path: FAILD! (From Path) {}", pathStr);
					RY_CORE_ERROR_IF(resultOrigne, "Test 1 Marked Path: FAILD! (From Origin) {}", magic_enum::enum_name(origne).data());
					RY_CORE_ERROR_IF(resultAbsoulte, "Test 1 Marked Path: FAILD! (From Absolute) {}", pathAbsoulteStr);
					RY_CORE_ERROR_IF(resultRealtiv, "Test 1 Marked Marked Path: FAILD! (From Realtiv) {}", pathRelativeStr);
					RY_CORE_ERROR_IF(resultMarked, "Test 1 Marked Path: FAILD! (From Marked) {}", pathMarkedStr);
					RY_CORE_ASSERT(false);
				}
			}
			{
				constexpr const char* checkPath = "Assets\\Models\\CV-Model\\scene.rystmesh";
				constexpr const char* expextOutPutRealtiv = "Assets/Models/CV-Model/scene.rystmesh";
				const std::string expextOutPutAbsoulte = (FileSystem::Path::GetProjectDirectory() / expextOutPutRealtiv).lexically_normal().generic_string();

				constexpr const char* expextOutPutMarked = "Project#!#/Assets/Models/CV-Model/scene.rystmesh";

				RY_CORE_TRACE("Check now: {1} {0}", checkPath, magic_enum::enum_name(expextOutPutOrigne).data());
				FileSystem::Path fileSystemPath(checkPath);
				std::string pathStr = fileSystemPath.GetPath().string();
				std::string pathAbsoulteStr = fileSystemPath.GetAbsolutePath().string();
				std::string pathRelativeStr = fileSystemPath.GetRelativePath().string();
				std::string pathMarkedStr = fileSystemPath.GetMarkedPathString();

				bool resultPath = pathStr == expextOutPutAbsoulte;
				bool resultAbsoulte = pathAbsoulteStr == expextOutPutAbsoulte;
				bool resultRealtiv = pathRelativeStr == expextOutPutRealtiv;
				bool resultMarked = pathMarkedStr == expextOutPutMarked;

				FileSystem::Path::Origin origne = fileSystemPath.GetOrigin();
				bool resultOrigne = origne == expextOutPutOrigne;

				if (resultPath && resultOrigne && resultAbsoulte && resultRealtiv && resultMarked)
				{
					RY_CORE_INFO("Test 2 Realtiv Path: Pass");
				}
				else
				{
					RY_CORE_ERROR_IF(resultPath, "Test 2 Realtiv Path: FAILD! (From Path) {}", pathStr);
					RY_CORE_ERROR_IF(resultOrigne, "Test 2 Realtiv Path: FAILD! (From Origin) {}", magic_enum::enum_name(origne).data());
					RY_CORE_ERROR_IF(resultAbsoulte, "Test 2 Realtiv Path: FAILD! (From Absolute) {}", pathAbsoulteStr);
					RY_CORE_ERROR_IF(resultRealtiv, "Test 2 Realtiv Marked Path: FAILD! (From Realtiv) {}", pathRelativeStr);
					RY_CORE_ERROR_IF(resultMarked, "Test 2 Realtiv Path: FAILD! (From Marked) {}", pathMarkedStr);
					RY_CORE_ASSERT(false);
				}
			}

			{
				constexpr const char* checkPath = "D:\\dev\\Rynex-Test-Projects\\Test-Project-System\\Assets\\Scene\\Cube2-Test-LigthShader.rynexscene";
				constexpr const char* expextOutPutRealtiv = "Assets/Scene/Cube2-Test-LigthShader.rynexscene";

				const std::string expextOutPutAbsoulte = (FileSystem::Path::GetProjectDirectory() / expextOutPutRealtiv).lexically_normal().generic_string();
				constexpr const char* expextOutPutMarked = "Project#!#/Assets/Scene/Cube2-Test-LigthShader.rynexscene";

				RY_CORE_TRACE("Check now: {1} {0}", checkPath, magic_enum::enum_name(expextOutPutOrigne).data());
				FileSystem::Path fileSystemPath(checkPath);
				std::string pathStr = fileSystemPath.GetPath().string();
				std::string pathAbsoulteStr = fileSystemPath.GetAbsolutePath().string();
				std::string pathRelativeStr = fileSystemPath.GetRelativePath().string();
				std::string pathMarkedStr = fileSystemPath.GetMarkedPathString();

				bool resultPath = pathStr == expextOutPutAbsoulte;
				bool resultAbsoulte = pathAbsoulteStr == expextOutPutAbsoulte;
				bool resultRealtiv = pathRelativeStr == expextOutPutRealtiv;
				bool resultMarked = pathMarkedStr == expextOutPutMarked;

				FileSystem::Path::Origin origne = fileSystemPath.GetOrigin();
				bool resultOrigne = origne == expextOutPutOrigne;

				if (resultPath && resultOrigne && resultAbsoulte && resultRealtiv && resultMarked)
				{
					RY_CORE_INFO("Test 3 Absolute Path: Pass");
				}
				else
				{
					RY_CORE_ERROR_IF(resultPath, "Test 3 Absolute Path: FAILD! (From Path) {}", pathStr);
					RY_CORE_ERROR_IF(resultOrigne, "Test 3 Absolute Path: FAILD! (From Origin) {}", magic_enum::enum_name(origne).data());
					RY_CORE_ERROR_IF(resultAbsoulte, "Test 3 Absolute Path: FAILD! (From Absolute) {}", pathAbsoulteStr);
					RY_CORE_ERROR_IF(resultRealtiv, "Test 3 Absolute Marked Path: FAILD! (From Realtiv) {}", pathRelativeStr);
					RY_CORE_ERROR_IF(resultMarked, "Test 3 Absolute Path: FAILD! (From Marked) {}", pathMarkedStr);
					RY_CORE_ASSERT(false);
				}
			}

		}

		static void EngineFilePathClassTestWhrongOrgnieInput()
		{
			constexpr const FileSystem::Path::Origin expextOutPutOrigne = FileSystem::Path::Origin::Engine;
			{
				constexpr const char* checkPath = "Engine#!#/../Rynex-Editor/Editor-Assets/shaders/TextureTransform2.glsl";
				constexpr const char* expextOutPutRealtiv = "../Rynex-Editor/Editor-Assets/shaders/TextureTransform2.glsl";
				const std::string expextOutPutAbsoulte = (FileSystem::Path::GetEngineDirectory() / expextOutPutRealtiv).lexically_normal().generic_string();
				
				constexpr const char* expextOutPutMarked = "Engine#!#/../Rynex-Editor/Editor-Assets/shaders/TextureTransform2.glsl";

				RY_CORE_TRACE("Check now: {1} {0}", checkPath, magic_enum::enum_name(expextOutPutOrigne).data());
				FileSystem::Path fileSystemPath(checkPath, FileSystem::Path::Origin::Project);
				std::string pathStr = fileSystemPath.GetPath().string();
				std::string pathAbsoulteStr = fileSystemPath.GetAbsolutePath().string();
				std::string pathRelativeStr = fileSystemPath.GetRelativePath().string();
				std::string pathMarkedStr = fileSystemPath.GetMarkedPathString();

				bool resultPath = pathStr == expextOutPutAbsoulte;
				bool resultAbsoulte = pathAbsoulteStr == expextOutPutAbsoulte;
				bool resultRealtiv = pathRelativeStr == expextOutPutRealtiv;
				bool resultMarked = pathMarkedStr == expextOutPutMarked;

				FileSystem::Path::Origin origne = fileSystemPath.GetOrigin();
				bool resultOrigne = origne == expextOutPutOrigne;

				if (resultPath && resultOrigne && resultAbsoulte && resultRealtiv && resultMarked)
				{
					RY_CORE_INFO("Test 1 Marked Path: Pass");
				}
				else
				{
					RY_CORE_ERROR_IF(resultPath, "Test 1 Marked Path: FAILD! (From Path) {}", pathStr);
					RY_CORE_ERROR_IF(resultOrigne, "Test 1 Marked Path: FAILD! (From Origin) {}", magic_enum::enum_name(origne).data());
					RY_CORE_ERROR_IF(resultAbsoulte, "Test 1 Marked Path: FAILD! (From Absolute) {}", pathAbsoulteStr);
					RY_CORE_ERROR_IF(resultRealtiv, "Test 1 Marked Path: FAILD! (From Realtiv) {}", pathRelativeStr);
					RY_CORE_ERROR_IF(resultMarked, "Test 1 Marked Path: FAILD! (From Marked) {}", pathMarkedStr);
					RY_CORE_ASSERT(false);
				}
			}
			{
				constexpr const char* checkPath = "../Rynex-Editor/Editor-Assets/Texture/ChernoLogo.png";
				constexpr const char* expextOutPutRealtiv = "../Rynex-Editor/Editor-Assets/Texture/ChernoLogo.png";
				const std::string expextOutPutAbsoulte = (FileSystem::Path::GetProjectDirectory() / expextOutPutRealtiv).lexically_normal().generic_string();

				
				constexpr const char* expextOutPutMarked = RY_PATH_PROJECT_MARKER_STR "/../Rynex-Editor/Editor-Assets/Texture/ChernoLogo.png";

				RY_CORE_TRACE("Check now: {1} {0}", checkPath, magic_enum::enum_name(FileSystem::Path::Origin::Project).data());
				FileSystem::Path fileSystemPath(checkPath, FileSystem::Path::Origin::Project);
				std::string pathStr = fileSystemPath.GetPath().string();
				std::string pathAbsoulteStr = fileSystemPath.GetAbsolutePath().string();
				std::string pathRelativeStr = fileSystemPath.GetRelativePath().string();
				std::string pathMarkedStr = fileSystemPath.GetMarkedPathString();

				bool resultPath = pathStr == expextOutPutAbsoulte;
				bool resultAbsoulte = pathAbsoulteStr == expextOutPutAbsoulte;
				bool resultRealtiv = pathRelativeStr == expextOutPutRealtiv;
				bool resultMarked = pathMarkedStr == expextOutPutMarked;

				FileSystem::Path::Origin origne = fileSystemPath.GetOrigin();
				bool resultOrigne = origne == FileSystem::Path::Origin::Project;

				if (resultPath && resultOrigne && resultAbsoulte && resultRealtiv && resultMarked)
				{
					RY_CORE_INFO("Test 2 Realtiv Path: Pass");
				}
				else
				{
					RY_CORE_ERROR_IF(resultPath, "Test 2 Realtiv Path: FAILD! (From Path) {}", pathStr);
					RY_CORE_ERROR_IF(resultOrigne, "Test 2 Realtiv Path: FAILD! (From Origin) {}", magic_enum::enum_name(origne).data());
					RY_CORE_ERROR_IF(resultAbsoulte, "Test 2 Realtiv Path: FAILD! (From Absolute) {}", pathAbsoulteStr);
					RY_CORE_ERROR_IF(resultRealtiv, "Test 2 Realtiv Marked Path: FAILD! (From Realtiv) {}", pathRelativeStr);
					RY_CORE_ERROR_IF(resultMarked, "Test 2 Realtiv Path: FAILD! (From Marked) {}", pathMarkedStr);
					RY_CORE_ASSERT(false);
				}
			}

			{
				// constexpr const char* checkPath = "D:/dev/Rynex-Rendering/Rynex-Editor/Editor-Assets/Assets/Models/Cube.rystmesh";
				constexpr const char* expextOutPutRealtiv = "../Rynex-Editor/Editor-Assets/Assets/Models/Cube.rystmesh";
				const std::string expextOutPutAbsoulte = (FileSystem::Path::GetEngineDirectory() / expextOutPutRealtiv).lexically_normal().generic_string();

				
				constexpr const char* expextOutPutMarked = "Engine#!#/../Rynex-Editor/Editor-Assets/Assets/Models/Cube.rystmesh";

				RY_CORE_TRACE("Check now: {1} {0}", expextOutPutAbsoulte, magic_enum::enum_name(expextOutPutOrigne).data());
				FileSystem::Path fileSystemPath(expextOutPutAbsoulte, FileSystem::Path::Origin::Project);
				std::string pathStr = fileSystemPath.GetPath().string();
				std::string pathAbsoulteStr = fileSystemPath.GetAbsolutePath().string();
				std::string pathRelativeStr = fileSystemPath.GetRelativePath().string();
				std::string pathMarkedStr = fileSystemPath.GetMarkedPathString();

				bool resultPath = pathStr == expextOutPutAbsoulte;
				bool resultAbsoulte = pathAbsoulteStr == expextOutPutAbsoulte;
				bool resultRealtiv = pathRelativeStr == expextOutPutRealtiv;
				bool resultMarked = pathMarkedStr == expextOutPutMarked;

				FileSystem::Path::Origin origne = fileSystemPath.GetOrigin();
				bool resultOrigne = origne == expextOutPutOrigne;

				if (resultPath && resultOrigne && resultAbsoulte && resultRealtiv && resultMarked)
				{
					RY_CORE_INFO("Test 3 Absolute Path: Pass");
				}
				else
				{
					RY_CORE_ERROR_IF(resultPath, "Test 3 Absolute Path: FAILD! (From Path) {}", pathStr);
					RY_CORE_ERROR_IF(resultOrigne, "Test 3 Absolute Path: FAILD! (From Origin) {}", magic_enum::enum_name(origne).data());
					RY_CORE_ERROR_IF(resultAbsoulte, "Test 3 Absolute Path: FAILD! (From Absolute) {}", pathAbsoulteStr);
					RY_CORE_ERROR_IF(resultRealtiv, "Test 3 Absolute Marked Path: FAILD! (From Realtiv) {}", pathRelativeStr);
					RY_CORE_ERROR_IF(resultMarked, "Test 3 Absolute Path: FAILD! (From Marked) {}", pathMarkedStr);
					RY_CORE_ASSERT(false);
				}

			}
		}

		static void EngineFilePathClassTest()
		{
			constexpr const FileSystem::Path::Origin expextOutPutOrigne = FileSystem::Path::Origin::Engine;
			{

				constexpr const char* checkPath = "Engine#!#/../Rynex-Editor/Editor-Assets/shaders/TextureTransform2.glsl";				
				constexpr const char* expextOutPutRealtiv = "../Rynex-Editor/Editor-Assets/shaders/TextureTransform2.glsl";

				const std::string expextOutPutAbsoulte = std::filesystem::absolute(FileSystem::Path::GetEngineDirectory() / expextOutPutRealtiv).generic_string();

				constexpr const char* expextOutPutMarked = "Engine#!#/../Rynex-Editor/Editor-Assets/shaders/TextureTransform2.glsl";

				RY_CORE_TRACE("Check now: {1} {0}", checkPath, magic_enum::enum_name(expextOutPutOrigne).data());
				FileSystem::Path fileSystemPath(checkPath);
				std::string pathStr = fileSystemPath.GetPath().string();
				std::string pathAbsoulteStr = fileSystemPath.GetAbsolutePath().string();
				std::string pathRelativeStr = fileSystemPath.GetRelativePath().string();
				std::string pathMarkedStr = fileSystemPath.GetMarkedPathString();

				bool resultPath = pathStr == expextOutPutAbsoulte;
				bool resultAbsoulte = pathAbsoulteStr == expextOutPutAbsoulte;
				bool resultRealtiv = pathRelativeStr == expextOutPutRealtiv;
				bool resultMarked = pathMarkedStr == expextOutPutMarked;

				FileSystem::Path::Origin origne = fileSystemPath.GetOrigin();
				bool resultOrigne = origne == expextOutPutOrigne;

				if (resultPath && resultOrigne && resultAbsoulte && resultRealtiv && resultMarked)
				{
					RY_CORE_INFO("Test 1 Marked Path: Pass");
				}
				else
				{
					RY_CORE_ERROR_IF(resultPath, "Test 1 Marked Path: FAILD! (From Path) {}", pathStr);
					RY_CORE_ERROR_IF(resultOrigne, "Test 1 Marked Path: FAILD! (From Origin) {}", magic_enum::enum_name(origne).data());
					RY_CORE_ERROR_IF(resultAbsoulte, "Test 1 Marked Path: FAILD! (From Absolute) {}", pathAbsoulteStr);
					RY_CORE_ERROR_IF(resultRealtiv, "Test 1 Marked Path: FAILD! (From Realtiv) {}", pathRelativeStr);
					RY_CORE_ERROR_IF(resultMarked, "Test 1 Marked Path: FAILD! (From Marked) {}", pathMarkedStr);
					RY_CORE_ASSERT(false);
				}
			}
			{
				constexpr const char* checkPath = "../Rynex-Editor/Editor-Assets/Texture/ChernoLogo.png";
				constexpr const char* expextOutPutRealtiv = "../Rynex-Editor/Editor-Assets/Texture/ChernoLogo.png";

				const std::string expextOutPutAbsoulte = (FileSystem::Path::GetEngineDirectory() / expextOutPutRealtiv).lexically_normal().generic_string();
				constexpr const char* expextOutPutMarked = "Engine#!#/../Rynex-Editor/Editor-Assets/Texture/ChernoLogo.png";

				RY_CORE_TRACE("Check now: {1} {0}", checkPath, magic_enum::enum_name(expextOutPutOrigne).data());
				FileSystem::Path fileSystemPath(checkPath);
				std::string pathStr = fileSystemPath.GetPath().string();
				std::string pathAbsoulteStr = fileSystemPath.GetAbsolutePath().string();
				std::string pathRelativeStr = fileSystemPath.GetRelativePath().string();
				std::string pathMarkedStr = fileSystemPath.GetMarkedPathString();

				bool resultPath = pathStr == expextOutPutAbsoulte;
				bool resultAbsoulte = pathAbsoulteStr == expextOutPutAbsoulte;
				bool resultRealtiv = pathRelativeStr == expextOutPutRealtiv;
				bool resultMarked = pathMarkedStr == expextOutPutMarked;

				FileSystem::Path::Origin origne = fileSystemPath.GetOrigin();
				bool resultOrigne = origne == expextOutPutOrigne;

				if (resultPath && resultOrigne && resultAbsoulte && resultRealtiv && resultMarked)
				{
					RY_CORE_INFO("Test 2 Realtiv Path: Pass");
				}
				else
				{
					RY_CORE_ERROR_IF(resultPath, "Test 2 Realtiv Path: FAILD! (From Path) {}", pathStr);
					RY_CORE_ERROR_IF(resultOrigne, "Test 2 Realtiv Path: FAILD! (From Origin) {}", magic_enum::enum_name(origne).data());
					RY_CORE_ERROR_IF(resultAbsoulte, "Test 2 Realtiv Path: FAILD! (From Absolute) {}", pathAbsoulteStr);
					RY_CORE_ERROR_IF(resultRealtiv, "Test 2 Realtiv Marked Path: FAILD! (From Realtiv) {}", pathRelativeStr);
					RY_CORE_ERROR_IF(resultMarked, "Test 2 Realtiv Path: FAILD! (From Marked) {}", pathMarkedStr);
					RY_CORE_ASSERT(false);
				}
			}

			{
				constexpr const char* checkPath = "D:/dev/Rynex-Rendering/Rynex-Editor/Editor-Assets/Assets/Models/Cube.rystmesh";
				constexpr const char* expextOutPutRealtiv = "../Rynex-Editor/Editor-Assets/Assets/Models/Cube.rystmesh";
				
				const std::string expextOutPutAbsoulte = (FileSystem::Path::GetEngineDirectory() / expextOutPutRealtiv).lexically_normal().generic_string();
				constexpr const char* expextOutPutMarked = "Engine#!#/../Rynex-Editor/Editor-Assets/Assets/Models/Cube.rystmesh";

				RY_CORE_TRACE("Check now: {1} {0}", checkPath, magic_enum::enum_name(expextOutPutOrigne).data());
				FileSystem::Path fileSystemPath(expextOutPutAbsoulte);
				std::string pathStr = fileSystemPath.GetPath().string();
				std::string pathAbsoulteStr = fileSystemPath.GetAbsolutePath().string();
				std::string pathRelativeStr = fileSystemPath.GetRelativePath().string();
				std::string pathMarkedStr = fileSystemPath.GetMarkedPathString();

				bool resultPath = pathStr == expextOutPutAbsoulte;
				bool resultAbsoulte = pathAbsoulteStr == expextOutPutAbsoulte;
				bool resultRealtiv = pathRelativeStr == expextOutPutRealtiv;
				bool resultMarked = pathMarkedStr == expextOutPutMarked;

				FileSystem::Path::Origin origne = fileSystemPath.GetOrigin();
				bool resultOrigne = origne == expextOutPutOrigne;

				if (resultPath && resultOrigne && resultAbsoulte && resultRealtiv && resultMarked)
				{
					RY_CORE_INFO("Test 3 Absolute Path: Pass");
				}
				else
				{
					RY_CORE_ERROR_IF(resultPath, "Test 3 Absolute Path: FAILD! (From Path) {}", pathStr);
					RY_CORE_ERROR_IF(resultOrigne, "Test 3 Absolute Path: FAILD! (From Origin) {}", magic_enum::enum_name(origne).data());
					RY_CORE_ERROR_IF(resultAbsoulte, "Test 3 Absolute Path: FAILD! (From Absolute) {}", pathAbsoulteStr);
					RY_CORE_ERROR_IF(resultRealtiv, "Test 3 Absolute Marked Path: FAILD! (From Realtiv) {}", pathRelativeStr);
					RY_CORE_ERROR_IF(resultMarked, "Test 3 Absolute Path: FAILD! (From Marked) {}", pathMarkedStr);
					RY_CORE_ASSERT(false);
				}

			}
		}

		static void ProjectFilePathClassTest()
		{
			constexpr const FileSystem::Path::Origin expextOutPutOrigne = FileSystem::Path::Origin::Project;
			{
				constexpr const char* checkPath = "Project#!#/Assets/Shaders/Fetures/Plane3DShadow.glsl";
				constexpr const char* expextOutPutRealtiv = "Assets/Shaders/Fetures/Plane3DShadow.glsl";

				const std::string expextOutPutAbsoulte = (FileSystem::Path::GetProjectDirectory() / expextOutPutRealtiv).lexically_normal().generic_string();
				constexpr const char* expextOutPutMarked = "Project#!#/Assets/Shaders/Fetures/Plane3DShadow.glsl";

				RY_CORE_TRACE("Check now: {1} {0}", checkPath, magic_enum::enum_name(expextOutPutOrigne).data());
				FileSystem::Path fileSystemPath(checkPath);
				std::string pathStr = fileSystemPath.GetPath().string();
				std::string pathAbsoulteStr = fileSystemPath.GetAbsolutePath().string();
				std::string pathRelativeStr = fileSystemPath.GetRelativePath().string();
				std::string pathMarkedStr = fileSystemPath.GetMarkedPathString();

				bool resultPath = pathStr == expextOutPutAbsoulte;
				bool resultAbsoulte = pathAbsoulteStr == expextOutPutAbsoulte;
				bool resultRealtiv = pathRelativeStr == expextOutPutRealtiv;
				bool resultMarked = pathMarkedStr == expextOutPutMarked;

				FileSystem::Path::Origin origne = fileSystemPath.GetOrigin();
				bool resultOrigne = origne == expextOutPutOrigne;

				if (resultPath && resultOrigne && resultAbsoulte && resultRealtiv && resultMarked)
				{
					RY_CORE_INFO("Test 1 Marked Path: Pass");
				}
				else
				{
					RY_CORE_ERROR_IF(resultPath, "Test 1 Marked Path: FAILD! (From Path) {}", pathStr);
					RY_CORE_ERROR_IF(resultOrigne, "Test 1 Marked Path: FAILD! (From Origin) {}", magic_enum::enum_name(origne).data());
					RY_CORE_ERROR_IF(resultAbsoulte, "Test 1 Marked Path: FAILD! (From Absolute) {}", pathAbsoulteStr);
					RY_CORE_ERROR_IF(resultRealtiv, "Test 1 Marked Marked Path: FAILD! (From Realtiv) {}", pathRelativeStr);
					RY_CORE_ERROR_IF(resultMarked, "Test 1 Marked Path: FAILD! (From Marked) {}", pathMarkedStr);
					RY_CORE_ASSERT(false);
				}
			}
			{
				constexpr const char* checkPath = "Assets/Models/CV-Model/scene.rystmesh";
				constexpr const char* expextOutPutRealtiv = "Assets/Models/CV-Model/scene.rystmesh";
				
				const std::string expextOutPutAbsoulte = (FileSystem::Path::GetProjectDirectory() / expextOutPutRealtiv).lexically_normal().generic_string();
				constexpr const char* expextOutPutMarked = "Project#!#/Assets/Models/CV-Model/scene.rystmesh";

				RY_CORE_TRACE("Check now: {1} {0}", checkPath, magic_enum::enum_name(expextOutPutOrigne).data());
				FileSystem::Path fileSystemPath(checkPath);
				std::string pathStr = fileSystemPath.GetPath().string();
				std::string pathAbsoulteStr = fileSystemPath.GetAbsolutePath().string();
				std::string pathRelativeStr = fileSystemPath.GetRelativePath().string();
				std::string pathMarkedStr = fileSystemPath.GetMarkedPathString();

				bool resultPath = pathStr == expextOutPutAbsoulte;
				bool resultAbsoulte = pathAbsoulteStr == expextOutPutAbsoulte;
				bool resultRealtiv = pathRelativeStr == expextOutPutRealtiv;
				bool resultMarked = pathMarkedStr == expextOutPutMarked;

				FileSystem::Path::Origin origne = fileSystemPath.GetOrigin();
				bool resultOrigne = origne == expextOutPutOrigne;

				if (resultPath && resultOrigne && resultAbsoulte && resultRealtiv && resultMarked)
				{
					RY_CORE_INFO("Test 2 Realtiv Path: Pass");
				}
				else
				{
					RY_CORE_ERROR_IF(resultPath, "Test 2 Realtiv Path: FAILD! (From Path) {}", pathStr);
					RY_CORE_ERROR_IF(resultOrigne, "Test 2 Realtiv Path: FAILD! (From Origin) {}", magic_enum::enum_name(origne).data());
					RY_CORE_ERROR_IF(resultAbsoulte, "Test 2 Realtiv Path: FAILD! (From Absolute) {}", pathAbsoulteStr);
					RY_CORE_ERROR_IF(resultRealtiv, "Test 2 Realtiv Marked Path: FAILD! (From Realtiv) {}", pathRelativeStr);
					RY_CORE_ERROR_IF(resultMarked, "Test 2 Realtiv Path: FAILD! (From Marked) {}", pathMarkedStr);
					RY_CORE_ASSERT(false);
				}
			}

			{
				constexpr const char* checkPath = "D:/dev/Rynex-Test-Projects/Test-Project-System/Assets/Scene/Cube2-Test-LigthShader.rynexscene";
				constexpr const char* expextOutPutRealtiv = "Assets/Scene/Cube2-Test-LigthShader.rynexscene";
				const std::string expextOutPutAbsoulte = (FileSystem::Path::GetProjectDirectory() / expextOutPutRealtiv).lexically_normal().generic_string();

				constexpr const char* expextOutPutMarked = "Project#!#/Assets/Scene/Cube2-Test-LigthShader.rynexscene";

				RY_CORE_TRACE("Check now: {1} {0}", checkPath, magic_enum::enum_name(expextOutPutOrigne).data());
				FileSystem::Path fileSystemPath(checkPath);
				std::string pathStr = fileSystemPath.GetPath().string();
				std::string pathAbsoulteStr = fileSystemPath.GetAbsolutePath().string();
				std::string pathRelativeStr = fileSystemPath.GetRelativePath().string();
				std::string pathMarkedStr = fileSystemPath.GetMarkedPathString();

				bool resultPath = pathStr == expextOutPutAbsoulte;
				bool resultAbsoulte = pathAbsoulteStr == expextOutPutAbsoulte;
				bool resultRealtiv = pathRelativeStr == expextOutPutRealtiv;
				bool resultMarked = pathMarkedStr == expextOutPutMarked;

				FileSystem::Path::Origin origne = fileSystemPath.GetOrigin();
				bool resultOrigne = origne == expextOutPutOrigne;

				if (resultPath && resultOrigne && resultAbsoulte && resultRealtiv && resultMarked)
				{
					RY_CORE_INFO("Test 3 Absolute Path: Pass");
				}
				else
				{
					RY_CORE_ERROR_IF(resultPath, "Test 3 Absolute Path: FAILD! (From Path) {}", pathStr);
					RY_CORE_ERROR_IF(resultOrigne, "Test 3 Absolute Path: FAILD! (From Origin) {}", magic_enum::enum_name(origne).data());
					RY_CORE_ERROR_IF(resultAbsoulte, "Test 3 Absolute Path: FAILD! (From Absolute) {}", pathAbsoulteStr);
					RY_CORE_ERROR_IF(resultRealtiv, "Test 3 Absolute Marked Path: FAILD! (From Realtiv) {}", pathRelativeStr);
					RY_CORE_ERROR_IF(resultMarked, "Test 3 Absolute Path: FAILD! (From Marked) {}", pathMarkedStr);
					RY_CORE_ASSERT(false);
				}
			}

		}
		
		static void UnknownFilePathClassTest()
		{
			constexpr const FileSystem::Path::Origin expextOutPutOrigne = FileSystem::Path::Origin::Unknown;
			{
				constexpr const char* checkPath = "../../../Dokument/Notiz.txt";
				constexpr const char* expextOutPutAbsoulte = "/../../../Dokument/Notiz.txt";

				constexpr const char* expextOutPutRealtiv = "/../../../Dokument/Notiz.txt";
				constexpr const char* expextOutPutMarked = "NotVaild!#/../../../Dokument/Notiz.txt";

				RY_CORE_TRACE("Check now: {1} {0}", checkPath, magic_enum::enum_name(expextOutPutOrigne).data());
				FileSystem::Path fileSystemPath(checkPath);
				std::string pathStr = fileSystemPath.GetPath().string();
				std::string pathAbsoulteStr = fileSystemPath.GetAbsolutePath().string();
				std::string pathRelativeStr = fileSystemPath.GetRelativePath().string();
				std::string pathMarkedStr = fileSystemPath.GetMarkedPathString();

				bool resultPath = pathStr == expextOutPutAbsoulte;
				bool resultAbsoulte = pathAbsoulteStr == expextOutPutAbsoulte;
				bool resultRealtiv = pathRelativeStr == expextOutPutRealtiv;
				bool resultMarked = pathMarkedStr == expextOutPutMarked;

				FileSystem::Path::Origin origne = fileSystemPath.GetOrigin();
				bool resultOrigne = origne == expextOutPutOrigne;

				if (resultPath && resultOrigne && resultAbsoulte && resultRealtiv && resultMarked)
				{
					RY_CORE_INFO("Test 1 Realtiv Path: Pass");
				}
				else
				{
					RY_CORE_ERROR_IF(resultPath, "Test 1 Realtiv Path: FAILD! (From Path) {}", pathStr);
					RY_CORE_ERROR_IF(resultOrigne, "Test 1 Realtiv Path: FAILD! (From Origin) {}", magic_enum::enum_name(origne).data());
					RY_CORE_ERROR_IF(resultAbsoulte, "Test 1 Realtiv Path: FAILD! (From Absolute) {}", pathAbsoulteStr);
					RY_CORE_ERROR_IF(resultRealtiv, "Test 1 Realtiv Marked Path: FAILD! (From Realtiv) {}", pathRelativeStr);
					RY_CORE_ERROR_IF(resultMarked, "Test 1 Realtiv Path: FAILD! (From Marked) {}", pathMarkedStr);
					RY_CORE_ASSERT(false);
				}
			}
			{
				constexpr const char* checkPath = "NotVaild!#/../../Pictures/Bird.png";
				constexpr const char* expextOutPutAbsoulte = "/../../Pictures/Bird.png";
				
				constexpr const char* expextOutPutRealtiv = "/../../Pictures/Bird.png";
				constexpr const char* expextOutPutMarked = "NotVaild!#/../../Pictures/Bird.png";

				RY_CORE_TRACE("Check now: {1} {0}", checkPath, magic_enum::enum_name(expextOutPutOrigne).data());
				FileSystem::Path fileSystemPath(checkPath);
				std::string pathStr = fileSystemPath.GetPath().string();
				std::string pathAbsoulteStr = fileSystemPath.GetAbsolutePath().string();
				std::string pathRelativeStr = fileSystemPath.GetRelativePath().string();
				std::string pathMarkedStr = fileSystemPath.GetMarkedPathString();

				bool resultPath = pathStr == expextOutPutAbsoulte;
				bool resultAbsoulte = pathAbsoulteStr == expextOutPutAbsoulte;
				bool resultRealtiv = pathRelativeStr == expextOutPutRealtiv;
				bool resultMarked = pathMarkedStr == expextOutPutMarked;

				FileSystem::Path::Origin origne = fileSystemPath.GetOrigin();
				bool resultOrigne = origne == expextOutPutOrigne;

				if (resultPath && resultOrigne && resultAbsoulte && resultRealtiv && resultMarked)
				{
					RY_CORE_INFO("Test 2 Marked Path: Pass");
				}
				else
				{
					RY_CORE_ERROR_IF(resultPath, "Test 2 Marked Path: FAILD! (From Path) {}", pathStr);
					RY_CORE_ERROR_IF(resultOrigne, "Test 2 Marked Path: FAILD! (From Origin) {}", magic_enum::enum_name(origne).data());
					RY_CORE_ERROR_IF(resultAbsoulte, "Test 2 Marked Path: FAILD! (From Absolute) {}", pathAbsoulteStr);
					RY_CORE_ERROR_IF(resultRealtiv, "Test 2 Marked Marked Path: FAILD! (From Realtiv) {}", pathRelativeStr);
					RY_CORE_ERROR_IF(resultMarked, "Test 2 Marked Path: FAILD! (From Marked) {}", pathMarkedStr);
					RY_CORE_ASSERT(false);
				}
			}

			{
				constexpr const char* checkPath = "C:/Users/Public/Pictures/CheckeBord.png";
				constexpr const char* expextOutPutAbsoulte = "C:/Users/Public/Pictures/CheckeBord.png";

				constexpr const char* expextOutPutRealtiv = "C:/Users/Public/Pictures/CheckeBord.png";
				constexpr const char* expextOutPutMarked = "NotVaild!#/C:/Users/Public/Pictures/CheckeBord.png";

				RY_CORE_TRACE("Check now: {1} {0}", checkPath, magic_enum::enum_name(expextOutPutOrigne).data());
				FileSystem::Path fileSystemPath(checkPath);
				std::string pathStr = fileSystemPath.GetPath().string();
				std::string pathAbsoulteStr = fileSystemPath.GetAbsolutePath().string();
				std::string pathRelativeStr = fileSystemPath.GetRelativePath().string();
				std::string pathMarkedStr = fileSystemPath.GetMarkedPathString();

				bool resultPath = pathStr == expextOutPutAbsoulte;
				bool resultAbsoulte = pathAbsoulteStr == expextOutPutAbsoulte;
				bool resultRealtiv = pathRelativeStr == expextOutPutRealtiv;
				bool resultMarked = pathMarkedStr == expextOutPutMarked;

				FileSystem::Path::Origin origne = fileSystemPath.GetOrigin();
				bool resultOrigne = origne == expextOutPutOrigne;

				if (resultPath && resultOrigne && resultAbsoulte && resultRealtiv && resultMarked)
				{
					RY_CORE_INFO("Test 2 Marked Path: Pass");
				}
				else
				{
					RY_CORE_ERROR_IF(resultPath, "Test 3 Absolute Path: FAILD! (From Path) {}", pathStr);
					RY_CORE_ERROR_IF(resultOrigne, "Test 3 Absolute Path: FAILD! (From Origin) {}", magic_enum::enum_name(origne).data());
					RY_CORE_ERROR_IF(resultAbsoulte, "Test 3 Absolute Path: FAILD! (From Absolute) {}", pathAbsoulteStr);
					RY_CORE_ERROR_IF(resultRealtiv, "Test 3 Absolute Marked Path: FAILD! (From Realtiv) {}", pathRelativeStr);
					RY_CORE_ERROR_IF(resultMarked, "Test 3 Absolute Path: FAILD! (From Marked) {}", pathMarkedStr);
					RY_CORE_ASSERT(false);
				}
			}
		}



		static void FileSystemClass()
		{
			UnknownFilePathClassTest();
			ProjectFilePathClassTest();
			EngineFilePathClassTest();

			EngineFilePathClassTestWhrongOrgnieInput();
			EngineFilePathClassTestChangeOrigne();

		}
	}

	bool EditorAssetMangerSerialzation::SerilzeThread(const std::filesystem::path& filepath, std::map<AssetHandle, AssetMetadata>* handleReg)
	{
		
		YAML::Emitter out;
		{
			out << YAML::BeginMap;
			out << YAML::Key << "AssetRegistry" << YAML::Value;
			out << YAML::BeginSeq;
			for (const auto& [handle, metadata] : *handleReg)
			{
				
				if (metadata.GetIntern())
					continue;
				out << YAML::BeginMap;
				out << YAML::Key << "Handle" << YAML::Value << handle;
				std::filesystem::path path = metadata.FilePath.lexically_normal();
				
				std::string filePathStr = path.generic_string();
				
				std::pair <std::string, std::filesystem::path> pair  = Project::GeanrateRealtivePathAndMarker(filePathStr);
				const std::string& marker = pair.first;
				const std::filesystem::path& realtivePath = pair.second;
				std::filesystem::path basePath = Project::GetAbsulteFilePathFormMarker(marker);
				std::filesystem::path pathAbosulte = basePath / realtivePath;
				std::string pathAbosulteStr = pathAbosulte.generic_string();
				pathAbosulte = pathAbosulteStr;

				std::string filePathMarkerStr = Project::SetMarker(realtivePath, marker);

				// out << YAML::Key << "FilePath" << YAML::Value << filePathStr;
				// out << YAML::Key << "FilePath-Absolute" << YAML::Value << pathAbosulte.string();
				// out << YAML::Key << "FilePath-Realtiv" << YAML::Value << realtivePath.string();
				out << YAML::Key << "FilePath-ProjectMarker" << YAML::Value << filePathMarkerStr;

				out << YAML::Key << "Type" << YAML::Value << Asset::AssetTypeToString(metadata.Type);
				out << YAML::Key << "Name" << YAML::Value << metadata.Name;
				out << YAML::Key << "ChangeTime" << YAML::Value << metadata.ChangeTime;

				out << YAML::EndMap;
			}
			out << YAML::EndSeq;
			out << YAML::EndMap;
		}

		std::ofstream fout(filepath);
		fout << out.c_str();
		fout.close();


		return true;
	}

	bool EditorAssetMangerSerialzation::DeserilzeThread(const std::filesystem::path& filepath, std::map<AssetHandle, AssetMetadata>* handleReg, std::map<std::filesystem::path, AssetHandle>* pathReg)
	{
		RY_CORE_INFO("Deserialze Path: '{0}'", filepath.string().c_str());
		RY_REMBER_FUNC_CHANGE("After Testing remove Utils::TestFileSystemClass from this methode and funktion self!");
		Test::FileSystemClass();

		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filepath.string());
			// data = YAML::LoadFile("AssetRegistry.ryr");
		}
		catch (YAML::ParserException e)
		{
			RY_CORE_ERROR("Failed to load project file '{0}'\n     {1}", filepath.string(), e.what());
			return false;
		}

		YAML::Node rootNode = data["AssetRegistry"];
		if (!rootNode)
			return false;
		for (const YAML::detail::iterator_value& node : rootNode)
		{
			AssetHandle handle = node["Handle"].as<uint64_t>();

			AssetMetadata metadata;
			std::string filePathStr = "";
			std::string filePathAbsoluteStr = "";
			std::string filePathRealtivStr = "";
			if(YAML::Node nodePath = node["FilePath-Absolute"])
				filePathAbsoluteStr = nodePath.as<std::string>();
			if (YAML::Node nodePath = node["FilePath-Realtiv"])
				filePathRealtivStr = nodePath.as<std::string>();
			std::string filePathMarkerStr = node["FilePath-ProjectMarker"].as<std::string>();

#if 1
			FileSystem::Path pathSystemMarked = FileSystem::Path(filePathMarkerStr);
			// FileSystem::Path pathSystemRealtiv = FileSystem::Path(filePathAbsoluteStr);
			// FileSystem::Path pathSystemAbsoulte = FileSystem::Path(filePathRealtivStr);
#else
			RY_REMBER_FUNC_CHANGE("Test #if FileSystem::Path with out put!");
#endif
			
			
			std::replace(filePathMarkerStr.begin(), filePathMarkerStr.end(), '\\', '/');
		
			if (YAML::Node nodePath = node["FilePath"])
			{
				filePathStr = nodePath.as<std::string>();

				std::filesystem::path origFilePath = filePathStr;
				filePathStr = origFilePath.generic_string();
				origFilePath = filePathStr;

				metadata.SetMarkedFilePath(filePathMarkerStr, origFilePath);

			}
			else 
			{
				metadata.SetMarkedFilePath(filePathMarkerStr);
			}
			

			metadata.Type = Asset::AssetTypeFromString(node["Type"].as<std::string>());
			AssetType assetType = Asset::GetAssetTypeFromFilePath(metadata.FilePath);
			if (metadata.Type != assetType)
				metadata.Type = assetType;


			metadata.Name = node["Name"].as<std::string>();
			if (node["ChangeTime"])
				metadata.ChangeTime = node["ChangeTime"].as<std::string>();
			else
				metadata.ChangeTime = AssetRegistry::GetCurrentTimeStr();
			metadata.SetActive(true);
			metadata.SetIntern(false);
			metadata.State = AssetState::LostConection;

			pathReg->insert_or_assign(metadata.RealtivePath, handle);
			pathReg->insert_or_assign(metadata.AbsolutePath, handle);
			pathReg->insert_or_assign(metadata.FilePath, handle);
			handleReg->insert_or_assign(handle, metadata);
		}

		return true;
	}

}