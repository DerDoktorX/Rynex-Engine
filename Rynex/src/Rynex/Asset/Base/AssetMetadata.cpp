#include "rypch.h"
#include "AssetMetadata.h"
#include <Rynex/Project/Project.h>

namespace Rynex {

	void AssetMetadata::SetFilePath(const std::filesystem::path& path)
	{

		FileSystem::Path fileSystemPath = FileSystem::Path(AbsolutePath);
		RealtivePath = fileSystemPath.GetRelativePath();
		AbsolutePath = fileSystemPath.GetAbsolutePath();
		PathMarker = fileSystemPath.GetMarkedPathString();


		

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
	    FileSystem::Path path(markedPathStr);
	    SetFilePath(path);
	}

	void AssetMetadata::SetMarkedFilePath(const std::string& markedPathStr, const std::filesystem::path& path)
	{
	    FileSystem::Path pathMarked(markedPathStr);
	    FileSystem::Path pathSystem(path);
	    RY_CORE_ASSERT(pathMarked == pathSystem, "The two path should be end up as the same!");
	}
}