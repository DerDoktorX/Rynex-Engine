#include "rypch.h"
#include "AssetMetadata.h"
#include <Rynex/Project/Project.h>

namespace Rynex {

	void AssetMetadata::SetFilePath(const std::filesystem::path& path)
	{
		std::filesystem::path pathToAsset = GetPathAsGenaric(path);
		std::pair <std::string, std::filesystem::path> pair = Project::GeanrateRealtivePathAndMarker(pathToAsset);
		const std::string& markerStr = pair.first;
		const std::filesystem::path& realtivePath = pair.second;
		std::filesystem::path basePath = Project::GetAbsulteFilePathFormMarker(markerStr);
		
		std::filesystem::path abosultePath = GetPathAsGenaric(basePath / realtivePath);
		std::string filePathMarker = Project::SetMarker(realtivePath, markerStr);

		if (path.is_absolute() && pathToAsset != abosultePath)
			RY_CORE_WARN("The Absolute Path {} is not like the Orignale Path {}", abosultePath, pathToAsset);
		else if (path.is_relative() && pathToAsset != realtivePath)
			RY_CORE_WARN("The Relative Path {} is not like the Orignale Path {}", realtivePath, pathToAsset);
		

		AbsolutePath = pathToAsset.is_absolute() && pathToAsset != abosultePath ? pathToAsset : abosultePath;
		RealtivePath = pathToAsset.is_relative() && pathToAsset != realtivePath ? pathToAsset : realtivePath;
		
		PathMarker = filePathMarker;
		FilePath = path;

		{
			FileSystem::Path fileSystemPath = FileSystem::Path(AbsolutePath);
			std::filesystem::path pathRealtive = fileSystemPath.GetRelativePath();
			std::filesystem::path pathAbosulte = fileSystemPath.GetAbsolutePath();
			std::string pathMarkedStr = fileSystemPath.GetMarkedPathString();

			RY_CORE_ASSERT(RealtivePath == realtivePath, "RealtivePath: {} realtivePath: {}", RealtivePath, realtivePath);
			RY_CORE_ASSERT(AbsolutePath == abosultePath, "AbsolutePath: {} abosultePath: {}", AbsolutePath, abosultePath);
			RY_CORE_ASSERT(PathMarker == pathMarkedStr, "PathMarker: {} pathMarkedStr: {}", PathMarker, pathMarkedStr);
		}
		

		SetDisc(true);
	}

	void AssetMetadata::SetFilePath(const FileSystem::Path& path)
	{
		Path = path;

		RealtivePath = path.GetRelativePath();
		AbsolutePath = path.GetAbsolutePath();
		FilePath = path.GetRelativePath();
		
	}

	void AssetMetadata::SetMarkedFilePath(const std::string& markedPathStr)
	{	
		std::string marker = Project::ExtraxtMarker(markedPathStr);
		std::filesystem::path realtivePath = Project::RemoveMarker(markedPathStr, marker);
		std::filesystem::path basePath = Project::GetAbsulteFilePathFormMarker(marker);
		std::filesystem::path abosultePath = GetPathAsGenaric(basePath / realtivePath);
		AbsolutePath = abosultePath;
		RealtivePath = realtivePath;
		PathMarker = markedPathStr;
		FilePath = marker == RY_PATH_ENGINE_MARKER_STR ? RealtivePath : AbsolutePath;
		{
			FileSystem::Path fileSystemPath = FileSystem::Path(AbsolutePath);
			std::filesystem::path pathRealtive = fileSystemPath.GetRelativePath();
			std::filesystem::path pathAbosulte = fileSystemPath.GetAbsolutePath();
			std::string pathMarkedStr = fileSystemPath.GetMarkedPathString();

			RY_CORE_ASSERT(RealtivePath == realtivePath, "RealtivePath: {} realtivePath: {}", RealtivePath, realtivePath);
			RY_CORE_ASSERT(AbsolutePath == abosultePath, "AbsolutePath: {} abosultePath: {}", AbsolutePath, abosultePath);
			RY_CORE_ASSERT(PathMarker == pathMarkedStr, "PathMarker: {} pathMarkedStr: {}", PathMarker, pathMarkedStr);
		}
		
		
		SetDisc(true);
	}

	void AssetMetadata::SetMarkedFilePath(const std::string& markedPathStr, const std::filesystem::path& path)
	{
		SetMarkedFilePath(markedPathStr);
		if (path != AbsolutePath && path != RealtivePath)
		{
			RY_CORE_ERROR("Marked File path not korekt Interpreted! {} as {}", markedPathStr, path);
			SetFilePath(path);
		}
	}
}