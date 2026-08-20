#include "rypch.h"
#include "Path.h"
#include <Rynex/Project/Project.h>

namespace Rynex {
	

	Path::Path(const Path& path)
		: m_Path(path.m_Path)
		, m_Origne(path.m_Origne)
	{
	}

	Path::Path(const std::string& path, Origne origne)
		: m_Path(path)
		, m_Origne(origne)
	{
		ConvertInternalPath();
	}

	Path::Path(const std::filesystem::path& path, Origne origne)
		: m_Path(path)
		, m_Origne(origne)
	{
		ConvertInternalPath();
	}

	Path::Path(const char* path, Origne origne)
		: m_Path(path)
		, m_Origne(origne)
	{
		ConvertInternalPath();
	}

	Path::~Path()
	{
	}

	bool Path::IsMarked() const
	{
		return IsOrignePathMarked(m_Origne);
	}

	bool Path::IsRealtive() const
	{
		return m_Path.is_relative();
	}

	bool Path::IsAbsoulte() const
	{
		return m_Path.is_absolute();
	}

	bool Path::IsExisting() const
	{
		return std::filesystem::exists(m_Path);
	}

	bool Path::IsDirectory() const
	{
		return std::filesystem::is_directory(m_Path);
	}

	bool Path::IsFile() const
	{
		return m_Path.has_filename();
	}

	bool Path::IsAssetExtension() const
	{
		return AssetType::None != Asset::GetAssetTypeFromFilePath(m_Path);
	}

	uint64_t Path::GetHash() const
	{
		uint64_t hash = robin_hood::hash<std::filesystem::path>{}(m_Path);
		return hash;
	}

	int16_t Path::GetState() const
	{
		int16_t state = FindeMarkedPathInlineState(m_Path, m_Origne);
		return state;
	}

	Path::Origne Path::GetMarkerOrigine() const
	{
		return m_Origne;
	}

	

	const std::filesystem::path& Path::GetPath() const
	{
		return m_Path;
	}

	std::filesystem::path Path::GetAbsolutePath() const
	{
		return m_Path;
	}

	std::filesystem::path Path::GetRelativePath() const
	{
		return m_Path;
	}

	std::filesystem::path Path::GetMarkedPath() const
	{
		std::filesystem::path markedPath = GetResolveAbsoluteToMarkedPath(m_Path, m_Origne);
		ConvertUniverselPath(markedPath);
		return markedPath;
	}

	std::string Path::GetAbsolutePathString() const
	{
		return GetAbsolutePath().string();
	}

	std::string Path::GetRelativePathString() const
	{
		return GetRelativePath().string();
	}

	std::string Path::GetMarkedPathString() const
	{
		return GetMarkedPath().string();
	}


	void Path::SetMarker(Origne origne)
	{
		std::string_view viewNext = magic_enum::enum_name(origne);
		std::string_view viewCurent = magic_enum::enum_name(m_Origne);

		RY_CORE_WARN("SetMarker overides the last Origne! {} -> {}", viewNext, viewCurent);
		m_Origne = origne;
	}

	

	std::filesystem::path Path::GetProjectDiretory()
	{
		return Project::GetActiveProjectDirectory();
	}

	std::filesystem::path Path::GetEngineDiretory()
	{
		RY_CORE_WARN("Engine Directory is curetly GetWorkingDiretory.");
		return GetWorkingDiretory();
	}

	std::filesystem::path Path::GetWorkingDiretory()
	{
		std::filesystem::path workingDiretory = std::filesystem::current_path();
		ConvertUniverselPath(workingDiretory);
		return workingDiretory;
	}

	

	void Path::ConvertPathNoMarker()
	{
		int16_t state = State::None;
		
		if (m_Path.empty())
			return;
		Origne orignePath = GetPathOrignFromMarkerPath(m_Path);
		std::filesystem::path pathCopy = m_Path;
		if (!IsOrignePathMarked(orignePath) && IsOrignePathMarked(m_Origne))
			return;

		RY_CORE_WARN_IF(!IsOrignePathMarked(orignePath) && !IsOrignePathMarked(m_Origne), "Multyple marked path Origne is not the default case curently we overide the case befor!");
		
		switch (orignePath)
		{
			case Origne::None:
			{
				// TODO implement finde Origen
				RY_CORE_ASSERT(false, "No implement find a none marked path!");
				break;
			}
			case Origne::Unknown:
			{
				state |= State::MarkedInline | State::MarkedUnknown;
				m_Path = RemovePathMarker(m_Path, orignePath);
				m_Origne = orignePath;
				break;
			}
			case Origne::Engine:
			{
				state |= State::MarkedInline | State::MarkedEngine;
				m_Path = RemovePathMarker(m_Path, orignePath);
				m_Origne = orignePath;
				break;
			}
			case Origne::Project:
			{
				state |= State::MarkedInline | State::MarkedProject;
				m_Path = RemovePathMarker(m_Path, orignePath);
				m_Origne = orignePath;
				break;
			}
			default:
			{
				std::string_view view = magic_enum::enum_name(orignePath);
				RY_CORE_ERROR("Origne-path: {}, path: {}", view, pathCopy);
				RY_CORE_ASSERT(false, "Not expexted State!");
				return;
			}
		}
	}

	void Path::ConvertAboultePath()
	{
		bool isAbsolute = m_Path.is_absolute();
		bool isRelative = m_Path.is_relative();
		RY_CORE_ASSERT(isAbsolute != isRelative, "Both path shoud never be equeal!");
		if (!isAbsolute && isRelative)
		{
			m_Path = GetResolveReltivePathToAbsoluteFromOrigne(m_Path, m_Origne);
		}
		
	}

	void Path::ConvertUniverselPath()
	{
		ConvertUniverselPath(m_Path);
	}

	void Path::ConvertInternalPath()
	{
		ConvertPathNoMarker();
		ConvertAboultePath();
		ConvertUniverselPath();
	}

	std::filesystem::path Path::GetResolveMarkerPath(Origne origne) const
	{
		return GetResolveReltivePathToAbsoluteFromOrigne(m_Path, origne);
	}

	std::filesystem::path Path::GetResolveAbsoluteToMarkedPath(Origne origne) const
	{
		return GetResolveAbsoluteToMarkedPath(m_Path, origne);
	}

	std::filesystem::path Path::ClearOringenMarker(Origne origne) const
	{
		std::filesystem::path reltivePath = RemovePathMarker(m_Path, origne);
		std::filesystem::path absolutePath = GetResolveReltivePathToAbsoluteFromOrigne(reltivePath, origne);
		ConvertUniverselPath(absolutePath);
		return absolutePath;
	}

	std::filesystem::path Path::GetRelativePathFromAbsoultePath() const
	{
		Origne origne = GetExpextedOrigineFromAbsoultePath(m_Path);
		std::filesystem::path absolutePath = GetPathAbsoluteMarker(origne);
		std::filesystem::path relativePath = std::filesystem::relative(m_Path, absolutePath);
		return relativePath;
	}

	int16_t Path::FindeMarkedPathInlineState(const std::filesystem::path& path, Origne origne)
	{
		int16_t state = State::None;
		if (path.empty()) 
		{
			return state;
		}

		Origne orignePath = GetMarkedPathOrigine(path);
		std::filesystem::path noInlineMarkedPath;
		switch (orignePath)
		{
			case Origne::None:
			{
				noInlineMarkedPath = path;
				break;
			}
			case Origne::Unknown:
			{
				state |= State::MarkedInline | State::MarkedUnknown;
				noInlineMarkedPath = RemovePathMarker(path, orignePath);
				origne = orignePath;
				break;
			}
			case Origne::Engine:
			{
				state |= State::MarkedInline | State::MarkedEngine;
				noInlineMarkedPath = RemovePathMarker(path, orignePath);
				origne = orignePath;
				break;
			}
			case Origne::Project:
			{
				state |= State::MarkedInline | State::MarkedProject;
				noInlineMarkedPath = RemovePathMarker(path, orignePath);
				origne = orignePath;
				break;
			}
			default:
			{
				std::string_view view = magic_enum::enum_name(orignePath);
				RY_CORE_ERROR("Origne-path: {}, path: {}", view, path);
				RY_CORE_ASSERT(false, "Not expexted State!");
				return State::None;
			}
		}
		
		state |= FindeFilesystemStateState(noInlineMarkedPath, origne);


		return state;
	}

	int16_t Path::FindeFilesystemStateState(const std::filesystem::path& noInlineMarkedPath, Origne origne)
	{
		int16_t state = State::None;
		std::filesystem::path absoultePath;
		if (noInlineMarkedPath.is_absolute()) 
		{
			absoultePath = noInlineMarkedPath;
			state |= State::Absoulte;
		}
		if (noInlineMarkedPath.is_relative()) 
		{
			absoultePath = GetResolveReltivePathToAbsoluteFromOrigne(noInlineMarkedPath, origne);
			state |= State::Realtive;
		}
		if (std::filesystem::exists(absoultePath)) 
		{
			state |= State::Existing;
		}
		if (std::filesystem::is_directory(absoultePath))
		{
			state |= State::Directory;
		}
		else 
		{
			state |= State::File;
			if (absoultePath.has_extension()) 
			{
				state |= State::Extension;
				AssetType assetType = Asset::GetAssetTypeFromFilePath(absoultePath);
				state |= assetType != AssetType::None ? State::AssetFileExtension : State::None;
			}
		}

		return state;
	}


	std::filesystem::path Path::GetResolveReltivePathToAbsoluteFromOrigne(const std::filesystem::path& path, Origne origne)
	{
		std::filesystem::path absoluteMarkerPath = GetPathAbsoluteMarker(origne);
		std::filesystem::path resolvePath = absoluteMarkerPath / path;
		ConvertUniverselPath(resolvePath);
		return resolvePath;
	}

	std::filesystem::path Path::GetResolveAbsoluteToMarkedPath(const std::filesystem::path& path, Origne origne)
	{
		std::filesystem::path absolutePath = GetPathAbsoluteMarker(origne);
		std::filesystem::path relativePath = std::filesystem::relative(absolutePath, path);
		if (Origne::Engine == origne) 
		{
			std::filesystem::path reltiveExpextedPathBegin = std::filesystem::path(RY_PATH_EXPEXT_ENGINE_RELATIV_START_STR);
			relativePath = reltiveExpextedPathBegin / relativePath;
		}
		int8_t index = GetOrigneIndex(origne);
		std::filesystem::path marker = std::filesystem::path(PATH_MARKER_STR[index]);
		std::filesystem::path markeredRelativePath = marker / relativePath;
		ConvertUniverselPath(markeredRelativePath);
		return markeredRelativePath;
	}

	uint32_t Path::GetMarkerCarkterCount(Origne origne)
	{
		int8_t origneIndex = GetOrigneIndex(origne);
		uint32_t count = GetMarkerLength(PATH_MARKER_STR[origneIndex]);
		return count;
	}

	bool Path::IsPathMarked(const std::filesystem::path& markedPath)
	{
		Origne origne = GetMarkedPathOrigine(markedPath);
		return IsOrignePathMarked(origne);
	}

	bool Path::IsOrignePathMarked(Origne origne)
	{
		return Origne::None != origne;
	}

	std::filesystem::path Path::RemovePathMarker(const std::filesystem::path& path)
	{
		Origne origne = GetMarkedPathOrigine(path);
		return RemovePathMarker(path, origne);
	}

	std::filesystem::path Path::RemovePathMarker(const std::filesystem::path& markedPath, Origne origne)
	{
		uint32_t markerCount = GetMarkerCarkterCount(origne);
		markerCount += 1u;
		std::string markerPathStr = markedPath.string();
		std::string pathStr = markerPathStr.substr(markerCount);
		std::filesystem::path path = pathStr;
		return path;
	}

	std::filesystem::path Path::GetReltivePath(const std::filesystem::path& path)
	{
		Origne origne = GetMarkedPathOrigine(path);
		RY_CORE_ASSERT(IsOrignePathMarked(origne), "file Path not Marked!");
		std::filesystem::path reltivePath = RemovePathMarker(path, origne);
		return reltivePath;
	}

	std::filesystem::path Path::GetReltivePath(const std::filesystem::path& path, const std::filesystem::path& origene)
	{
		if (IsPathMarked(path))
		{
			return GetReltivePath(path);
		}
		std::filesystem::path reltivePath;
		if (path.is_relative()) 
		{
			reltivePath = path;
			return reltivePath;
		}
		reltivePath = std::filesystem::relative(path, origene);
		return reltivePath;
	}

	std::filesystem::path Path::GetPathAbsoluteMarker(Origne origne)
	{
		switch (origne)
		{
			case Path::Origne::None:	return std::filesystem::path("");
			case Path::Origne::Unknown:	return std::filesystem::path("");
			case Path::Origne::Engine:	
			{
				std::filesystem::path enginePath = GetEngineDiretory();
				ConvertUniverselPath(enginePath);
				return enginePath;
			}
			case Path::Origne::Project:
			{
				std::filesystem::path projectPath = GetProjectDiretory();
				ConvertUniverselPath(projectPath);
				return projectPath;
			}		
			default:
				RY_CORE_ASSERT(false, "Not defined Oringe!");
		}
		return std::filesystem::path("");
	}

	Path::Origne Path::GetMarkedPathOrigine(const std::filesystem::path& markedPath)
	{
		Origne origne = GetPathOrignFromMarkerPath(markedPath);
		if (IsOrignePathMarked(origne))
		{
			return origne;
		}
		RY_CORE_ASSERT(false, "not implement, marker set!");
		return Origne::None;
	}

	Path::Origne Path::GetPathOrignFromMarkerPath(const std::filesystem::path& markedPath)
	{
		int8_t i = PATH_MARKER_COUNT - 1;
		while (1 <= i && !Project::HasStringInPath(markedPath, PATH_MARKER_STR[i]))
		{
			i--;
		}
		if (i < 0)
		{
			RY_CORE_ERROR("Diden't found a vaild Marker! {}", i);
			Origne origne = Origne::None;
			return origne;
		}
		Origne origne = static_cast<Origne>(i);
		return origne;
	}

	Path::Origne Path::GetExpextedOrigineFromRelativePath(const std::filesystem::path& path)
	{
		int8_t i = 0;
		const std::string relativePathSearch[] = {
			RY_PATH_EXPEXT_ENGINE_RELATIV_START_STR,
			"",
		};
		while ( i < 2 && !Project::HasStringInPath(path, relativePathSearch[i]))
		{
			i++;
		}
		if (2 <= i)
		{
			RY_CORE_ERROR("Diden't found a vaild Marker! {}", i);
			Origne origne = Origne::None;
			return origne;
		}
		int8_t origneIndex = PATH_MARKER_COUNT - i;
		Origne origne = static_cast<Origne>(origneIndex);
		return origne;
	}

	Path::Origne Path::GetExpextedOrigineFromAbsoultePath(const std::filesystem::path& path)
	{
		int8_t i = 0;
		const std::string absoulutePathSearch[] = {
			GetProjectDiretory().string(),
			GetEngineDiretory().string(),
		};
		while (i < 2 && !Project::HasStringInPath(path, absoulutePathSearch[i]))
		{
			i++;
		}
		if ( 2 <= i)
		{
			RY_CORE_ERROR("Diden't found a vaild Marker! {}", i);
			Origne origne = Origne::None;
			return origne;
		}
		int8_t origneIndex = PATH_MARKER_COUNT - i;
		Origne origne = static_cast<Origne>(origneIndex);
		return origne;
	}

	void Path::ConvertUniverselPath(std::filesystem::path& path)
	{
		std::filesystem::path normalizedPath = path.lexically_normal();
		std::string genaricPathStr = normalizedPath.generic_string();
		path = genaricPathStr;
	}



}