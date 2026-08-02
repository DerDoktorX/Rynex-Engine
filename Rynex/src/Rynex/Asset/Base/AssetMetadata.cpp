#include "rypch.h"
#include "AssetMetadata.h"
#include <Rynex/Project/Project.h>

namespace Rynex {

	void AssetMetadata::SetFilePath(const std::filesystem::path& path)
	{
		std::filesystem::path pathToAsset = GetPathAsGenaric(path);
		std::pair <std::string, std::filesystem::path> pair = Project::GeanrateRealtivePathAndMarker(pathToAsset);
		const std::string& marker = pair.first;
		const std::filesystem::path& realtivePath = pair.second;
		std::filesystem::path basePath = Project::GetAbsulteFilePathFormMarker(marker);
		std::filesystem::path filePathMarker = Project::SetMarker(realtivePath, marker);
		std::filesystem::path pathAbosulte = GetPathAsGenaric(basePath / realtivePath);


		if (path.is_absolute() && pathToAsset != pathAbosulte)
			RY_CORE_WARN("The Absolute Path {} is not like the Orignale Path {}", pathAbosulte, pathToAsset);
		else if (path.is_relative() && pathToAsset != realtivePath)
			RY_CORE_WARN("The Realtive Path {} is not like the Orignale Path {}", realtivePath, pathToAsset);

		AbsolutePath = pathToAsset.is_absolute() && pathToAsset != pathAbosulte ? pathToAsset : pathAbosulte;
		RealtivePath = pathToAsset.is_relative() && pathToAsset != realtivePath ? pathToAsset : realtivePath;
		PathMarker = filePathMarker;
		FilePath = path;

		SetDisc(true);
	}

	void AssetMetadata::SetMarkedFilePath(const std::filesystem::path& markedPath)
	{	
		std::string marker = Project::ExtraxtMarker(markedPath);
		std::filesystem::path realtivePath = Project::RemoveMarker(markedPath, marker);
		std::filesystem::path basePath = Project::GetAbsulteFilePathFormMarker(marker);
		std::filesystem::path pathAbosulte = GetPathAsGenaric(basePath / realtivePath);


		AbsolutePath = pathAbosulte;
		RealtivePath = realtivePath;
		PathMarker = markedPath;
		FilePath = marker == RY_PATH_ENGINE_MARKER_STR ? RealtivePath : AbsolutePath;
		SetDisc(true);
	}

	void AssetMetadata::SetMarkedFilePath(const std::filesystem::path& markedPath, const std::filesystem::path& path)
	{
		SetMarkedFilePath(markedPath);
		if (path != AbsolutePath && path != RealtivePath)
		{
			RY_CORE_ERROR("Marked File path not korekt Interpreted! {} as {}", markedPath, path);
			SetFilePath(path);
		}
	}
}