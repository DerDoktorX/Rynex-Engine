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
		return Origne::None != GetMarkerOrigine();
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
		constexpr uint32_t markerCount = 4;
		constexpr uint32_t markerVaildCount = 2;
		constexpr const char* markersArray[markerCount] = {
			RY_PATH_PROJECT_MARKER_STR,
			RY_PATH_ENGINE_MARKER_STR,
			RY_PATH_NO_VAILD_MARKER_STR,
			""
		};

		uint32_t i = 0;
		while (i < markerCount && !Project::HasStringInPath(m_Path, markersArray[i]))
		{
			i++;
		}
		if (markerVaildCount < i)
		{
			RY_CORE_ERROR("Diden't found a vaild Marker! {}", i);
			Origne origne = Origne::None;
			return origne;
		}
		Origne origne = static_cast<Origne>(i);
		return origne;
	}

	Path::Origne Path::GetMarkerExpextedPath() const
	{
		Origne origne = GetMarkerOrigine();
		if (Origne::None != origne) {
			return origne;
		}
		RY_CORE_ASSERT(false, "not implement, marker set!");

		return origne;
	}


	void Path::SetMarker(Origne origne)
	{
		if (IsMarker()) {
			ClearOringenMarker();
		}
		constexpr const char* markersArray[] = {
			RY_PATH_NO_VAILD_MARKER_STR
			RY_PATH_PROJECT_MARKER_STR,
			RY_PATH_ENGINE_MARKER_STR,
		};
		uint32_t index = static_cast<uint32_t>(origne);

		std::string pathStr = m_Path.string() + markersArray[index];
		m_Path = pathStr;
	}

	void Path::GenertaMarker()
	{
		RY_CORE_ASSERT(false, "not implement!");
	}

	void Path::ClearOringenMarker()
	{
		Origne origne = GetMarkerOrigine();
		if (Origne::None != origne)
			return;

		RY_CORE_ASSERT(false, "not implement, marker set!");
	}

	void Path::ConvertUniverselPath()
	{
		std::string genaricPathStr = m_Path.generic_string();
		m_Path = genaricPathStr;
	}
}