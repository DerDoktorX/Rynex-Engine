#include "rypch.h"
#include "Path.h"
#include <Rynex/Project/Project.h>

namespace Rynex {
	

	Path::Path(const Path& path)
		: m_Path(path.m_Path)
	{
	}

	Path::Path(const std::string& path)
		: m_Path(path)
	{
		ConvertUniverselPath();
	}

	Path::Path(const std::filesystem::path& path)
		: m_Path(path)
	{
		ConvertUniverselPath();
	}

	Path::Path(const char* path)
		: m_Path(path)
	{
		ConvertUniverselPath();
	}

	Path::~Path()
	{
	}

	bool Path::IsMarker() const
	{
		return IsPathMarked(m_Path);
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

	Path::Origne Path::GetMarkerOrigine() const
	{
		Origne origne = GetPathOrignFromMarkerPath(m_Path);
		return origne;
	}

	Path::Origne Path::GetExpextedMarkerOrigne() const
	{
		Origne origne = GetMarkerOrigine();
		if (IsOrignePathMarked(origne)) {
			return origne;
		}
		RY_CORE_ASSERT(false, "not implement, marker set!");
		return origne;
	}


	void Path::SetMarker(Origne origne)
	{
		if (IsMarker())
			ClearOringenMarker();

		m_Path = GetResolveMarkerPath(origne);
	}

	void Path::GenertaMarker()
	{
		RY_CORE_ASSERT(false, "not implement!");
	}

	void Path::ClearOringenMarker()
	{
		Origne origne = GetMarkerOrigine();
		if (IsOrignePathMarked(origne))
			return;

		m_Path = RemovePathMarker(m_Path, origne);
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

	void Path::ConvertUniverselPath()
	{
		ConvertUniverselPath(m_Path);
	}

	std::filesystem::path Path::GetResolveMarkerPath(Origne origne) const
	{
		std::filesystem::path absoluteMarkerPath = GetPathAbsoluteMarker(origne);
		std::filesystem::path resolvePath = absoluteMarkerPath / m_Path;
		ConvertUniverselPath(resolvePath);
		return resolvePath;
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
		if (IsPathMarked(path)) {
			return GetReltivePath(path);
		}
		std::filesystem::path reltivePath;
		if (path.is_relative()) {
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
		if (IsOrignePathMarked(origne)) {
			return origne;
		}
		RY_CORE_ASSERT(false, "not implement, marker set!");
		return Origne::None;
	}

	Path::Origne Path::GetPathOrignFromMarkerPath(const std::filesystem::path& markedPath)
	{
		int8_t i = PATH_MARKER_COUNT - 1;
		while (0 <= i && !Project::HasStringInPath(markedPath, PATH_MARKER_STR[i]))
		{
			i--;
		}
		if (0 <= i)
		{
			RY_CORE_ERROR("Diden't found a vaild Marker! {}", i);
			Origne origne = Origne::None;
			return origne;
		}
		Origne origne = static_cast<Origne>(i);
		return origne;
	}

	Path::Origne Path::GetExpextedOrigineFromPath(const std::filesystem::path& path)
	{
		int8_t i = PATH_MARKER_COUNT - 1;
		const std::string relativePathSearch[] = {
			RY_PATH_EXPEXT_ENGINE_RELATIV_START_STR,
			"",
		};
		while (PATH_MARKER_INDEX <= i && !Project::HasStringInPath(path, PATH_MARKER_STR[i]))
		{
			i--;
		}
		if (PATH_MARKER_INDEX <= i)
		{
			RY_CORE_ERROR("Diden't found a vaild Marker! {}", i);
			Origne origne = Origne::None;
			return origne;
		}

		Origne origne = static_cast<Origne>(i);
		return origne;
	}

	void Path::ConvertUniverselPath(std::filesystem::path& path)
	{
		std::string genaricPathStr = path.generic_string();
		path = genaricPathStr;
	}



}